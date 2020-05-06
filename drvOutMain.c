#include <avr/io.h>

#include "drvOutShiftReg.h"

void initDrvOut( void )
{
	initDrvOutShiftReg();
}

void drvOutMain( void )
{
	drvOutShiftRegMain();
}
