#include <avr/io.h>

#include "lnkOutMain.h"
#include "lnkOutEep.h"
#include "lnkOutCom.h"

void initLnkOut( void )
{
	initLnkOutEep();
	initLnkOutCom();
}

void lnkOutMain( void )
{
	lnkOutEepMain();
	lnkOutComMain();
}
