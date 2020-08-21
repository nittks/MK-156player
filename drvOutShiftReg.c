
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
static DRV_LED_7SEG_DATA	drvLed7SegData;
static unsigned char		stateSegNo=0;

unsigned long bitReverceSort( unsigned long inData , unsigned char size );

//********************************************************************************
// 初期化
//********************************************************************************
void initDrvOutShiftReg( void )
{
	unsigned char	i;
	
	stateSegNo	= 0;
	for( i=0; i<LED_7SEG_DIGIT_NUM ; i++ ){
		drvLed7SegData.val[i] = 0;
	}
	drvLed7SegData.brightRed	= 0;
	drvLed7SegData.brightGreen	= 0;
	drvLed7SegData.brightBlue	= 0;

	//シフトレジスタ出力初期化
	SER_OFF;
//	OE_ON;
	RCLK_OFF;
	SRCLK_OFF;

	SRCLR_ON;
	WAIT_SREG;
	SRCLR_OFF;	
	
//	START_PWM1;
//	START_PWM2;
	
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
void drvOutShiftRegMain( void )
{
//	static unsigned char	tmpspeed=0;
	unsigned char	i,j;
	unsigned char	sendData[CPLD_SEND_DATA_NUM];
	unsigned char	sendDataByte;

	for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
		sendData[i]	= drvLed7SegData.val[i];
	}

	sendData[i++]	=	drvLed7SegData.brightRed;
	sendData[i++]	=	drvLed7SegData.brightGreen;
	sendData[i++]	=	drvLed7SegData.brightBlue;

	cli();	
	//シフトレジスタ出力
	for( i=0 ; i<CPLD_SEND_DATA_NUM ; i++ ){
		sendDataByte	= sendData[i];
		for( j=0 ; j<sizeof(char)*8 ; j++ ){		//8bitの良い定義方法はないか？
			//2)データセット
			//上位から1bitずつセット。セットしたら元データ変数をシフト。
			SER_SET( sendDataByte >> 7 );		//1bitずつセット
			sendDataByte	<<= 1;
			WAIT_SREG;

			//3)クロックposedge
			SRCLK_ON;
			WAIT_SREG;
			SRCLK_OFF;
			WAIT_SREG;
		}
	}
	//4)2-3をループ
	//5)出力ポートへセット
	RCLK_ON;
	WAIT_SREG;
	RCLK_OFF;

	sei();

}

