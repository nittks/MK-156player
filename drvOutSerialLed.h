#ifndef DRV_OUT_SERIAL_LED_H
#define DRV_OUT_SERIAL_LED_H

#include "hardware.h"

typedef enum{
	NO_TCB1,
	NO_TCB2
}TIMER_NO;


typedef struct{
	unsigned char	brightRed;
	unsigned char	brightGreen;
	unsigned char	brightBlue;
	unsigned char	val[LED_7SEG_DIGIT_NUM];
}DRV_LED_7SEG_DATA;

extern void initDrvOutSerialLed( void );
extern void drvOutSerialLedMain( void );
extern void setDrvOutSerialLed7seg( DRV_LED_7SEG_DATA *inP );
extern void interSetTxBuffer( void );

#endif
