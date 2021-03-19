
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

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>


#include "drvOutSerialLed_inc.h"
#include "drvOutSerialLed.h"
#include "hardware.h"

//LINKよりセットされる
static DRV_LED_7SEG_DATA	drvLed7SegData;

static DRV_LED_7SEG_DATA	drvLed7SegDataPre;


static uint8_t	sendData[LED_7SEG_DIGIT_NUM][LED_7SEG_SEG_NUM * LED_7SEG_COLOR];
static uint8_t	digitUpdateFlag;
static uint8_t	idxTxDigit,idxTxData;

unsigned long bitReverceSort( unsigned long inData , unsigned char size );
static uint8_t isLedUpdate( void );

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

	drvLed7SegDataPre.brightRed		= 0;
	drvLed7SegDataPre.brightGreen	= 0;
	drvLed7SegDataPre.brightBlue	= 0;

	idxTxDigit	= 0;
	idxTxData	= 0;

	cli();
	// 参考
	// https://www.avrfreaks.net/forum/anyone-have-working-ws2812-library-das?

	// EVENTS
	EVSYS.CHANNEL0	= EVSYS_GENERATOR_USART0_XCK_gc;
	EVSYS.USERTCB1	= EVSYS_CHANNEL0_bm;
	EVSYS.USERTCB2	= EVSYS_CHANNEL0_bm;
	// USART0(MSPI mode)
	USART0.BAUD		= (uint16_t)(F_CPU * T_WIDTH_NS * 4 / 125 + 32) & 0xFFC0;
	USART0.CTRLC	= USART_CMODE_MSPI_gc | USART_UCPHA_bm;
	USART0.CTRLB	= USART_TXEN_bm;

	// TCB1('1' bit generation)
	TCB1.CNT	= TCB1.CCMP	= (uint16_t)(F_CPU * T1H_NS / 1000 + 0.5F);
	TCB1.EVCTRL	= TCB_CAPTEI_bm;
	TCB1.CTRLB	= TCB_CNTMODE_SINGLE_gc;
	TCB1.CTRLA	= TCB_ENABLE_bm;

	// TCB1('0' bit generation)
	TCB2.CNT	= TCB2.CCMP	= (uint16_t)(F_CPU * T1H_NS / 1000 + 0.5F);
	TCB2.EVCTRL	= TCB_CAPTEI_bm;
	TCB2.CTRLB	= TCB_CNTMODE_SINGLE_gc;
	TCB2.CTRLA	= TCB_ENABLE_bm;

	// CCL-LUT0
	CCL.TRUTH0		= 0b11011000;		//sel0=selector , if(sel0=0?sel1:sel2)  ,TRUTH:Datasheet CCl真理値表参照
	CCL.LUT0CTRLC	= CCL_INSEL2_TCB2_gc;
	CCL.LUT0CTRLB	= CCL_INSEL1_TCB1_gc | CCL_INSEL0_USART0_gc;
	CCL.LUT0CTRLA	= CCL_OUTEN_bm | CCL_ENABLE_bm;
	CCL.CTRLA		= CCL_ENABLE_bm;

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

	//LED点灯指示に変化があれば、送信データを準備する
	//ネストが少し嫌
	if( isLedUpdate() == true ){
		for( digitCnt=0 ; digitCnt<LED_7SEG_DIGIT_NUM ; digitCnt++ ){
			if( (digitUpdateFlag& (1<<digitCnt)) != 0 ){
				chkBit = (1<<(LED_7SEG_SEG_NUM-1));		//上位から
				for( segCnt=0 ; segCnt<LED_7SEG_SEG_NUM ; segCnt++ ){
					if( (drvLed7SegData.val[segCnt] & chkBit) != 0 ){
						sendData[digitCnt][segCnt]	= drvLed7SegData.brightGreen;
						sendData[digitCnt][segCnt]	= drvLed7SegData.brightRed;
						sendData[digitCnt][segCnt]	= drvLed7SegData.brightBlue;
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

	digitUpdateFlag= 0;
	//色が変更されていたら全て更新
	if(	(drvLed7SegData.brightRed	!= drvLed7SegDataPre.brightRed) |
		(drvLed7SegData.brightGreen	!= drvLed7SegDataPre.brightGreen) |
		(drvLed7SegData.brightBlue	!= drvLed7SegDataPre.brightBlue)
	){
		for( i=0 ; i<LED_7SEG_DIGIT_NUM ; i++ ){
			digitUpdateFlag|= (1<<i);
		}
		idxTxDigit	= 0;
		idxTxData	= 0;
		return true;
	}

	//表示値が変更されていたら、その桁だけ更新
	retUpdate = false;
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

	return retUpdate;
}

//********************************************************************************
// 送信データセット
//********************************************************************************
// USART0_DRE_vect(Data Register Empty)
void interSetTxBuffer(void)
{
	USART0.TXDATAL	= sendData[idxTxDigit][idxTxData];

	if( idxTxData <= (LED_7SEG_SEG_NUM * LED_7SEG_COLOR)-1 ){
		idxTxData++;
	}else{
		digitUpdateFlag &= (1<<idxTxDigit);
		if( idxTxDigit >= (LED_7SEG_DIGIT_NUM -1) ){
			USART0.CTRLA &= (~USART_DREIE_bm);

		}else{
			idxTxDigit++;
		}
	}
}


// USART0_TXC_vect
/*
interUsartDataRegisterEmpty()
{
	CCL.LUT0CTRLA	= 0;
	TCB1.CTRLA		= 0;
	TCB1.CCMP		= (unit16_t)(F_CPU * T_RESET_US) - 1;
	TCB1.CNT		= 0;
	TCB1.INTFLAGS	= TCB_CAPT_bm;
	TCB1.INTCTRL	= TCB_CAPT_bm;
	TCB1.CTRLB		= TCB_CNTMODE_INT_gc;
	TCB1.CTRLA		= TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;

}
*/

// TCB1_INT_vect
// TCB2_INT_vect
/*
 * void interStopWaveTimer( TIMER_NO timerNo )
{
	TCB_t*	tcb;
	uint_16	highTime;

	if( timerNo == NO_TCB1 ){
		tcb			= TCB1;
		highTime	= T1H_NS;
	}else{
		tcb = TCB2;
		highTime	= T2H_NS;
	}

	tcb.CTRLA		= 0;
	tcb.CNT = tcb.CCMP	= (uint16_t)(F_CPU_MHZ * highTime / 1000 + 0.5F);
	tcb.EVCTRL		= TCB_CAPTEI_bm;
	tcb.INTCTRL	= 0;
	tcb.CTRLB		= TCB_CNTMODE_SINGLE_gc;
	tcb.CTRLA		= TCB_ENABLE_bm;
	CCL.LUT0CTRLA	= CCL_OUTEN_bm | CCL_ENABLE_bm;

	buf_ptr = buf_org;
	USART0.CTRLA	= USART_DREIE_bm;
}
*/
