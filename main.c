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
	cli();

	//I/O設定
	PORTA.DIR	-= 0x00;
	PORTC.DIR	-= 0x00;
	PORTD.DIR	-= 0x00;
	PORTE.DIR	-= 0x00;

	PORTA.PIN0CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN1CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN2CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN3CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN4CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN6CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN7CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;

	PORTC.PIN0CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTC.PIN1CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTC.PIN2CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTC.PIN3CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;

	PORTD.PIN0CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_BOTHEDGES_gc;
	PORTD.PIN1CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_BOTHEDGES_gc;
	PORTD.PIN2CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_BOTHEDGES_gc;
	PORTD.PIN3CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTD.PIN4CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTD.PIN5CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTD.PIN6CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTD.PIN7CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;

	PORTE.PIN0CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN1CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN2CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN3CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN4CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN5CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN6CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;
	PORTE.PIN7CTRL	= (1<<PORT_PULLUPEN_bp) | PORT_ISC_INPUT_DISABLE_gc;

	//クロック設定
	CLKCTRL.MCLKCTRLB	= SET_CLKCTRL_MCLKCTRLB( CLKCTRL_PDIV_64X_gc );

	//タイマ設定
	TCA0.SINGLE.CTRLESET	= SET_TCA_CTRLESET( TCA_SINGLE_DIR_UP_gc );
	TCA0.SINGLE.CTRLA		= SET_TCA_CTRLA( TCA_SINGLE_CLKSEL_DIV2_gc );
	TCA0.SINGLE.PER			= (double)0.001/((double)1/(FOSC/64/2));
	TCA0.SINGLE.CTRLB		= SET_TCA_CTRLB( TCA_SINGLE_WGMODE_NORMAL_gc );
	TCA0.SINGLE.INTCTRL		= SET_TCA_INTCTRL( OVF_EN );
	
	//割り込み許可
	sei();
}

//********************************************************************************
// 動作確認用LED点滅
//********************************************************************************
static void powerLed( void )
{
//	PORTB ^= (1<<PB1);
}
