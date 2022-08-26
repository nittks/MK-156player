#include <avr/io.h>
#include "drvOutLed.h"

void initDrvOut( void )
{
	initDrvOutLed();
}

void drvOutMain( void )
{
	drvOutLedMain();
}
