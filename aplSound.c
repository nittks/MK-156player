#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "aplSound_inc.h"
#include "aplSound.h"
#include "aplData.h"
#include "aplCtrl.h"
#include "hardware.h"

//公開用
static APL_SOUND	aplSound;

//内部処理用
//通常時
static unsigned short	dispCycSpeed;

//テストデータ
static unsigned char	testCycSpeed;
static unsigned char	testSpeed;
static TEST_STATE		testStateSpeed;
static DISP_STATE		dispState;
static WATER_TEMP		waterTempState;

static void dispSpeed(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static void disp7seg(	unsigned char *retSpeed	, const unsigned char  inSpeed	);
static unsigned char	makeTestDataSpeed( void );
static unsigned char	makeTestDataSpeedManual( void );
static bool isErr( void );
static bool valveChk( void );
static bool segRGBsequential( void );
static bool allSegRGBGradation( void );
static void alloff( void );
static void firstPosDigitSeg( void );
static bool nextColor( void );
static bool nextSegDigitColor( void );

volatile static uint8_t	debugSpeed = 0;
//********************************************************************************
// 初期化
//********************************************************************************
void initAplSound( void )
{
	aplSound.waterOk		= false;

	waterTempState	= WATER_TEMP_STATE_LOW;
	
}
//********************************************************************************
// 取得
//********************************************************************************
APL_SOUND *getAplSound( void )
{
	return( &aplSound );
}
//********************************************************************************
// メイン処理
//********************************************************************************
void aplSoundMain( void )
{
	APL_CTRL		*inAplCtrl	= getAplCtrl();

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
		porcWaterTemp();
		break;
	
	default:
		break;
	}
}

static void porcWaterTemp( void )
{
	APL_DATA_CAR	*inAplDataCar	= getAplDataCar();
	
	if( waterTempState == WATER_TEMP_LOW ){
		if( inAplDataCar->waterTemp >= WATER_TEMP_NORMAL ){
			waterTempState	= WATER_TEMP_STATE_NORMAL;
			aplSound.waterOk	= true;
		}
	}else{
		aplSound.waterOk	= false;	//1発出しなので、遷移したらOFF
		if( inAplDataCar->waterTemp <= WATER_TEMP_LOW ){	//未満にしようかとも思ったけど以下で。
			waterTempState	= WATER_TEMP_STATE_LOW;
		}
	}
}		
