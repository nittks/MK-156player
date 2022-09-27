#ifndef APL_INDICATOR_INC_H
#define APL_INDICATOR_INC_H

#include "aplCtrl.h"

#define		CYC_TIME_MS		((uint8_t)10)
//********************************************************************************
// 標準動作用定数
//********************************************************************************
typedef enum{
	DISP_STATE_VALVE_CHK,
	DISP_STATE_NORMAL
}DISP_STATE;


typedef enum{
	WATER_TEMP_STATE_LOW,
	WATER_TEMP_STATE_NORMAL,	//通常温度
}WATER_TEMP_STATE;


//********************************************************************************
// 水温
//********************************************************************************
#define	WATER_TEMP_LOW		((uint8_t)55)
#define	WATER_TEMP_NORMAL	((uint8_t)65)

/*

75 , 100 , 110 , 120

*/
enum{
	WATER_TMP_STATE_COOL_NORMAL,
	WATER_TMP_STATE_NORMAL_100,
	WATER_TMP_STATE_100_110,
	WATER_TMP_STATE_110_120,
	WATER_TMP_STATE_120OVER,
	WATER_TMP_STATE_MAX
};

static uint8_t	waterTmpTable[WATER_TMP_STATE_MAX] = { 75 , 100 , 110 , 120 , 0 };





//********************************************************************************
// VTEC
//********************************************************************************
#define VTEC_COOL_TIME		((uint16_t)(10000/CYC_TIME_MS))		// 音声再使用まで30sec

//********************************************************************************
// LED
//********************************************************************************
#define LED_BRIGHT_MAX	((uint8_t)100)
#define LED_BRIGHT_TIME	((uint16_t)1000)
#define LED_CHANGE_MS	((uint8_t)10)
#define LED_BRIGHT_STEP	(((double)LED_BRIGHT_MAX/LED_BRIGHT_TIME)*LED_CHANGE_MS)	// XXmsで100%分変化させる

#endif
