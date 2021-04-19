#include <avr/io.h>

#include "lnkOutMain.h"
#include "lnkOutLed.h"
#include "lnkOutEep.h"
#include "lnkOutCom.h"

void initLnkOut( void )
{
	initLnkOutLed();
	initLnkOutEep();
	initLnkOutCom();
}

void lnkOutMain( void )
{
	lnkOutLedMain();
	lnkOutEepMain();
	lnkOutComMain();
}
