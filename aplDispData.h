#include "hardware.h"

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

typedef enum{
	APL_DISP_SETTING_NO_BRIGHT_7SEG,
	APL_DISP_SETTING_NO_BRIGHT_BARLED,
	APL_DISP_SETTING_NO_DISP_CYC_7SEG,
	APL_DISP_SETTING_NO_DISP_CYC_BARLED,
	APL_DISP_SETTING_NO_DISP_PALSE_SPEED,
	APL_DISP_SETTING_NO_DISP_PALSE_REV
}APL_DISP_SETTING_NO;

typedef struct {
	unsigned char	led7Seg[LED_7SEG_NUM];
	unsigned char	barLedOnNum;
	unsigned char	bright7seg;
	unsigned char	brightBarled;
	APL_DISP_SETTING	settingMode;
	APL_DISP_SETTING_NO	settingNo;
}APL_DISP_DATA;


extern void initAplDispData( void );
extern APL_DISP_DATA *getAplDispData( void );
extern void aplDispDataMain( void );
