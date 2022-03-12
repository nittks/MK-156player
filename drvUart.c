
#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  //割り込みを使用するため
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "drvUart_inc.h"
#include "drvUart.h"
#include "hardware.h"
#include "timer.h"

static UART_DATA		uartData[UART_MAX];

static int8_t USART_0_init();
static int8_t USART_1_init();
static void uartTx( uint8_t uartNo , USART_t* USART_REG );
static void uartRx( uint8_t uartNo , USART_t* USART_REG );
static bool isRxComplete( uint8_t uartNo , uint8_t rxCnt , uint8_t* rxData );

//********************************************************************************//
// 初期化
//********************************************************************************//
void initDrvUart( void )
{
	unsigned char	i;
	
	USART_0_init();		//defi
	USART_1_init();		//MK-156

	for( uint8_t uartNo=0 ; uartNo <= UART_MAX ; uartNo++ ){
		//送信
		for( i=0 ; i<DRV_UART_RX_BUF_SIZE; i++ ){
			uartData[uartNo].tx.drvUartTx.txData[i]	= 0;
		}
		uartData[uartNo].tx.drvUartTx.txDataNum =	0;
		uartData[uartNo].tx.dataCnt	= 0;
		uartData[uartNo].tx.cnt		= 0;
		uartData[uartNo].tx.reqFlag	= false;

		//受信
		for( i=0 ; i<DRV_UART_RX_BUF_SIZE ; i++ ){
			uartData[uartNo].rx.drvUartRx.rxData[i]	= 0;
		}
		uartData[uartNo].rx.drvUartRx.rxDataNum =	0;
		uartData[uartNo].rx.dataCnt		= 0;
		uartData[uartNo].rx.dataLen		= 0;
		uartData[uartNo].rx.flag		= false;

		uartData[uartNo].uartState		= UART_STATE_STANDBY;
	}

	//送受信許可
	EN_UART_TX;


	//受信完了割込み許可
	EN_INTER_UART_RX_COMP;
}
//********************************************************************************//
// USART0設定。defi
//********************************************************************************//
static int8_t USART_0_init()
{
	/* 周波数誤差保証 参照元：mega3209.pdf 日本語版 P.48,158 */
	/* 工場で格納された周波数誤差でのボーレート補償 */
	/* 自動ボーレート(同期領域)なしでの非同期通信 */
	
	volatile	int8_t sigrow_val = SIGROW.OSC20ERR5V; // 符号付き誤差取得
	volatile	int32_t baud_reg_val = (uint16_t)USART_BAUD_RATE(19200);
	
	assert (baud_reg_val >= 0x4A); // 負の最大比較で正当な最小BAUDレジスタ値を確認
	baud_reg_val *= (1024 + sigrow_val); // (分解能+誤差)で乗算
	baud_reg_val /= 1024; // 分解能で除算
	USART1.BAUD = (int16_t) baud_reg_val; // 補正したボーレート設定

	USART1.CTRLA = 0
	//			 | 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
	//			 | 1 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
	//			 | 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
	//			 | USART_RS485_EXT_gc /* RS485 Mode disabled */
				| 1 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
	//			 | 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
	//			 | 1 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */
	;
	USART1.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
	//	               | 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
						| 1 << USART_RXEN_bp     /* Receiver Enable: enabled */
						| USART_RXMODE_NORMAL_gc /* Normal mode */
	//	               | 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	//	               | 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */
	;
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
					| USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
					| USART_PMODE_EVEN_gc /* even Parity */
					| USART_SBMODE_1BIT_gc; /* 1 stop bit */

	// USART1.DBGCTRL = 0 << USART_DBGRUN_bp; /* Debug Run: disabled */

	// USART1.EVCTRL = 0 << USART_IREI_bp; /* IrDA Event Input Enable: disabled */

	// USART1.RXPLCTRL = 0x0 << USART_RXPL_gp; /* Receiver Pulse Length: 0x0 */

	// USART1.TXPLCTRL = 0x0 << USART_TXPL_gp; /* Transmit pulse length: 0x0 */

	return 0;
}
//********************************************************************************//
// USART1設定。MK-156
//********************************************************************************//
static int8_t USART_1_init()
{
	/* 周波数誤差保証 参照元：mega3209.pdf 日本語版 P.48,158 */
	/* 工場で格納された周波数誤差でのボーレート補償 */
	/* 自動ボーレート(同期領域)なしでの非同期通信 */
	
volatile	int8_t sigrow_val = SIGROW.OSC20ERR5V; // 符号付き誤差取得
volatile	int32_t baud_reg_val = (uint16_t)USART_BAUD_RATE(9600);
	
	assert (baud_reg_val >= 0x4A); // 負の最大比較で正当な最小BAUDレジスタ値を確認
	baud_reg_val *= (1024 + sigrow_val); // (分解能+誤差)で乗算
	baud_reg_val /= 1024; // 分解能で除算
	USART1.BAUD = (int16_t) baud_reg_val; // 補正したボーレート設定

	USART1.CTRLA = 0
//			 | 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
//			 | 1 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
//			 | 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
//			 | USART_RS485_EXT_gc /* RS485 Mode disabled */
			 | 1 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
//			 | 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
//			 | 1 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */
	;
	USART1.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
//	               | 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
	               | 1 << USART_RXEN_bp     /* Receiver Enable: enabled */
	               | USART_RXMODE_NORMAL_gc /* Normal mode */
//	               | 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
//	               | 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */
	;
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
			 | USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
			 | USART_PMODE_DISABLED_gc /* no Parity */
			 | USART_SBMODE_1BIT_gc; /* 1 stop bit */

	// USART1.DBGCTRL = 0 << USART_DBGRUN_bp; /* Debug Run: disabled */

	// USART1.EVCTRL = 0 << USART_IREI_bp; /* IrDA Event Input Enable: disabled */

	// USART1.RXPLCTRL = 0x0 << USART_RXPL_gp; /* Receiver Pulse Length: 0x0 */

	// USART1.TXPLCTRL = 0x0 << USART_TXPL_gp; /* Transmit pulse length: 0x0 */

	return 0;
}
//********************************************************************************//
// 送信セット
//********************************************************************************//
void setDrvUartTx( uint8_t uartNo , DRV_UART_TX *inP )
{
	uartData[uartNo].tx.drvUartTx	= *inP;
	uartData[uartNo].tx.reqFlag		= true;		//送信要求セット
}
													
