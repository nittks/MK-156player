#include <avr/io.h>

#include "drvInMain_inc.h"
#include "drvInMain.h"

#include "drvInSw.h"
#include "drvUart.h"
#include "drvEep.h"

void initDrvIn( void )
{
	initDrvInSw();
	initDrvUart();
	initDrvEep();
}

void drvInMain( void )
{
	drvInSwMain();
}
