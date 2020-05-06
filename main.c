/*
 * periodTask.c
 *
 * Created: 2016/05/06 8:25:08
 *  Author: sin
 */ 

/*
定期タスク
MPU		:ATmega328
clock	:内蔵8MHz
*/

#define F_CPU 8000000UL
#define CNTMAX		((unsigned char)7)	//8bit
#define CNT100MSMAX	((unsigned char)200)	//5ms割り込み20回で100ms

#define FOSC	8000000				/* MCUｸﾛｯｸ周波数 */
#define BAUD	9600				/* 目的USARTﾎﾞｰﾚｰﾄ速度 */
#define MYUBRR	(FOSC/16/BAUD-1)	 /* 目的UBRR値 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "timer.h"
#include "drvInMain.h"
#include "drvOutMain.h"
#include "lnkInMain.h"
#include "lnkOutMain.h"
#include "aplMain.h"

#include "drvUart.h"
#include "main_inc.h"
#include "main.h"


#define INT_CNT_MAX	((unsigned char)125)	//8us*125=1ms毎割り込み

static void mainTask( void );
static void initReg( void );
static void USART_Init( unsigned short baud );

//********************************************************************************
// メイン
//********************************************************************************
int main(void)
{
	initMain();

	while(1)
	{
		//処理したらスリープ。タイマ割り込みで起きたら再度ループ開始
		mainTask();
		set_sleep_mode(SLEEP_MODE_IDLE);
	}
}

//********************************************************************************
// 初期化
//********************************************************************************
void initMain( void )
{
	initReg();

	initDrvIn();
	initLnkIn();
	initApl();
	initLnkOut();
	initDrvOut();
	
}
static void mainTask( void )
{
	unsigned char	i;
	
	//処理タスク実行時間チェック
	for( i=0; i<TASK_MAX ; i++){
		if( taskParameter[i].regist == true ){	//タスク有効

			//周期時間経過していたら処理
			if( taskParameter[i].currentTime <= 0 ){
				//リセット
				taskParameter[i].currentTime = taskParameter[i].cycleTime; 
				//タスク実行
				taskParameter[i].func();
			}
		}
	}
}


//********************************************************************************
// タスク時間カウント
//********************************************************************************
void interTaskTime( void )
{
	unsigned char	i;
	
	//処理タスク実行時間チェック
	for( i=0; i<TASK_MAX ; i++){
		if( taskParameter[i].regist == true ){	//登録有効タスクのみ

			//10msで1カウントダウン。
			if( taskParameter[i].currentTime > 0 ){
				taskParameter[i].currentTime--;
			}
		}
	}
}
//********************************************************************************//
// タスク有効化
//********************************************************************************//
void enableTask( unsigned char taskNo )
{
	//タスク有効化
	taskParameter[taskNo].regist	= true;
	//周期セット
	taskParameter[taskNo].currentTime	= taskParameter[taskNo].cycleTime;
}
//********************************************************************************//
// タスク無効化
//********************************************************************************//
void disableTask( unsigned char taskNo )
{
	//タスク有効化
	taskParameter[taskNo].regist	= false;
}

//********************************************************************************
// レジスタ初期化
//********************************************************************************
static void initReg(void)
{
	//I/O設定
	DDRB	= 0xCF;	
	DDRC	= 0x0F;
	DDRD	= 0xFC;

	//初期出力値設定
	PORTB	= 0xFF;
	PORTC	= 0xFF;
	PORTD	= 0x00;

//		set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	//タイマ0、CTC、割り込み用、比較A一致で割り込み
	TCCR0A	= SET_TCCR0A;
	TCCR0B	= SET_TCCR0B;
	OCR0A	= SET_OCR0A;
	TIMSK0	= SET_TIMSK0;
	OSCCAL	= SET_OSCCAL;	//クロック校正
	
	USART_Init( MYUBRR );		//UARTレジスタ初期化
	
	//割り込み許可
	sei();
}

//********************************************************************************
// UARTレジスタ初期化
//********************************************************************************
static void USART_Init( unsigned short baud )
{
	UBRR0H	= (unsigned char)(baud>>8);	//ボーレート上位
	UBRR0L	= (unsigned char) baud;		//ボーレート下位
	UCSR0C	= (1<<USBS0) | (3<<UCSZ00);	//停止bit2bit、データビット長8bit
	UCSR0B	= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);	//受信完了割込許可、受信許可、送信許可
}
//********************************************************************************
// 動作確認用LED点滅
//********************************************************************************
static void powerLed( void )
{
//	PORTB ^= (1<<PB1);
}
