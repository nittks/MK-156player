#include <avr/io.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplDispData.h"

void initApl( void )
{
	initAplData();
	initAplCtrl();
	initAplDispData();
}

void aplMain( void )
{
	aplDataMain();
	aplCtrlMain();
	aplDispDataMain();
}
