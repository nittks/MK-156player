#ifndef APL_CTRL_INC_H
#define APL_CTRL_INC_H

#include <stdio.h>
#include "aplCtrl.h"
#include "aplCtrlSetting.h"

#define		SETTING_VAL_MAX			((unsigned char)100)
#define		SETTING_VAL_MIN			((unsigned char)0)
#define		SETTING_VAL_DEF			((unsigned char)50)		//デフォルト値(EEPROM読み込みエラー時
#define		SETTING_VAL_INTERVAL	((unsigned char)5)	//変化間隔(%)


enum{
	COLOR_7SEG_WHITE,
	COLOR_7SEG_RED,
	COLOR_7SEG_YELLOW,
	COLOR_7SEG_GREEN,
	COLOR_7SEG_AQUA,
	COLOR_7SEG_BLUE,
	COLOR_7SEG_PURPLE,
	COLOR_7SEG_USER,
	COLOR_7SEG_MAX
};

enum{
	EEP_COLOR_7SEG,
	EEP_COLOR_RED,
	EEP_COLOR_GREEN,
	EEP_COLOR_BLUE,
	EEP_BRIGHT_7SEG,
	EEP_BRIGHT_DIM_7SEG,
	EEP_DISPCYC_7SEG,
	EEP_SETTING_ITEM_MAX
};
unsigned char eepDefault[EEP_SETTING_ITEM_MAX] = {
	COLOR_7SEG_WHITE,
	50,		//red
	50,		//green
	50,		//blue
	80,		//7セグ輝度
	20,		//7セグ輝度(減光
	20,		//7セグ表示更新速度
};
#define		INIT_PALSE_SPEED	SETTING_PALSE_SPEED_04
#define		INIT_PALSE_REV		SETTING_PALSE_REV_04

static APL_CTRL_SET			aplCtrlSet;
static APL_CTRL_SET			aplCtrlSetBak;		//設定保存せず抜ける祭に戻す、元の値保持用
static APL_CTRL_SET_PALSE	aplCtrlSetPalse;
static APL_CTRL_SET_PALSE	aplCtrlSetPalseBak;

#define	HIERARCHY_NUM		((uint8_t)4)
#define	ITEM_NUM_RGB		((uint8_t)3)
#define VOLUME_MAX			((uint8_t)100)
static SETTING_CONTENTS		settingContents3[] = {
	{ APL_CTRL_STATE_SETTING_COLOR_R	,VOLUME_MAX	, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.colorRGB.red		, &aplCtrlSetBak.colorRGB.red	},
	{ APL_CTRL_STATE_SETTING_COLOR_G	,VOLUME_MAX	, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.colorRGB.green	, &aplCtrlSetBak.colorRGB.green	},
	{ APL_CTRL_STATE_SETTING_COLOR_B	,VOLUME_MAX	, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.colorRGB.blue		, &aplCtrlSetBak.colorRGB.blue	}
};

static SETTING_CONTENTS		settingContents2 [] = {
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_NON		,	0	, TYPE_NON	, 0	, NULL	, NULL	, NULL	, NULL	},
	{ APL_CTRL_STATE_SETTING_COLOR_RGB	,	(sizeof(settingContents3)/sizeof(SETTING_CONTENTS)-1)	
												, TYPE_ITEM	, 1	, NULL	, &settingContents3[0]	
																				, NULL	, NULL	}
};

uint8_t SETTING_PALSE_SPEED[]	= {4,8,16,20,25};			//車速パルス仕様
uint8_t SETTING_PALSE_REV[]		= {1,2,3,4,5,6,8,10,12};	//回転数パルス仕様(気筒数
static SETTING_CONTENTS		settingContents1 [] = {
	{ APL_CTRL_STATE_SETTING_BRIGHT			,VOLUME_MAX			, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.bright7seg	, &aplCtrlSetBak.bright7seg		},
	{ APL_CTRL_STATE_SETTING_BRIGHT_DIM		,VOLUME_MAX			, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.brightDim7seg , &aplCtrlSetBak.brightDim7seg	},
	{ APL_CTRL_STATE_SETTING_COLOR			,(sizeof(settingContents2)/sizeof(SETTING_CONTENTS)-1)
																, TYPE_ITEM	, 1	, NULL	, &settingContents2[0]
																								, &aplCtrlSet.colorNo		, &aplCtrlSetBak.colorNo		},
	{ APL_CTRL_STATE_SETTING_DSPCYC			,VOLUME_MAX			, TYPE_VAL	, 5	, NULL	, NULL	, &aplCtrlSet.dispcyc7seg	, &aplCtrlSetBak.dispcyc7seg	},
	{ APL_CTRL_STATE_SETTING_PALSE_SPEED	,(sizeof(SETTING_PALSE_SPEED)/sizeof(uint8_t)-1)
																, TYPE_ITEM	, 1	, &SETTING_PALSE_SPEED[0]
																						, NULL	, &aplCtrlSetPalse.speed	, &aplCtrlSetPalseBak.speed		},
	{ APL_CTRL_STATE_SETTING_PALSE_REV		,(sizeof(SETTING_PALSE_REV)/sizeof(uint8_t)-1)
																, TYPE_ITEM	, 1	, &SETTING_PALSE_REV[0]
																						, NULL	, &aplCtrlSetPalse.rev		, &aplCtrlSetPalseBak.rev		},
};

static SETTING_CONTENTS		settingContents0[1] ={
	{ APL_CTRL_STATE_SETTING_ROOT	, (sizeof(settingContents1)/sizeof(SETTING_CONTENTS)-1)
																, TYPE_ITEM	, 1	, NULL	, &settingContents1[0]	, NULL	, NULL	}
};
#endif