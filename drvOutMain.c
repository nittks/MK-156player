#include <avr/io.h>

#include "drvOutSerialLed.h"

void initDrvOut( void )
{
	initDrvOutSerialLed();
}

void drvOutMain( void )
{
	drvOutSerialLedMain();
}
