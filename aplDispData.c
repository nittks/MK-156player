#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "aplDispData_inc.h"
#include "aplDispData.h"
#include "aplData.h"
#include "aplCtrl.h"
#include "hardware.h"

//公開用
static APL_DISP_DATA	aplDispData;

//内部処理用
//通常時
static unsigned short	dispCycSpeed;

//テストデータ
static unsigned char	testCycSpeed;
static unsigned char	testSpeed;
static TEST_STATE		testStateSpeed;
static DISP_STATE		dispState;

static void dispSpeed(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static void disp7seg(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static unsigned char	makeTestDataSpeed( void );
static unsigned char	makeTestDataSpeedManual( void );
static bool valveChk( void );
static bool segRGBsequential( void );
static bool allSegRGBGradation( void );
static void convertHSVtoRGB(float h,float s,float v,float* r,float* g,float* b);
static void setRgb(float* r,float* g,float* b,float	val_0,float val_1,float val_2);
static void alloff( void );
static void firstPosDigitSeg( void );
static void firstColor( void );
static bool nextColor( void );
static bool nextSegDigitColor( void );

volatile static uint8_t	debugSpeed = 0;
//********************************************************************************
// 初期化
//********************************************************************************
void initAplDispData( void )
{
	unsigned char	i;
	
	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		aplDispData.led7Seg[i]	= APL_DSP_DATA_7SEG_BLANK;
	}
	
	aplDispData.red		= 0;
	aplDispData.green	= 0;
	aplDispData.blue	= 0;
	aplDispData.valveChkMode	= true;

	dispCycSpeed	= 0;

	testCycSpeed	= 0;
	testSpeed		= 0;
	testStateSpeed	= TEST_STATE_UP;
	dispState		= DISP_STATE_VALVE_CHK;
	
}
//********************************************************************************
// 取得
//********************************************************************************
APL_DISP_DATA *getAplDispData( void )
{
	return( &aplDispData );
}
//********************************************************************************
// メイン処理
//********************************************************************************
void aplDispDataMain( void )
{
	APL_DATA_CAR		*inAplDataCar;
	APL_CTRL			*inAplCtrl;
	APL_CTRL_SET		*inAplCtrlSet;
	APL_CTRL_SET_PALSE	*inAplCtrlSetPalse;
	
	unsigned char		tmpSpeed;

	inAplDataCar		= getAplDataCar();
	inAplCtrl			= getAplCtrl();
	inAplCtrlSet		= getAplCtrlSet();
	inAplCtrlSetPalse	= getAplCtrlSetPalse();
	

	//------------------------------
	// 調色データ出力
	//------------------------------
	if( dispState != DISP_STATE_VALVE_CHK ){
		if( inAplCtrlSet->colorNo == SETTING_COLOR_USER ){
			aplDispData.red		= inAplCtrlSet->colorRGB.red;
			aplDispData.green	= inAplCtrlSet->colorRGB.green;
			aplDispData.blue	= inAplCtrlSet->colorRGB.blue;
		}else{
			aplDispData.red		= COLOR_TABLE[inAplCtrlSet->colorNo].red;
			aplDispData.green	= COLOR_TABLE[inAplCtrlSet->colorNo].green;
			aplDispData.blue	= COLOR_TABLE[inAplCtrlSet->colorNo].blue;
		}
		
	}
	
	//------------------------------
	// 輝度データ出力
	//------------------------------
	if( inAplDataCar->ill == APL_DATA_ILL_OFF ){
		aplDispData.bright7seg		= inAplCtrlSet->bright7seg;
	}else{
		aplDispData.bright7seg		= inAplCtrlSet->brightDim7seg;
	}

	switch( inAplCtrl->state ){
	//****************************************
	// 初回起動
	//****************************************
	case APL_CTRL_STATE_BOOT:
		//無処理
		break;
	//****************************************
	// 通常
	//****************************************
	case APL_CTRL_STATE_NOMARL:		//通常
		switch( dispState ){
		case DISP_STATE_VALVE_CHK:
			if( valveChk() == VALVE_CHK_END ){		//valveChk()が値取得のみのように見えそう
				aplDispData.valveChkMode = false;
				dispState	= DISP_STATE_NORMAL;
			}
			break;
		case DISP_STATE_NORMAL:
			//dispSpeed( &aplDispData.led7Seg[0] , inAplDataCar->speed );
			dispSpeed( &aplDispData.led7Seg[0] , debugSpeed );
			break;
		default:
			break;
		}
		break;

	//****************************************
	// テストモード
	//****************************************
	case APL_CTRL_STATE_TESTDISP:		//テストモード
		
		switch( inAplCtrl->stateTest ){
		case APL_CTRL_STATE_TEST_AUTO:
			tmpSpeed	= makeTestDataSpeed();
			dispSpeed( &aplDispData.led7Seg[0] , tmpSpeed );
			break;
		case APL_CTRL_STATE_TEST_SPEED:
			tmpSpeed	= makeTestDataSpeedManual();
			dispSpeed( &aplDispData.led7Seg[0] , tmpSpeed );
			break;
		}
		break;
	//****************************************
	// 設定
	//****************************************
	case APL_CTRL_STATE_SETTING:		//設定
		//調光(減光)だけ上書き。上書き法はちょっと嫌
		if( inAplCtrl->stateSet == APL_CTRL_STATE_SETTING_BRIGHT_DIM ){
			aplDispData.bright7seg		= inAplCtrlSet->brightDim7seg;
		}else{
			aplDispData.bright7seg		= inAplCtrlSet->bright7seg;
		}
		
		
		//設定モード表示ON
		disp7seg( &aplDispData.led7Seg[0] , inAplCtrlSet->dspVal );

		break;
	}
}

//********************************************************************************
// 車速表示
//********************************************************************************
static void dispSpeed( unsigned char *retSpeed , const unsigned char inSpeed )
{
	APL_CTRL_SET	*inAplCtrlSet;

	inAplCtrlSet	= getAplCtrlSet();

	//表示更新周期待機(設定値[100%]単位*1%辺りの時間→時間を出し、比較
	if( dispCycSpeed < ( inAplCtrlSet->dispcyc7seg * DISPCYC_7SEG_DIGIT ) ){
		dispCycSpeed++;
	}else{
		dispCycSpeed = 0;

		disp7seg( retSpeed , inSpeed );
	}
	
}
//********************************************************************************
// 7seg表示
//********************************************************************************
static void disp7seg( unsigned char *retSpeed , const unsigned char inSpeed )
{
	unsigned char	i;
	unsigned char	tmpSpeed;

	//スピード表示処理
	tmpSpeed = inSpeed;	// /=で代入していくためワークへコピー
	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		//1桁抽出
		if( tmpSpeed > 0 ){
			//下の桁から計算
			retSpeed[i]	= tmpSpeed % 10;
			tmpSpeed	/= 10;
		}else{
			if( i==0 ){
				//1の位が0の時は0表示
				retSpeed[i]	= 0;
			}else{
				//10、100の位が0の時は非表示
				retSpeed[i]	= APL_DSP_DATA_7SEG_BLANK;
			}
		}
	}	
}
//********************************************************************************
// 車速テストデータ
//********************************************************************************
static unsigned char makeTestDataSpeed( void )
{
	if( testCycSpeed < TEST_CYC_SPEED ){
		testCycSpeed++;
	}else{
		testCycSpeed = 0;
		if( testStateSpeed == TEST_STATE_UP ){
			if( testSpeed < (SPEED_MAX-CHG_VAL_SPEED)){
				//1msに1rpm増加
				testSpeed += CHG_VAL_SPEED;
			}else{
				//状態を減少方向へ
				testSpeed = SPEED_MAX;
				testStateSpeed	= TEST_STATE_DOWN;
			}
		}else if( testStateSpeed == TEST_STATE_DOWN ){
			if( testSpeed > (SPEED_MIN+CHG_VAL_SPEED)){
				//1msに1rpm増加
				testSpeed -= CHG_VAL_SPEED;
			}else{
				//状態を減少方向へ
				testSpeed = SPEED_MIN;
				testStateSpeed	= TEST_STATE_UP;
			}
		}
	}
	return( testSpeed );
}
//********************************************************************************
// 車速テストデータ(手動
//********************************************************************************
static unsigned char makeTestDataSpeedManual( void )
{
	APL_DATA_SW			*inAplDataSw;
	inAplDataSw		= getAplDataSw();

	if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_UP ){
		if( testSpeed < (SPEED_MAX-CHG_VAL_SPEED_MANUAL) ){
			//1msに1rpm増加
			testSpeed += CHG_VAL_SPEED_MANUAL;
		}else{
			testSpeed = SPEED_MAX;
		}
	}else if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_DOWN ){
		if( testSpeed > (SPEED_MIN+CHG_VAL_SPEED_MANUAL)){
			//1msに1rpm増加
			testSpeed -= CHG_VAL_SPEED_MANUAL;
		}else{
			testSpeed = SPEED_MIN;
		}
	}
	return( testSpeed );
}
//********************************************************************************
// バルブチェック
//********************************************************************************
static bool valveChk( void )
{
	static uint8_t	step=0;
	static bool		endFlag = false;	
	
	switch( step ){
		case 0:if( segRGBsequential()	== VALVE_CHK_END )	step++;			break;
		case 1:if( allSegRGBGradation()	== VALVE_CHK_END )	endFlag = true;	break;
		default:	break;
	}
	return( endFlag );
}

