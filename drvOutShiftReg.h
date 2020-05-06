#include "hardware.h"

typedef union{
	unsigned char data;
	struct{
		unsigned char bit0 : 1;
		unsigned char bit1 : 1;
		unsigned char bit2 : 1;
		unsigned char bit3 : 1;
		unsigned char bit4 : 1;
		unsigned char bit5 : 1;
		unsigned char bit6 : 1;
		unsigned char bit7 : 1;
	}bit;
}DRV_BITF_BYTE;

typedef union{
	unsigned long data;
	struct{
		unsigned char bit00 : 1;
		unsigned char bit01 : 1;
		unsigned char bit02 : 1;
		unsigned char bit03 : 1;
		unsigned char bit04 : 1;
		unsigned char bit05 : 1;
		unsigned char bit06 : 1;
		unsigned char bit07 : 1;
		unsigned char bit08 : 1;
		unsigned char bit09 : 1;
		unsigned char bit10 : 1;
		unsigned char bit11 : 1;
		unsigned char bit12 : 1;
		unsigned char bit13 : 1;
		unsigned char bit14 : 1;
		unsigned char bit15 : 1;
		unsigned char bit16 : 1;
		unsigned char bit17 : 1;
		unsigned short blank : 14;
	}bit;
}DRV_BITF_SHIFT_REG;

typedef union{
	struct{
		unsigned char bit0 : 1;
		unsigned char bit1 : 1;
		unsigned char bit2 : 1;
		unsigned char bit3 : 1;
		unsigned char bit4 : 1;
		unsigned char bit5 : 1;
		unsigned char bit6 : 1;
		unsigned char bit7 : 1;
		unsigned char bit8 : 1;
		unsigned char bit9 : 1;
		unsigned char bitBlank : 6;
	}bit;
	unsigned short data;
}DRV_BITF_10BIT;

typedef struct{
	unsigned char	bright;
	DRV_BITF_BYTE	data[LED_7SEG_NUM];
}DRV_LED_7SEG_DATA;

typedef struct{
	unsigned char	bright;
	DRV_BITF_10BIT	data[LED_BAR_NUM];
}DRV_LED_BAR_DATA;


extern void initDrvOutShiftReg( void );
extern void drvOutShiftRegMain( void );
extern void setDrvOutShiftRegLedBar( DRV_LED_BAR_DATA *inP );
extern void setDrvOutShiftReg7seg( DRV_LED_7SEG_DATA *inP );
extern void interDrvOutLedBar( void );
