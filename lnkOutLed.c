
#include "hardware.h"
#include "lnkOutLed_inc.h"
#include "lnkOutLed.h"
#include "drvOutSerialLed.h"
#include "aplDispData.h"

static void valveChk( void );
static void normal( void );
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

	inAplDispData = getAplDispData();
	
	
	if( inAplDispData->valveChkMode == true ){
		valveChk();
	}else{
		normal();
	}
}

//********************************************************************************
// バルブチェック
//********************************************************************************
static void valveChk( void )
{
	APL_DISP_DATA		*inAplDispData;
	DRV_LED_7SEG_DATA	outDrvLed7SegData;
	uint8_t				i;

	inAplDispData = getAplDispData();

	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		if( (inAplDispData->valveChk.digitBit & (1<<i)) != 0 ){
			outDrvLed7SegData.val[i] = positionConvert( inAplDispData->valveChk.segBit );
		}else{
			outDrvLed7SegData.val[i] = 0;
		}
	}

	outDrvLed7SegData.brightRed		= inAplDispData->valveChk.red  * ((float)inAplDispData->bright7seg/100);
	outDrvLed7SegData.brightGreen	= inAplDispData->valveChk.green* ((float)inAplDispData->bright7seg/100);
	outDrvLed7SegData.brightBlue	= inAplDispData->valveChk.blue * ((float)inAplDispData->bright7seg/100);

	setDrvOutSerialLed7seg( &outDrvLed7SegData );
}

//********************************************************************************
// 通常
//********************************************************************************
static void normal( void )
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
