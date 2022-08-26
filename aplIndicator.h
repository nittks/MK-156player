#ifndef APL_INDICATOR_H
#define APL_INDICATOR_H

#include <avr/io.h>
#include <stdbool.h>
#include "hardware.h"
#include "aplCtrl.h"

typedef struct {
	bool			waterOk;
	bool			vtec;
	uint8_t			ledGreen;
	uint8_t			ledBlue;
	uint8_t			cycCntGreen;
	uint8_t			cycCntBlue;
}APL_INDICATOR;



extern void initAplIndicator( void );
extern APL_INDICATOR *getAplIndicator( void );
extern void aplIndicatorMain( void );


#endif
