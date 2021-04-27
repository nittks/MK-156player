/*
SerialLed:WS2812C-2020
通信フレーム24bit
[First] Green8bit,Red8bit,Blue8bit [Last]
上位bitから送信

Configurable Custom Logic (CCL)によるシリアル通信
参考資料 Microchip AN2387
https://ww1.microchip.com/downloads/en/AppNotes/00002387B.pdf
https://avr.jp/user/AN/PDF/AN2387.pdf
*/
#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>

#include "drvOutSerialLed_inc.h"
#include "drvOutSerialLed.h"
#include "hardware.h"

#define F_CLK_PER_MHZ		((uint8_t)(F_CPU / F_PDIV / 1000000))
#define TIMER_DIV			((uint8_t)(8))

//LINKよりセットされる
static DRV_LED_7SEG_DATA	drvLed7SegData;
static DRV_LED_7SEG_DATA	drvLed7SegDataPre;


static uint8_t	sendData[LED_7SEG_DIGIT_NUM][LED_7SEG_SEG_NUM * LED_7SEG_COLOR];
static uint8_t	digitUpdateFlag;
static uint8_t	idxTxDigit,idxTxData;
static uint8_t	cntStartupTime;

static uint16_t	debugTimeCnt;
static uint16_t	debugBaud;
#define	TIME1SEC	(10)

volatile	uint8_t		debugGreen	= 0x20;
volatile	uint8_t		debugRed	= 0x10;
volatile	uint8_t		debugBlue	= 0x05;


unsigned long bitReverceSort( unsigned long inData , unsigned char size );
static uint8_t isLedUpdate( void );
static uint8_t serchNextDigitNo( void );
//********************************************************************************
// 初期化
//********************************************************************************
void initDrvOutSerialLed( void )
{
	unsigned char	i,j;
	
	for( i=0; i<LED_7SEG_DIGIT_NUM ; i++ ){
		drvLed7SegData.val[i] = 0;
		drvLed7SegDataPre.val[i] = 0;
		for( j=0; j<LED_7SEG_SEG_NUM * LED_7SEG_COLOR ; j++ ){
			sendData[i][j]	= 0;
		}
	}
	digitUpdateFlag				= 0;

	drvLed7SegData.brightRed	= 0;
	drvLed7SegData.brightGreen	= 0;
	drvLed7SegData.brightBlue	= 0;

	drvLed7SegDataPre.brightRed		= 0xFF;		//起動後初回送信を行うため、値に差をつけておく
	drvLed7SegDataPre.brightGreen	= 0xFF;
	drvLed7SegDataPre.brightBlue	= 0xFF;

	idxTxDigit	= 0;
	idxTxData	= 0;
	cntStartupTime	= 0;

	cli();
	// 参考
	// https://www.avrfreaks.net/forum/anyone-have-working-ws2812-library-das?

	// EVENTS
	EVSYS.CHANNEL0	= EVSYS_GENERATOR_USART0_XCK_gc;
	EVSYS.USERTCB1	= EVSYS_CHANNEL0_bm;
	EVSYS.USERTCB2	= EVSYS_CHANNEL0_bm;
	// USART0(MSPI mode)
	USART0.BAUD		= (uint16_t)((uint32_t)F_CLK_PER_MHZ * T_WIDTH_NS * 4 / 125 + 32) & 0xFFC0;
//	USART0.BAUD		= 0x40;
	USART0.CTRLC	= USART_CMODE_MSPI_gc | USART_UCPHA_bm;
	USART0.CTRLB	= USART_TXEN_bm;

	// TCB1='1' , TBC2='0'
	TCB1.CNT	= TCB1.CCMP		= (uint16_t)((F_CLK_PER_MHZ  * T1H_NS ) / 1000 + 0.5F);
	TCB2.CNT	= TCB2.CCMP		= (uint16_t)((F_CLK_PER_MHZ  * T0H_NS ) / 1000 + 0.5F);
	TCB1.EVCTRL	= TCB_CAPTEI_bm;
	TCB1.CTRLB	= TCB_CNTMODE_SINGLE_gc;
	TCB1.CTRLA	= TCB_ENABLE_bm;
	
	TCB2.EVCTRL	= TCB_CAPTEI_bm;
	TCB2.CTRLB	= TCB_CNTMODE_SINGLE_gc;
	TCB2.CTRLA	= TCB_ENABLE_bm;

	// CCL-LUT0
	CCL_TRUTH0		= CCL_TRUTH2	= CCL_TRUTH3	= 0b11011000;
	CCL_LUT0CTRLC	= CCL_LUT2CTRLC	= CCL_LUT3CTRLC	= CCL_INSEL2_TCB2_gc;
	CCL_LUT0CTRLB	= CCL_LUT2CTRLB	= CCL_LUT3CTRLB	= CCL_INSEL1_TCB1_gc | CCL_INSEL0_USART0_gc;
	CCL_LUT0CTRLA	= CCL_LUT2CTRLA	= CCL_LUT3CTRLA	= CCL_ENABLE_bm;
	PORTA.OUTSET	&= (~PIN3_bm);
	PORTF.OUTSET	&= (~PIN3_bm);
	PORTD.OUTSET	&= (~PIN3_bm);
	PORTA.DIRSET	= PIN3_bm; 
	PORTF.DIRSET	= PIN3_bm;
	PORTD.DIRSET	= PIN3_bm;
	
	sei();
}

