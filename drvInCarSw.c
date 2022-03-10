
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include "drvInCarSw_inc.h"
#include "drvInCarSw.h"

//Lnkが取得する
static DRV_IN_CAR_SW	drvInCarSw;

//********************************************************************************
// 初期化
//********************************************************************************
void initDrvInCarSw( void )
{
	drvInCarSw.vtc	= DRV_IN_CAR_SW_OFF;
}

//********************************************************************************
// LINKが取得
//********************************************************************************
DRV_IN_CAR_SW *getDrvInCarSw( void )
{
	return( &drvInCarSw );
}

//********************************************************************************
// main
//********************************************************************************
void drvInCarSwMain( void )
{
	static	uint8_t		debounceTimeCnt = 0;
	
	if( PORT_VTC == ON ){
		drvInCarSw.vtc	= DRV_IN_CAR_SW_ON;
	}else{
		if( debounceTimeCnt >= DEBOUNCE_TIME ){
			drvInCarSw.vtc	= DRV_IN_CAR_SW_OFF;
			debounceTimeCnt	= 0;
		}else{
			debounceTimeCnt++;
		}
	}
}
