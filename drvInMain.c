#include <avr/io.h>

#include "drvInSw.h"
#include "drvUart.h"
#include "drvEep.h"

#include "drvInMain_inc.h"
#include "drvInMain.h"

void initDrvIn( void )
{
	initDrvInSw();
	initDrvUart();
	initDrvEep();
}

void drvInMain( void )
{
	drvInSwMain();
	drvInCarSwMain();
}
