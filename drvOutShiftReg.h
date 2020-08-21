#ifndef DRV_OUT_SHIFT_REG_H
#define DRV_OUT_SHIFT_REG_H

#include "hardware.h"

typedef struct{
	unsigned char	brightRed;
	unsigned char	brightGreen;
	unsigned char	brightBlue;
	unsigned char	val[LED_7SEG_DIGIT_NUM];
}DRV_LED_7SEG_DATA;

extern void initDrvOutShiftReg( void );
extern void drvOutShiftRegMain( void );
extern void setDrvOutShiftReg7seg( DRV_LED_7SEG_DATA *inP );
extern void interDrvOutLedBar( void );

#endif
