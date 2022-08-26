//#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "aplIndicator_inc.h"
#include "aplIndicator.h"
#include "aplData.h"
#include "aplCtrl.h"
#include "hardware.h"

//公開用
static APL_INDICATOR	aplIndicator;

static bool		waterTempState;
static bool		vtecOnState;
static bool		preVtc;
static uint16_t	vtecCoolTimeCnt;

static void porcWaterTemp( void );
static void procVtec( void );
static void procLed( void );

//********************************************************************************
// 初期化
//********************************************************************************
void initAplIndicator( void )
{
	aplIndicator.waterOk		= false;
	aplIndicator.vtec			= false;
	aplIndicator.ledGreen		= 0;
	aplIndicator.ledBlue		= 0;
	
	waterTempState		= WATER_TEMP_STATE_LOW;
	vtecOnState			= false;
	preVtc				= false;
	vtecCoolTimeCnt		= VTEC_COOL_TIME;
	
}
//********************************************************************************
// 取得
//********************************************************************************
APL_INDICATOR *getAplIndicator( void )
{
	return( &aplIndicator );
}
//********************************************************************************
// メイン処理
//********************************************************************************
void aplIndicatorMain( void )
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
		procVtec();
		procLed();
		break;
	
	default:
		break;
	}
}
//********************************************************************************
// 水温処理
//********************************************************************************
static void porcWaterTemp( void )
{
	APL_DATA_CAR	*inAplDataCar	= getAplDataCar();

	if( waterTempState == WATER_TEMP_STATE_LOW ){
		if( inAplDataCar->waterTemp >= WATER_TEMP_NORMAL ){
			waterTempState	= WATER_TEMP_STATE_NORMAL;
			aplIndicator.waterOk	= true;
		}
	}else{
		aplIndicator.waterOk	= false;	//1発出しなので、遷移したらOFF
		if( inAplDataCar->waterTemp <= WATER_TEMP_LOW ){	//未満にしようかとも思ったけど以下で。
			waterTempState	= WATER_TEMP_STATE_LOW;
		}
	}

}		
//********************************************************************************
// VTEC処理
//********************************************************************************
static void procVtec( void )
{
	APL_DATA_CAR	*inAplDataCar	= getAplDataCar();

	aplIndicator.vtec	= false;

	if( vtecCoolTimeCnt >= VTEC_COOL_TIME ){
		if( (inAplDataCar->vtc == true) && (preVtc == false) ){		// ONエッジ
			aplIndicator.vtec	= true;
			vtecCoolTimeCnt	= 0;
		}
	}else{
		aplIndicator.vtec	= false;
		vtecCoolTimeCnt++;
	}
	preVtc	= inAplDataCar->vtc;
}
//********************************************************************************
// LED処理
//********************************************************************************
static void procLed( void )
{

	APL_DATA_CAR	*inAplDataCar	= getAplDataCar();

	if( ( waterTempState == WATER_TEMP_STATE_LOW ) ||
		( inAplDataCar->vtc == true )
	){
		aplIndicator.ledGreen	= ( aplIndicator.ledGreen > 0				)? aplIndicator.ledGreen - LED_BRIGHT_STEP : 0;
	}else if( waterTempState == WATER_TEMP_STATE_NORMAL ){
		aplIndicator.ledGreen	= ( aplIndicator.ledGreen < LED_BRIGHT_MAX	)? aplIndicator.ledGreen + LED_BRIGHT_STEP : LED_BRIGHT_MAX;
	}

	if( inAplDataCar->vtc == false ){
		aplIndicator.ledBlue	= ( aplIndicator.ledBlue > 0				)? aplIndicator.ledBlue - LED_BRIGHT_STEP : 0;
	}else if( inAplDataCar->vtc == true ){
		aplIndicator.ledBlue	= ( aplIndicator.ledBlue < LED_BRIGHT_MAX	)? aplIndicator.ledBlue + LED_BRIGHT_STEP : LED_BRIGHT_MAX;
	}
}