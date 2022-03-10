#ifndef APL_SOUND_INC_H
#define APL_SOUND_INC_H

#include "aplCtrl.h"

#define		CYC_TIME_MS		((uint8_t)10)
//********************************************************************************
// 標準動作用定数
//********************************************************************************
typedef enum{
	DISP_STATE_VALVE_CHK,
	DISP_STATE_NORMAL
}DISP_STATE;

#define		SPEED_MAX		((unsigned char)200)
#define		SPEED_MIN		((unsigned char)0)
#define		DISPCYC_7SEG_DIGIT			((unsigned short)1)	//1%辺りの周期[10ms](100%で1000ms)
#define		SPEED_DIGIT		((unsigned char)1)		//1%辺りの表示値

typedef struct{
	float	h,s,v;
}stHSV;
//********************************************************************************
// テストモード
//********************************************************************************
//テストデータ計算周期
#define		TEST_CYC_SPEED	((unsigned char)10-1)	//10x10ms=100ms毎処理
//計算毎の変化値
#define		CHG_VAL_SPEED	((unsigned char)2)
//ロータリークリック毎の変化値
#define		CHG_VAL_SPEED_MANUAL	((unsigned char)5)


typedef enum{
	TEST_STATE_UP,
	TEST_STATE_DOWN
}TEST_STATE;


//********************************************************************************
// 設定
//********************************************************************************
stHSV COLOR_TABLE[SETTING_COLOR_MAX] ={
	{   0 ,   0 , 0.33 },
	{   0 ,   1 ,   1 },
	{ 0.16,   1 , 0.5 },
	{ 0.33,   1 ,   1 },
	{ 0.50,   1 , 0.5 },
	{ 0.66,   1 ,   1 },
	{ 0.83,   1 , 0.5 }};	

#define SET_BRIGHT_7SEG_DISP	((unsigned char)180)

//********************************************************************************
// バルブチェック
//********************************************************************************
#define VALVE_CHK_RUN	false
#define VALVE_CHK_END	true

#define VALVE_CHK_OFFTIME_MS	1000
#define VALVE_CHK_STEPTIME_MS	40

#define HSV_MAX			1
#define HSV_V_MAX		0.4
#define HSV_CYC_ADD_H	0.005
#define HSV_CYC_ADD_S	0.01
#define HSV_CYC_ADD_V	0.01
#define RGB_MAX			100

typedef enum{
	HSV_RED,
	HSV_GREEN,
	HSV_BLUE,
	HSV_COLOR_MAX
}HSV_COLOR;

static stHSV RGB2HSV_TABLE[HSV_COLOR_MAX] = {
	{ 0		, 1		, 1	},
	{ 0.33	, 1		, 1 },
	{ 0.66	, 1		, 1 },
};



typedef enum{
	WATER_TEMP_STATE_LOW,
	WATER_TEMP_STATE_NORMAL,	//通常温度
}WATER_TEMP_STATE;

#define	WATER_TEMP_LOW		((uint8_t)65)
#define	WATER_TEMP_NORMAL	((uint8_t)75)

#endif
