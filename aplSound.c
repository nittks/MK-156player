//#define DEBUG
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

static bool		waterTempState;
static bool		vtecOnState;
static bool		preVtc;
static uint16_t	vtecCoolTimeCnt;

static void porcWaterTemp( void );
static void procVtec( void );

//********************************************************************************
// 初期化
//********************************************************************************
void initAplSound( void )
{
	aplSound.waterOk		= false;

	waterTempState	= WATER_TEMP_STATE_LOW;
	vtecOnState		= false;
	preVtc			= false;
	vtecCoolTimeCnt		= VTEC_COOL_TIME;
	
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
	PORTF.OUTTGL	= 0x10;

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
			aplSound.waterOk	= true;
		}
	}else{
		aplSound.waterOk	= false;	//1発出しなので、遷移したらOFF
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

	aplSound.vtec	= false;

	if( vtecCoolTimeCnt >= VTEC_COOL_TIME ){
		if( (inAplDataCar->vtc == true) && (preVtc == false) ){		// ONエッジ
			aplSound.vtec	= true;
			vtecCoolTimeCnt	= 0;
		}
	}else{
		aplSound.vtec	= false;
		vtecCoolTimeCnt++;
	}
	preVtc	= inAplDataCar->vtc;
}