
#include "hardware.h"
#include "lnkOutLed_inc.h"
#include "lnkOutLed.h"
#include "drvOutSerialLed.h"
#include "aplDispData.h"

static uint8_t positionConvert( uint8_t onSegBit );
static void convertHSVtoRGB( float  h,float	s,float  v, float* r,float*	g,float* b );
static void setRgb( float* r,float* g,float* b,float val_0,float val_1,float val_2);
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
	float				red,green,blue;

	inAplDispData = getAplDispData();
	
	switch(inAplDispData->mode){
	case APL_DISP_DATA_MODE_VALVE_CHK:
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			if( (inAplDispData->digitBit & (1<<i)) != 0 ){
				outDrvLed7SegData.val[i] = positionConvert( inAplDispData->segBit );
			}else{
				outDrvLed7SegData.val[i] = 0;
			}
		}
		break;
	case APL_DISP_DATA_MODE_ERR:
		if( inAplDispData->errNo == APL_DISP_DATA_ERR_RX ){
			//セット記述を関数にしようと思ったけど、とりあえずこれで
			outDrvLed7SegData.val[0]	= positionConvert( errSeg[APL_DISP_DATA_ERR_RX][2] );
			outDrvLed7SegData.val[1]	= positionConvert( errSeg[APL_DISP_DATA_ERR_RX][1] );
			outDrvLed7SegData.val[2]	= positionConvert( errSeg[APL_DISP_DATA_ERR_RX][0] );
		}else if( inAplDispData->errNo == APL_DISP_DATA_ERR_SUM ){
			outDrvLed7SegData.val[0]	= positionConvert( errSeg[APL_DISP_DATA_ERR_SUM][2] );
			outDrvLed7SegData.val[1]	= positionConvert( errSeg[APL_DISP_DATA_ERR_SUM][1] );
			outDrvLed7SegData.val[2]	= positionConvert( errSeg[APL_DISP_DATA_ERR_SUM][0] );
		}
		break;	
	case APL_DISP_DATA_MODE_NORMAL:
		//1の位から1桁毎、セグメントパターンをセット
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			outDrvLed7SegData.val[i] = led7SegBit[ inAplDispData->led7Seg[i] ];
		}
		break;
	}
	
	convertHSVtoRGB(	inAplDispData->h ,inAplDispData->s ,inAplDispData->v * ((float)inAplDispData->bright7seg / BRIGHT_MAX),
						&red ,&green ,&blue	);

	outDrvLed7SegData.brightRed		= red  * RGB_MAX;
	outDrvLed7SegData.brightGreen	= green* RGB_MAX;
	outDrvLed7SegData.brightBlue	= blue * RGB_MAX;

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

//********************************************************************************
// HSV -> RGB変換
// 参考：検索ヒットした複数サイト。最終的にはwikiベース
// https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93
//********************************************************************************
static void convertHSVtoRGB(
	float  h,float	s,float  v,
	float* r,float*	g,float* b
){
	float	f,calcA,calcB,calcC,calcD;
	int		i;
	
	if( s <= 0 ){
		*r = *g = *b = v;
	}else{
		h	*= 6;
		i	= (int)h;
		f	= h - (float)i;
		
		calcA	= v;
		calcB	= v * (1-s);
		calcC	= v * (1-s*f);
		calcD	= v * (1-s*(1-f));	
		
		switch(i){
			case 0:	setRgb( r,g,b,  calcA,calcD,calcB );	break;
			case 1:	setRgb( r,g,b,  calcC,calcA,calcB );	break;
			case 2:	setRgb( r,g,b,  calcB,calcA,calcD );	break;
			case 3:	setRgb( r,g,b,  calcB,calcC,calcA );	break;
			case 4:	setRgb( r,g,b,  calcD,calcB,calcA );	break;
			case 5:	setRgb( r,g,b,  calcA,calcB,calcC );	break;
			default:break;
		}
	}
}
static void setRgb(
	float*	r,
	float*	g,
	float*	b,
	float	val_0,
	float	val_1,
	float	val_2
){
	*r	= val_0;
	*g	= val_1;
	*b	= val_2;
}