//********************************************************************************
// Lnkからセット
//********************************************************************************
void setDrvOutSerialLed7seg( DRV_LED_7SEG_DATA *inP )
{
	drvLed7SegData = *inP;
}

//********************************************************************************
// メイン処理
//********************************************************************************
void drvOutSerialLedMain( void )
{
	uint8_t		digitCnt,segCnt;
	uint8_t		chkBit;
	uint8_t		isLedUpdateChk;

	//起動後一定時間は、消灯指示を送り続ける
	if( cntStartupTime <= LED_STARTUP_TIME ){
		cntStartupTime++;
			
		digitUpdateFlag	= (uint8_t)pow(2,LED_7SEG_DIGIT_NUM) -1;
		idxTxDigit	= 0;
		idxTxData	= 0;
		USART0.CTRLA |= USART_DREIE_bm;
			
		return;
	}
	
	//LED点灯指示に変化があれば、送信データを準備する
	//ネストが少し嫌
	isLedUpdateChk = isLedUpdate();
	if( isLedUpdateChk == true ){
		for( digitCnt=0 ; digitCnt<LED_7SEG_DIGIT_NUM ; digitCnt++ ){
			if( (digitUpdateFlag & (1<<digitCnt)) != 0 ){
				chkBit = (1<<(LED_7SEG_SEG_NUM-1));		//上位から
				for( segCnt=0 ; segCnt<LED_7SEG_SEG_NUM ; segCnt++ ){
					if( (drvLed7SegData.val[digitCnt] & chkBit) == 0 ){
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+0]	= 0;
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+1]	= 0;
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+2]	= 0;
					}else{
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+0]	= drvLed7SegData.brightGreen;
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+1]	= drvLed7SegData.brightRed;
						sendData[digitCnt][segCnt*LED_7SEG_COLOR+2]	= drvLed7SegData.brightBlue;
					}
					chkBit >>= 1;
				}
			}
		}
		USART0.CTRLA |= USART_DREIE_bm;
	}
}

