
#include "hardware.h"
#include "lnkOutLed_inc.h"
#include "lnkOutLed.h"
#include "drvOutSerialLed.h"
#include "aplDispData.h"

static uint8_t positionConvert( uint8_t onSegBit );
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
	uint8_t				i;

	inAplDispData = getAplDispData();
	
	if( inAplDispData->valveChkMode == true ){
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			if( (inAplDispData->digitBit & (1<<i)) != 0 ){
				outDrvLed7SegData.val[i] = positionConvert( inAplDispData->segBit );
			}else{
				outDrvLed7SegData.val[i] = 0;
			}
		}
	}else{
		//1の位から1桁毎、セグメントパターンをセット
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			outDrvLed7SegData.val[i] = led7SegBit[ inAplDispData->led7Seg[i] ];
		}
	}
		
	outDrvLed7SegData.brightRed		= inAplDispData->red  * ((float)inAplDispData->bright7seg/100);
	outDrvLed7SegData.brightGreen	= inAplDispData->green* ((float)inAplDispData->bright7seg/100);
	outDrvLed7SegData.brightBlue	= inAplDispData->blue * ((float)inAplDispData->bright7seg/100);

	setDrvOutSerialLed7seg( &outDrvLed7SegData );
}


//********************************************************************************
// 7セグ点灯情報ビットの並びを、基板上の配置に並び替える
//********************************************************************************
static uint8_t positionConvert( uint8_t onSegBit )
{
	uint8_t		segBit;

	segBit = 
		((( onSegBit >> 6 ) & 0x01) << 0 ) |
		((( onSegBit >> 2 ) & 0x01) << 1 ) |
		((( onSegBit >> 1 ) & 0x01) << 2 ) |
		((( onSegBit >> 0 ) & 0x01) << 3 ) |
		((( onSegBit >> 5 ) & 0x01) << 4 ) |
		((( onSegBit >> 4 ) & 0x01) << 5 ) |		
		((( onSegBit >> 3 ) & 0x01) << 6 ) ;

	return( segBit );
}
