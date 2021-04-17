#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため
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


static void dispSpeed(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static void disp7seg(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static unsigned char	makeTestDataSpeed( void );
static unsigned char	makeTestDataSpeedManual( void );
//********************************************************************************
// 初期化
//********************************************************************************
void initAplDispData( void )
{
	unsigned char	i;
	
	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		aplDispData.led7Seg[i]	= APL_DSP_DATA_7SEG_0;
	}
	aplDispData.settingMode	= APL_DISP_SETTING_OFF;
	aplDispData.color7seg	= APL_DISP_DATA_WHITE;

	dispCycSpeed	= 0;

	testCycSpeed	= 0;
	testSpeed		= 0;
	testStateSpeed	= TEST_STATE_UP;
	
	//debug
	dispSpeed( &aplDispData.led7Seg[0] , 123);
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
	//aplDispData.color7seg			= inAplCtrlSet->color7seg;

	//------------------------------
	// 輝度データ出力
	//------------------------------
	if( inAplDataCar->ill == APL_DATA_ILL_OFF ){
		aplDispData.bright7seg		= inAplCtrlSet->bright7seg/3;
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
		aplDispData.settingMode		= APL_DISP_SETTING_OFF;
		
		//dispSpeed( &aplDispData.led7Seg[0] , inAplDataCar->speed );

		break;

	//****************************************
	// テストモード
	//****************************************
	case APL_CTRL_STATE_TESTDISP:		//テストモード
		aplDispData.settingMode		= APL_DISP_SETTING_OFF;
		
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
		//設定モード表示ON
		aplDispData.settingMode		= APL_DISP_SETTING_ON;

		switch( inAplCtrl->stateSet){
		case APL_CTRL_STATE_SET_COLOR_7SEG:		//調色
			disp7seg( &aplDispData.led7Seg[0] , SET_BRIGHT_7SEG_DISP );
			break;
		case APL_CTRL_STATE_SET_BRIGHT_7SEG:		//調光(7セグ
			disp7seg( &aplDispData.led7Seg[0] , SET_BRIGHT_7SEG_DISP );
			break;
		case APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG:		//調光減光(7セグ
			disp7seg( &aplDispData.led7Seg[0] , SET_BRIGHT_7SEG_DISP );
			break;
		case APL_CTRL_STATE_SET_DISPCYC_7SEG:		//表示更新速度(7セグ
			disp7seg( &aplDispData.led7Seg[0], inAplCtrlSet->dispcyc7seg * SPEED_DIGIT );
			break;
		case APL_CTRL_STATE_SET_PALSE_SPEED:			//パルス仕様車速 
			disp7seg( &aplDispData.led7Seg[0] , SETTING_PALSE_SPEED[inAplCtrlSetPalse->speed] );
			break;
		case APL_CTRL_STATE_SET_PALSE_REV:			//パルス仕様回転数 
			disp7seg( &aplDispData.led7Seg[0] , SETTING_PALSE_REV[inAplCtrlSetPalse->rev] );
			break;
		default:
			break;
		}
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
