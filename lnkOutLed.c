
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
	DRV_LED_BAR_DATA	outDrvLedBarData;
	unsigned char		i,j;
	unsigned char		onLedNum;

	inAplDispData = getAplDispData();
	
	
	//----------------------------------
	// 7セグ表示処理
	//----------------------------------
	//1の位から1セグメント毎on,offをセット
	for( i=0 ; i<LED_7SEG_NUM ; i++ ){
		outDrvLed7SegData.data[i].data = led7SegBit[ inAplDispData->led7Seg[i] ];
	}
	//設定モード表示ON
	if( inAplDispData->settingMode == APL_DISP_SETTING_ON ){
		//1000の位に設定No表示。dot付き
		outDrvLed7SegData.data[LED_7SEG_NUM-1].data = DOT_ON( led7SegBit[ inAplDispData->settingNo ] );
	}

	//輝度
	outDrvLed7SegData.bright = inAplDispData->bright7seg;
	
	//drvへ出力
	setDrvOutShiftReg7seg( &outDrvLed7SegData );
	
	//----------------------------------
	// バーLED表示処理
	//----------------------------------
	for( i=0 ; i<LED_BAR_NUM ; i++ ){
		if( inAplDispData->barLedOnNum <= ( i*LED_BAR_SEG )){
			onLedNum	= 0;
		}else if( inAplDispData->barLedOnNum >= ( (i*LED_BAR_SEG) + LED_BAR_SEG )){
			onLedNum	= 10;
		}else{
			//該当モジュールの点灯個数
			onLedNum	= inAplDispData->barLedOnNum - ( i*LED_BAR_SEG );
		}
		//ONするbitのみ1をセットするため、初期化
		outDrvLedBarData.data[i].data = 0;
		for( j=0 ; j<onLedNum ; j++ ){
			//下位ビットから1をセットし、シフトで必要数埋める
			
			//シフトしてからセット(初週は意味なし
			outDrvLedBarData.data[i].data <<= 1;
			outDrvLedBarData.data[i].bit.bit0	= 1;

		}
	}
	outDrvLedBarData.bright = inAplDispData->brightBarled;
	setDrvOutShiftRegLedBar( &outDrvLedBarData );
}


