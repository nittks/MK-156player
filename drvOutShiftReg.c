
/*
SRCLK		シフトレジスタクロック。FFへ取り込む
SRCLR		クリア。FFをリセットする
SER			データ
RCLK		出力確定。出力FFのクロック
OE			出力ポートハイインピー
*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "drvOutShiftReg_inc.h"
#include "drvOutShiftReg.h"
#include "hardware.h"

//LINKよりセットされる
static DRV_LED_BAR_DATA		drvLedBarData;
static DRV_LED_7SEG_DATA	drvLed7SegData;
static DRV_BITF_BYTE		drvLedAnode;
static DRV_BITF_SHIFT_REG	outShiftReg[LED_BAR_NUM];
static unsigned char		stateSegNo=0;
static unsigned char		preSetBright7seg;
static unsigned char		preSetBrightBarled;

unsigned long bitReverceSort( unsigned long inData , unsigned char size );

//********************************************************************************
// 初期化
//********************************************************************************
void initDrvOutShiftReg( void )
{
	unsigned char	i;
	
	stateSegNo	= 0;
	for( i=0; i<LED_7SEG_NUM ; i++ ){
		drvLed7SegData.data[i].data = 0;
	}
	for( i=0; i<LED_BAR_NUM ; i++ ){
		drvLedBarData.data[i].data = 0;
	}
	preSetBright7seg	= OCR1B_MAX;
	preSetBrightBarled	= OCR2A_MAX;

	SET_COM1B;
	SET_WGM1;
	SET_ICR1;

	SET_COM2A;
	SET_WGM2;

	SET_OCR1B( OCR1B_MAX );
	SET_OCR2A( OCR2A_MAX );

	//シフトレジスタ出力初期化
	SER_OFF;
	OE_ON;
	RCLK_OFF;
	SRCLK_OFF;
	SRCLR_OFF;
	
	START_PWM1;
	START_PWM2;
	
}

//********************************************************************************
// Lnkからセット
//********************************************************************************
void setDrvOutShiftRegLedBar( DRV_LED_BAR_DATA *inP )
{
	drvLedBarData = *inP;
}

//********************************************************************************
// Lnkからセット
//********************************************************************************
void setDrvOutShiftReg7seg( DRV_LED_7SEG_DATA *inP )
{
	drvLed7SegData = *inP;
}

//********************************************************************************
// メイン処理
//********************************************************************************
//LED出力。16ms周期(60FPS)で4セグ、1セグ4ms周期で処理
void drvOutShiftRegMain( void )
{
//	static unsigned char	tmpspeed=0;
	unsigned char	i;
	DRV_BITF_SHIFT_REG	tmp;
	DRV_BITF_BYTE	tmp7seg[LED_7SEG_NUM];	
	DRV_BITF_10BIT	tmpLedBar[LED_BAR_NUM];
	
	//7セグの明るさが変更された
	if( drvLed7SegData.bright != preSetBright7seg ){
		SET_OCR1B( drvLed7SegData.bright );
		preSetBright7seg = drvLed7SegData.bright;
	}
	//バーLEDの明るさが変更された
	if( drvLedBarData.bright != preSetBrightBarled ){
		SET_OCR2A( drvLedBarData.bright );
		preSetBrightBarled = drvLedBarData.bright;
	}

	//1セグ目(周期の頭)に1回データを取得し、出力バッファへセットする
	if( stateSegNo == 0 ){
		
		//セグメントデータ順入れ替え
		for( i=0 ; i< LED_7SEG_NUM ; i++ ){
			tmp7seg[i].data		= drvLed7SegData.data[LED_7SEG_NUM-(i+1)].data;		
			tmpLedBar[i].data	= bitReverceSort( (unsigned long)drvLedBarData.data[LED_7SEG_NUM-(i+1)].data , LED_BAR_SEG );
		}
		//7セグとバーLEDのデータをシリアルデータとして用意		
		for( i=0 ; i< LED_7SEG_NUM ; i++ ){
			outShiftReg[i].data	= (
				(((unsigned long)tmp7seg[i].data & 0x00FF) << 10) |
				(tmpLedBar[i].data & 0x3FF)
			);
		}
	}

	cli();	
	//一度全消灯
	ALL_OFF;

	//シフトレジスタ出力
	//1)初期値セット
	RCLK_OFF;
	SRCLK_OFF;
	ALL_OFF;
	SRCLR_ON;
	ALL_OFF;
	WAIT_SREG;
	SRCLR_OFF;

	tmp.data = outShiftReg[stateSegNo].data;
	for( i=0 ; i<SER_DATA_NUM ; i++ ){
		//2)データセット
		//下位から1bitずつセット。セットしたら元データ変数をシフト。
		SER_SET( ~(tmp.data & 0x01) );		//1bitずつセット
		tmp.data >>= 1;
		WAIT_SREG;

		//3)クロックposedge
		SRCLK_ON;
		WAIT_SREG;
		SRCLK_OFF;
		WAIT_SREG;
	}
	//4)2-3をループ
	//5)出力ポートへセット

	RCLK_ON;
	//アノード出力
	drvLedAnode.data	= (1 << stateSegNo);
	ANODE_SET( drvLedAnode.data );

	sei();
	
	//次のセグメントへ
	stateSegNo++;
	if( stateSegNo >= LED_BAR_NUM ){
		stateSegNo = 0;
	}

}

//********************************************************************************
// ビット並び反転
//********************************************************************************
unsigned long bitReverceSort( unsigned long inData , unsigned char size )
{
	//上位<->下位、隣り合う8bit、4bit、2bit、1bitの順に入れ替える
	inData = ((inData & 0x0000FFFF)<<16) | ((inData>>16) & 0x0000FFFF);
	inData = ((inData & 0x00FF00FF)<< 8) | ((inData>> 8) & 0x00FF00FF);
	inData = ((inData & 0x0F0F0F0F)<< 4) | ((inData>> 4) & 0x0F0F0F0F);
	inData = ((inData & 0x33333333)<< 2) | ((inData>> 2) & 0x33333333);
	inData = ((inData & 0x55555555)<< 1) | ((inData>> 1) & 0x55555555);

	//逆転して空きとなった下位bitを詰める
	inData = inData >> ((sizeof(unsigned long)*8)-size);

	return( inData );
}




