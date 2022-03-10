#include <avr/io.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplSound.h"

void initApl( void )
{
	initAplData();
	initAplCtrl();
	initAplSound();
}

void aplMain( void )
{
   	aplDataMain();
	aplCtrlMain();
	aplSoundMain();
}
