#ifndef DRV_OUT_LED_H
#define DRV_OUT_LED_H

typedef struct{
	uint8_t		green;		// 22pin PF2	TCD WOC
	uint8_t		blue;		// 23pin PF3	TCD WOD
}DRV_OUT_LED;



extern void initDrvOutLed( void );
extern void drvOutLedMain( void );
extern void setDrvOutLed( DRV_OUT_LED *inP );


#endif