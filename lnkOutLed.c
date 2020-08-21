
#include "hardware.h"
#include "lnkOutLed_inc.h"
#include "lnkOutLed.h"
#include "drvOutShiftReg.h"
#include "aplDispData.h"

//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutLed( void )
{
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkOutLedMain( void )
{
	APL_DISP_DATA		*inAplDispData;
	DRV_LED_7SEG_DATA	outDrvLed7SegData;
	unsigned char		i;

	inAplDispData = getAplDispData();
	
	
	//----------------------------------
	// 7セグ表示処理
	//----------------------------------
	//1の位から1桁毎、セグメントパターンをセット
	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		outDrvLed7SegData.val[i] = led7SegBit[ inAplDispData->led7Seg[i] ];
	}
	
	//色
	outDrvLed7SegData.brightRed		= 0;
	outDrvLed7SegData.brightGreen	= 0;
	outDrvLed7SegData.brightBlue	= 0;

	switch(inAplDispData->color7seg){
	case APL_DISP_DATA_RED:
		outDrvLed7SegData.brightRed		= inAplDispData->bright7seg;
		break;
	case APL_DISP_DATA_GREEN:
		outDrvLed7SegData.brightGreen	= inAplDispData->bright7seg;
		break;
	case APL_DISP_DATA_BLUE:
		outDrvLed7SegData.brightBlue	= inAplDispData->bright7seg;
		break;
	case APL_DISP_DATA_WHITE:
		outDrvLed7SegData.brightRed		= inAplDispData->bright7seg / 3;
		outDrvLed7SegData.brightGreen	= inAplDispData->bright7seg / 3; 
		outDrvLed7SegData.brightBlue	= inAplDispData->bright7seg / 3; 
		break;
	default:
		outDrvLed7SegData.brightGreen	= inAplDispData->bright7seg;
		break;
	}

	//drvへ出力
	setDrvOutShiftReg7seg( &outDrvLed7SegData );
	
}


