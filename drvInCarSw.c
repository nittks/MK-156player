
#include <avr/io.h>
#include <avr/interrupt.h>

#include "drvInCarSw_inc.h"
#include "drvInCarSw.h"

//Lnkが取得する
static DRV_IN_CAR_SW	drvInCarSw;
static DEBOUNCE_TIME_CNT	debounceTimeCnt;	

//********************************************************************************
// 初期化
//********************************************************************************
void initDrvInCarSw( void )
{
	drvInCarSw.vtc	= DRV_IN_CAR_SW_OFF;
	debounceTimeCnt.on	= 0;
	debounceTimeCnt.off	= 0;
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
	if( PORT_VTC == ON ){
		debounceTimeCnt.off	= 0;
		if( debounceTimeCnt.on >= DEBOUNCE_TIME ){
			drvInCarSw.vtc	= DRV_IN_CAR_SW_ON;
		}else{
			debounceTimeCnt.on++;
		}
	}else{
		debounceTimeCnt.on	= 0;
		if( debounceTimeCnt.off >= DEBOUNCE_TIME ){
			drvInCarSw.vtc	= DRV_IN_CAR_SW_OFF;
		}else{
			debounceTimeCnt.off++;
		}
	}
}