//********************************************************************************
// 1セグメントずつ順に点消灯させる
// 直書きだと何をしているか分かりにくかったので関数化したが、
// 余計に分かりにくかったりしないか？
//********************************************************************************
static bool segRGBsequential( void )
{
	static	uint16_t	cntTimeMs = 0;
	bool	endFlag = false;

	if( cntTimeMs < VALVE_CHK_OFFTIME_MS ){
		alloff();
	}else if( cntTimeMs == VALVE_CHK_OFFTIME_MS ){
		firstPosDigitSeg();
		firstColor();
	}else if( cntTimeMs % VALVE_CHK_STEPTIME_MS == 0 ){
		endFlag = nextSegDigitColor();		//終了時間は規定しないため、工程終了した時点でendとする。
	}
	cntTimeMs += CYC_TIME_MS;
	
	return( endFlag );
}

static void alloff( void )
{
	aplDispData.digitBit	= 0;
	aplDispData.segBit		= 0;
	aplDispData.red			= 0;
	aplDispData.green		= 0;
	aplDispData.blue		= 0;
}

static void firstPosDigitSeg( void )
{
	aplDispData.segBit		= 1;
	aplDispData.digitBit	= (1 << (LED_7SEG_DIGIT_NUM-1));
}
static void firstColor( void )
{
	aplDispData.red			= VALVE_CHK_BRIGHTNESS;
	aplDispData.green		=   0;
	aplDispData.blue		=   0;
}
static bool nextSegDigitColor( void )
{
	bool	endFlag = false;
	
	if( aplDispData.segBit != (1 << LED_7SEG_SEG_NUM)){
		aplDispData.segBit <<= 1;
	}else{
		if( aplDispData.digitBit != 1 ){
			aplDispData.segBit		= 1;
			aplDispData.digitBit	>>= 1;
		}else{
			aplDispData.segBit		= 1;
			aplDispData.digitBit	= (1 << (LED_7SEG_DIGIT_NUM-1));
			endFlag = nextColor();
		}
	}
	return( endFlag );
}
static bool nextColor( void )
{
	bool	endFlag = false;
	
	if( aplDispData.red == VALVE_CHK_BRIGHTNESS ){
		aplDispData.red			=   0;
		aplDispData.green		= VALVE_CHK_BRIGHTNESS;
		aplDispData.blue		=   0;
	}else if( aplDispData.green == VALVE_CHK_BRIGHTNESS ){
		aplDispData.red			=   0;
		aplDispData.green		=   0;
		aplDispData.blue		= VALVE_CHK_BRIGHTNESS;
	}else if( aplDispData.blue == VALVE_CHK_BRIGHTNESS ){
		aplDispData.red			=   0;
		aplDispData.green		=   0;
		aplDispData.blue		=   0;
		endFlag = true;
	}
	return( endFlag );
}
//********************************************************************************
// 全セグメントをRGBグラデーションアニメーションする
// 元ネタはHSVで作り、RGB変換する。
// floatでは誤差が発生するが、表示で目に見える問題が確認されなかったのでこのまま。
//********************************************************************************
static bool allSegRGBGradation( void )
{	
	static float	h = 0;	// 0.00~1.00 
	static float	s = 0;
	static float	v = 0;
	float	r,g,b;
	static	uint16_t	cntTimeMs = 0;
		
	// vshの順で0.00~1.00まで加算する
	if( v < HSV_MAX ){
		v += HSV_CYC_ADD_V;
		v = ( v > HSV_MAX )? HSV_MAX : v;
	}else if( s < HSV_MAX ){
		s += HSV_CYC_ADD_S;
		s = ( s > HSV_MAX )? HSV_MAX : s;
	}else if( h < HSV_MAX ){
		h += HSV_CYC_ADD_H;
		h = ( h > HSV_MAX )? HSV_MAX : h;
	}
	
	convertHSVtoRGB( h,s,v,&r,&g,&b );
	
	aplDispData.red			= r * RGB_MAX;		// 0~1.00 -> 0~100
	aplDispData.green		= g * RGB_MAX;
	aplDispData.blue		= b * RGB_MAX;
	aplDispData.segBit		= pow(2,LED_7SEG_SEG_NUM)-1;		//全ON
	aplDispData.digitBit	= pow(2,LED_7SEG_DIGIT_NUM)-1;		//全ON
	
	if( h >= HSV_MAX ){
		return( VALVE_CHK_END );
	}else{
		return( VALVE_CHK_RUN );	
	}
}
//********************************************************************************
// 参考：検索ヒットした複数サイト。最終的にはwikiベース
// https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93
//********************************************************************************
static void convertHSVtoRGB(
	float  h,float	s,float  v,
	float* r,float*	g,float* b
){
	float	f,calcA,calcB,calcC,calcD;
	int		i;
	
	if( s <= 0 ){
		*r = *g = *b = v;
	}else{
		h	*= 6;
		i	= (int)h;
		f	= h - (float)i;
		
		calcA	= v;
		calcB	= v * (1-s);
		calcC	= v * (1-s*f);
		calcD	= v * (1-s*(1-f));	
		
		switch(i){
			case 0:	setRgb( r,g,b,  calcA,calcD,calcB );	break;
			case 1:	setRgb( r,g,b,  calcC,calcA,calcB );	break;
			case 2:	setRgb( r,g,b,  calcB,calcA,calcD );	break;
			case 3:	setRgb( r,g,b,  calcB,calcC,calcA );	break;
			case 4:	setRgb( r,g,b,  calcD,calcB,calcA );	break;
			case 5:	setRgb( r,g,b,  calcA,calcB,calcC );	break;
			default:break;
		}
	}
}
static void setRgb(
	float*	r,
	float*	g,
	float*	b,
	float	val_0,
	float	val_1,
	float	val_2
){
	*r	= val_0;
	*g	= val_1;
	*b	= val_2;
}
