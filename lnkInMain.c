#include <avr/io.h>

#include "lnkInMain.h"
#include "lnkInCom.h"
#include "lnkInSw.h"
#include "lnkInEep.h"

void initLnkIn( void )
{
	initLnkInCom();
	initLnkInSw();
	initLnkInEep();
}

void lnkInMain( void )
{
	lnkInComMain();
	lnkInSwMain();
	lnkInEepMain();
}
