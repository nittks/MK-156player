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


typedef enum{
	WATER_TEMP_STATE_LOW,
	WATER_TEMP_STATE_NORMAL,	//通常温度
}WATER_TEMP_STATE;


//********************************************************************************
// 水温
//********************************************************************************
#define	WATER_TEMP_LOW		((uint8_t)65)
#define	WATER_TEMP_NORMAL	((uint8_t)75)

//********************************************************************************
// VTEC
//********************************************************************************
#define VTEC_COOL_TIME		((uint16_t)(30000/CYC_TIME_MS))		// 音声再使用まで30sec




#endif
