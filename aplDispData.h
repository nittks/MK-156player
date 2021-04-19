#ifndef APL_DISP_DATA_H
#define APL_DISP_DATA_H

#include "hardware.h"
#include "aplCtrl.h"


enum{
	APL_DISP_DATA_RED	= SETTING_COLOR_RED,
	APL_DISP_DATA_GREEN	= SETTING_COLOR_GREEN,
	APL_DISP_DATA_BLUE	= SETTING_COLOR_BLUE,
	APL_DISP_DATA_WHITE	= SETTING_COLOR_WHITE,
	APL_DISP_DATA_MAX
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

typedef enum{
	APL_DISP_SETTING_OFF,
	APL_DISP_SETTING_ON
}APL_DISP_SETTING;

typedef struct {
	unsigned char	led7Seg[LED_7SEG_DIGIT_NUM];
	unsigned char	bright7seg;
	unsigned char	color7seg;
	APL_DISP_SETTING	settingMode;
}APL_DISP_DATA;


extern void initAplDispData( void );
extern APL_DISP_DATA *getAplDispData( void );
extern void aplDispDataMain( void );


#endif