//********************************************************************************//
// 送信レジスタ空割込み
//********************************************************************************//
void interSetUartTxData0(void)
{
	cli();
	uartTx(UART_NO_0 , &USART0);
	sei();
}
void interSetUartTxData1(void)
{
	cli();
	uartTx(UART_NO_1 , &USART1);
	sei();
}

static void uartTx( uint8_t uartNo , USART_t* USART_REG )
{
	//長いのでポインタに入れる
	UART_TX_DATA*	tx	= &uartData[uartNo].tx;

	while( USART_REG->STATUS & USART_DREIF_bm ){	//送信レジスタ空の間回す
		USART_REG->TXDATAH = tx->drvUartTx.txData[tx->dataCnt];
		tx->dataCnt++;

		if( tx->dataCnt >= tx->drvUartTx.txDataNum ){	//全データ送信済み

			tx->dataCnt = 0;
			DI_INTER_UART_TX_REG_EMPTY;		//送信レジスタ空割込み禁止
			EN_INTER_UART_TX_FIN;			//送信完了割込み許可
		}
	}
	sei();	//割込み許可
}

//********************************************************************************//
// 送信完了割込み
//********************************************************************************//
void interUartTxFin( uint8_t uartNo )
{
	cli();	//割り込み禁止

	uartData[uartNo].uartState = UART_STATE_STANDBY;
	EN_INTER_UART_RX_COMP;			//受信完了割込み許可

	sei();	//割込み許可
}


