#include <avr/io.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplIndicator.h"

void initApl( void )
{
	initAplData();
	initAplCtrl();
	initAplIndicator();
}

void aplMain( void )
{
   	aplDataMain();
	aplCtrlMain();
	aplIndicatorMain();
}
