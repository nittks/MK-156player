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
static unsigned short	dispCycRev;

//テストデータ
static unsigned char	testCycSpeed;
static unsigned char	testCycRev;
static unsigned char	testSpeed;
static unsigned short	testRev;
static TEST_STATE		testStateSpeed;
static TEST_STATE		testStateRev;


static void dispSpeed(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static void dispRev(	unsigned char *retRev	, const unsigned short inRev	);
static void disp7seg(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static void dispBarLed(	unsigned char *retRev	, const unsigned short inRev	);
static unsigned char	makeTestDataSpeed( void );
static unsigned short	makeTestDataRev( void );
static unsigned char	makeTestDataSpeedManual( void );
static unsigned short	makeTestDataRevManual( void );
//********************************************************************************
// 初期化
//********************************************************************************
void initAplDispData( void )
{
	unsigned char	i;
	
	for( i=0 ; i<LED_7SEG_NUM ; i++ ){
		aplDispData.led7Seg[i]	= APL_DSP_DATA_7SEG_0;
	}
	aplDispData.barLedOnNum	= 0;
	aplDispData.settingMode	= APL_DISP_SETTING_OFF;
	aplDispData.settingNo	= APL_DISP_SETTING_NO_BRIGHT_7SEG;

	dispCycSpeed	= 0;
	dispCycRev		= 0;

	testCycSpeed	= 0;
	testCycRev		= 0;
	testSpeed		= 0;
	testRev			= 0;
	testStateSpeed	= TEST_STATE_UP;
	testStateRev	= TEST_STATE_UP; 
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
	unsigned short		tmpRev;

	inAplDataCar		= getAplDataCar();
	inAplCtrl			= getAplCtrl();
	inAplCtrlSet		= getAplCtrlSet();
	inAplCtrlSetPalse	= getAplCtrlSetPalse();
	
	//------------------------------
	// 輝度データ出力
	//------------------------------
	if( inAplDataCar->ill == APL_DATA_ILL_OFF ){
		aplDispData.bright7seg		= inAplCtrlSet->bright7seg;
		aplDispData.brightBarled	= inAplCtrlSet->brightBarled;
	}else{
		aplDispData.bright7seg		= inAplCtrlSet->brightDim7seg;
		aplDispData.brightBarled	= inAplCtrlSet->brightDimBarled;
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
		
		dispSpeed( &aplDispData.led7Seg[0] , inAplDataCar->speed );
		dispRev( &aplDispData.barLedOnNum , inAplDataCar->rev );
		break;

	//****************************************
	// テストモード
	//****************************************
	case APL_CTRL_STATE_TESTDISP:		//テストモード
		aplDispData.settingMode		= APL_DISP_SETTING_OFF;
		
		switch( inAplCtrl->stateTest ){
		case APL_CTRL_STATE_TEST_AUTO:
			tmpSpeed	= makeTestDataSpeed();
			tmpRev		= makeTestDataRev();
			dispSpeed( &aplDispData.led7Seg[0] , tmpSpeed );
			dispRev( &aplDispData.barLedOnNum , tmpRev );
			break;
		case APL_CTRL_STATE_TEST_SPEED:
			tmpSpeed	= makeTestDataSpeedManual();
			dispSpeed( &aplDispData.led7Seg[0] , tmpSpeed );
			dispRev( &aplDispData.barLedOnNum , testRev );
			break;
		case APL_CTRL_STATE_TEST_REV:
			tmpRev		= makeTestDataRevManual();
			dispSpeed( &aplDispData.led7Seg[0] , testSpeed );
			dispRev( &aplDispData.barLedOnNum , tmpRev );
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
		case APL_CTRL_STATE_SET_BRIGHT_7SEG:		//調光(7セグ
			aplDispData.settingNo = APL_DISP_SETTING_NO_BRIGHT_7SEG;
			disp7seg( &aplDispData.led7Seg[0] , SET_BRIGHT_7SEG_DISP );
			dispBarLed( &aplDispData.barLedOnNum , inAplCtrlSet->bright7seg * REV_DIGIT );
			break;
		case APL_CTRL_STATE_SET_BRIGHT_BARLED:		//調光(バーLED
			aplDispData.settingNo = APL_DISP_SETTING_NO_BRIGHT_BARLED;
			disp7seg( &aplDispData.led7Seg[0] , inAplCtrlSet->brightBarled * SPEED_DIGIT );
			dispBarLed( &aplDispData.barLedOnNum , SET_BRIGHT_BARLED_DISP ); 
			break;
		case APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG:		//調光減光(7セグ
			aplDispData.settingNo = APL_DISP_SETTING_NO_BRIGHT_7SEG;
			disp7seg( &aplDispData.led7Seg[0] , SET_BRIGHT_7SEG_DISP );
			dispBarLed( &aplDispData.barLedOnNum , inAplCtrlSet->brightDim7seg * REV_DIGIT );
			break;
		case APL_CTRL_STATE_SET_BRIGHT_DIM_BARLED:	//調光減光(バーLED
			aplDispData.settingNo = APL_DISP_SETTING_NO_BRIGHT_BARLED;
			disp7seg( &aplDispData.led7Seg[0] , inAplCtrlSet->brightDimBarled * SPEED_DIGIT );
			dispBarLed( &aplDispData.barLedOnNum , SET_BRIGHT_BARLED_DISP );
			break;
		case APL_CTRL_STATE_SET_DISPCYC_7SEG:		//表示更新速度(7セグ
			aplDispData.settingNo = APL_DISP_SETTING_NO_DISP_CYC_7SEG;
			tmpSpeed = makeTestDataSpeed();
			dispSpeed( &aplDispData.led7Seg[0] , tmpSpeed );
			dispBarLed( &aplDispData.barLedOnNum , inAplCtrlSet->dispcyc7seg * REV_DIGIT );
			break;
		case APL_CTRL_STATE_SET_DISPCYC_BARLED:		//表示更新速度(バーLED
			aplDispData.settingNo = APL_DISP_SETTING_NO_DISP_CYC_BARLED;
			tmpRev = makeTestDataRev();
			disp7seg( &aplDispData.led7Seg[0] , inAplCtrlSet->dispcycBarled * SPEED_DIGIT );
			dispRev( &aplDispData.barLedOnNum , tmpRev ); 
			break;
		case APL_CTRL_STATE_SET_PALSE_SPEED:			//パルス仕様車速 
			aplDispData.settingNo = APL_DISP_SETTING_NO_DISP_PALSE_SPEED;
			disp7seg( &aplDispData.led7Seg[0] , SETTING_PALSE_SPEED[inAplCtrlSetPalse->speed] );
			aplDispData.barLedOnNum	= 0;
			break;
		case APL_CTRL_STATE_SET_PALSE_REV:			//パルス仕様回転数 
			aplDispData.settingNo = APL_DISP_SETTING_NO_DISP_PALSE_REV;
			disp7seg( &aplDispData.led7Seg[0] , SETTING_PALSE_REV[inAplCtrlSetPalse->rev] );
			aplDispData.barLedOnNum	= 0;
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
	for( i=0 ; i<LED_7SEG_NUM ; i++ ){
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
// 回転数表示
//********************************************************************************
static void dispRev( unsigned char *retRev , const unsigned short inRev )
{
	APL_CTRL_SET		*inAplCtrlSet;
	inAplCtrlSet		= getAplCtrlSet();

	//表示更新周期待機(設定値[100%]単位*1%辺りの時間→時間を出し、比較
	if( dispCycRev < ( inAplCtrlSet->dispcycBarled * DISPCYC_BARLED_DIGIT ) ){
		dispCycRev++;
	}else{
		dispCycRev = 0;

		dispBarLed( retRev , inRev );
	}
}
//********************************************************************************
// バーLED表示
//********************************************************************************
static void dispBarLed( unsigned char *retRev , const unsigned short inRev )
{
	//回転数表示処理
	//LEDが1つ以上点灯する
	if( inRev > REV_PER_SEG ){
		//点灯させるセグメント数 = 回転数 / 1セグ辺りの回転数
		*retRev = inRev / REV_PER_SEG;
	}else{
		*retRev = 0;
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
// 回転数テストデータ
//********************************************************************************
static unsigned short makeTestDataRev( void )
{
	if( testCycRev < TEST_CYC_REV ){
		testCycRev++;
	}else{
		testCycRev = 0;
		if( testStateRev == TEST_STATE_UP ){
			if( testRev < (REV_MAX-CHG_VAL_REV)){
				//1msに1rpm増加
				testRev += CHG_VAL_REV;
			}else{
				//状態を減少方向へ
				testRev = REV_MAX;
				testStateRev	= TEST_STATE_DOWN;
			}
		}else if( testStateRev == TEST_STATE_DOWN ){
			if( testRev > (REV_MIN+CHG_VAL_REV)){
				//1msに1rpm増加
				testRev -= CHG_VAL_REV;
			}else{
				//状態を減少方向へ
				testRev = REV_MIN;
				testStateRev	= TEST_STATE_UP;
			}
		}
	}
	return( testRev );
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
// 回転数テストデータ(手動
//********************************************************************************
static unsigned short makeTestDataRevManual( void )
{
	APL_DATA_SW			*inAplDataSw;
	inAplDataSw		= getAplDataSw();

	if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_UP ){
		if( testRev < (REV_MAX-CHG_VAL_REV_MANUAL) ){
			//1msに1rpm増加
			testRev += CHG_VAL_REV_MANUAL;
		}else{
			testRev = REV_MAX;
		}
	}else if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_DOWN ){
		if( testRev > (REV_MIN+CHG_VAL_REV_MANUAL)){
			//1msに1rpm増加
			testRev -= CHG_VAL_REV_MANUAL;
		}else{
			testRev = REV_MIN;
		}
	}
	return( testRev );
}