//********************************************************************************//
// 受信データ取得
//********************************************************************************//
DRV_UART_RX *getDrvUartRx( uint8_t uartNo )
{
	DRV_UART_RX*	ret;
	
	if( uartData[uartNo].rx.flag == false ){
		ret = NULL;
	}else{
		ret = &uartData[uartNo].rx.drvUartRx;
		uartData[uartNo].rx.flag = false;
	}
	return( ret );
}


//********************************************************************************//
// UART受信データ割り込み処理
//********************************************************************************//
void interGetUartRxData0(void)
{
	cli();
	uartRx(UART_NO_0 , &USART0);
	sei();
}
void interGetUartRxData1(void)
{
	cli();
	uartRx(UART_NO_1 , &USART1);
	sei();
}

static void uartRx( uint8_t uartNo , USART_t* USART_REG )
{
	//長いのでポインタに入れる
	UART_RX_DATA*	rx	= &uartData[uartNo].rx;		//rxだと被って置換時大変そう。もっといい名称ほしい。intrnalVariableRx,inside,insRx?
	UART_TX_DATA*	tx	= &uartData[uartNo].tx;
	UART_STATE*		uartState	= &uartData[uartNo].uartState;

	unsigned char	rxBuf;
	unsigned char	timerCnt;
	
	while( USART_REG->RXDATAH & USART_RXCIF_bm ){

		//レジスタよりデータ取得
		rxBuf = USART_REG->RXDATAL;
		
		//タイマオーバーフロー or フレーム間タイムアウト
		//フレームの最初(ID)から受信しなおす
		timerCnt	= getTimerCnt  ( TIMER_DRV_IN_UART_TIMEOUT );
		if( (timerCnt == TIMER_OVER_FLOW ) || (timerCnt > UART_FRAME_TIMEOUT) ){
			*uartState = UART_STATE_STANDBY;	//待機中へリセット
			rx->dataCnt=0;
			rx->dataBufCnt=0;
		}else{
			//受信したので、タイムアウトタイマクリア
			clearTimer( TIMER_DRV_IN_UART_TIMEOUT );
		}

		rx->dataBuf[rx->dataCnt] = rxBuf;
		rx->dataCnt++;
					
		//受信完了
		if( isRxComplete(uartNo , rx->dataCnt , &rx->dataBuf[0] ) ){
			//送信要求有りの場合、送信状態へ移行
			if( tx->reqFlag == true ){	
				tx->reqFlag = false;
				*uartState = UART_STATE_STANDBY;
			}else{
				*uartState = UART_STATE_STANDBY;
			}
			//Lnk取得用配列へコピー
			memcpy( &rx->drvUartRx.rxData[0] , &rx->dataBuf[0] , rx->dataCnt);
			rx->drvUartRx.rxDataNum	= rx->dataCnt;
			rx->dataCnt = 0;
			rx->flag = true;
		}
	}
}

static bool isRxComplete( uint8_t uartNo , uint8_t rxCnt , uint8_t* rxData )
{
	bool	ret=false;

	switch( uartNo ){
		case UART_NO_0:
			if( rxCnt >= UART_LEN_DEFI){
				ret	= true;
			}
		break;
		case UART_NO_1:
			if( ( rxCnt >= UART_MK156_LEN_POS ) &&
				( rxCnt >= UART_LEN_MK156(rxData[UART_MK156_LEN_POS]) )
			){
				ret = true;
			}
			break;
		default:
			//取りえない
			break;
	}
	return( ret );
}



//********************************************************************************//
// UART
// ボーレート=9600bps(9.6b/ms
// 受信データ=5byte
// スタートbit=1,ストップbit=2bit,データ=8bit,パリティ=なし→1byte11bitデータ
// 5x11=55bit送信する
// 55bit/9.6bpms = 5.72ms →1フレーム辺り、最低6ms以上は必要
//********************************************************************************//