//********************************************************************************
// LED表示更新判定。点灯指示に変化があれば更新する
//********************************************************************************
static uint8_t isLedUpdate( void )
{
	uint8_t	i;
	uint8_t	retUpdate;

	cli();
	
	retUpdate = false;
	digitUpdateFlag= 0;
	if(	(drvLed7SegData.brightRed	!= drvLed7SegDataPre.brightRed) |
		(drvLed7SegData.brightGreen	!= drvLed7SegDataPre.brightGreen) |
		(drvLed7SegData.brightBlue	!= drvLed7SegDataPre.brightBlue)
	){
		//色が変更されていたら全て更新
		digitUpdateFlag	= (uint8_t)pow(2,LED_7SEG_DIGIT_NUM) -1;

		idxTxDigit	= 0;
		idxTxData	= 0;
		retUpdate = true;
	}else{
		//表示値が変更されていたら、その桁だけ更新
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			if( drvLed7SegData.val[i] != drvLed7SegDataPre.val[i] ){
				if( digitUpdateFlag== 0 ){
					idxTxDigit	= i;
				}
				idxTxData	= 0;
				digitUpdateFlag|= (1<<i);
				retUpdate = true;
			}
		}
	}
	drvLed7SegDataPre = drvLed7SegData;

	sei();
	return retUpdate;
}

//********************************************************************************
// 送信データセット
//********************************************************************************
// USART0_DRE_vect(Data Register Empty)
void interSetTxBuffer(void)
{
	cli();
	
	if( idxTxData == 0 ){
		CCL_CTRLA &= ~CCL_ENABLE_bm;	//CCL有効時はレジスタ変更ロックがかかるため、先に解除する
		switch( idxTxDigit ){
			case 0:	CCL.LUT0CTRLA |= (CCL_OUTEN_bm | CCL_ENABLE_bm);	break;
			case 1:	CCL.LUT3CTRLA |= (CCL_OUTEN_bm | CCL_ENABLE_bm);	break;
			case 2:	CCL.LUT2CTRLA |= (CCL_OUTEN_bm | CCL_ENABLE_bm);	break;
			default:break;
		}
		CCL_CTRLA |= CCL_ENABLE_bm;
	}
	USART0.TXDATAL	= sendData[idxTxDigit][idxTxData];
	idxTxData++;
	if( idxTxData >= (LED_7SEG_SEG_NUM * LED_7SEG_COLOR) ){
		USART0.CTRLA	&= (~USART_DREIE_bm);
		USART0.STATUS	|= USART_TXCIF_bm;
		USART0.CTRLA	|= USART_TXCIE_bm;
	}
	sei();
}

//********************************************************************************
// CCL出力ポートを次のポートへ変更する
//********************************************************************************
void interChangeNextCCLPort( void ){
	
	cli();

	//現在出力中のCCLポートを止める
	CCL_CTRLA &= ~CCL_ENABLE_bm;		//CCL有効時はレジスタ変更ロックがかかるため、先に解除する
	switch( idxTxDigit ){	//off
		case 0:	
			CCL.LUT0CTRLA &= (~CCL_ENABLE_bm);		//LUT0のレジスタにロックがかかるため、先に解除
			CCL.LUT0CTRLA &= (~CCL_OUTEN_bm);
		break;
		case 1:	
			CCL.LUT3CTRLA &= (~CCL_ENABLE_bm);
			CCL.LUT3CTRLA &= (~CCL_OUTEN_bm);
		break;
		case 2:	
			CCL.LUT2CTRLA &= (~CCL_ENABLE_bm);
			CCL.LUT2CTRLA &= (~CCL_OUTEN_bm);
		break;
		default:
			break;
	}
	CCL_CTRLA |= CCL_ENABLE_bm;


	idxTxData	= 0;			
	digitUpdateFlag &= (~(1<<idxTxDigit));
	if( digitUpdateFlag != 0 ){
		idxTxDigit	= serchNextDigitNo();
		USART0.CTRLA |= USART_DREIE_bm;
	}
	USART0.CTRLA &= (~USART_TXCIE_bm);
	
	sei();
}
//********************************************************************************
//	次に出力する桁Noを調べる
//********************************************************************************
static uint8_t serchNextDigitNo( void ){
	uint8_t	digitNo;
	
	digitNo = 0;
	while( (digitUpdateFlag & (1<<digitNo)) == 0 ){
		digitNo++;
		if( digitNo >= LED_7SEG_DIGIT_NUM ){
			// 取りえない。
		}
	}
	return digitNo;
}


