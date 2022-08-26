#include <avr/io.h>
#include <string.h>
#include <stdbool.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplIndicator.h"
#include "drvOutLed.h"

#include "lnkOutLed_inc.h"
#include "lnkOutLed.h"

//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutLed( void )
{
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkOutLedMain( void )
{
	APL_INDICATOR			*inAplIndicator	= getAplIndicator();
	DRV_OUT_LED				outDrvOutLed;

	outDrvOutLed.green	= inAplIndicator->ledGreen;
	outDrvOutLed.blue	= inAplIndicator->ledBlue;

	setDrvOutLed( &outDrvOutLed );
}

