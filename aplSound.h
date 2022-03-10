#ifndef APL_SOUND_H
#define APL_SOUND_H

#include <avr/io.h>
#include <stdbool.h>
#include "hardware.h"
#include "aplCtrl.h"

typedef enum{
	APL_SOUND_MODE_VALVE_CHK,
	APL_SOUND_MODE_NORMAL,
	APL_SOUND_MODE_ERR,
}APL_SOUND_MODE;

typedef enum{
	APL_SOUND_ERR_RX	= 0,
	APL_SOUND_ERR_SUM,
	APL_SOUND_ERR_MAX,
}APL_SOUND_ERR;

enum{
	APL_SOUND_RED	= SETTING_COLOR_RED,
	APL_SOUND_GREEN	= SETTING_COLOR_GREEN,
	APL_SOUND_BLUE	= SETTING_COLOR_BLUE,
	APL_SOUND_WHITE	= SETTING_COLOR_WHITE,
	APL_SOUND_MAX
};

enum{
	APL_DSP_DATA_7SEG_0,
	APL_DSP_DATA_7SEG_1,
	APL_DSP_DATA_7SEG_2,
	APL_DSP_DATA_7SEG_3,
	APL_DSP_DATA_7SEG_4,
	APL_DSP_DATA_7SEG_5,
	APL_DSP_DATA_7SEG_6,
	APL_DSP_DATA_7SEG_7,
	APL_DSP_DATA_7SEG_8,
	APL_DSP_DATA_7SEG_9,
	APL_DSP_DATA_7SEG_BLANK
};

typedef struct {
	APL_SOUND_MODE	mode;
	APL_SOUND_ERR	errNo;
	
	unsigned char	led7Seg[LED_7SEG_DIGIT_NUM];
	unsigned char	bright7seg;
	unsigned char	red;
	unsigned char	green;
	unsigned char	blue;
	float			h,s,v;
	uint8_t			digitBit;
	uint8_t			segBit;
	bool			waterOk;
	
}APL_SOUND;



extern void initAplSound( void );
extern APL_SOUND *getAplSound( void );
extern void aplSoundMain( void );


#endif
