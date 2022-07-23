
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

static int8_t USART_1_init();
static int8_t USART_0_init();
static void uartTx( uint8_t uartNo , USART_t* USART_REG );
static void uartRx( uint8_t uartNo , USART_t* USART_REG );
static bool isDefiNotFirstData( uint8_t uartNo , UART_RX_DATA* rx , uint8_t rxBuf );
static bool isRxComplete( uint8_t uartNo , uint8_t rxCnt , uint8_t* rxData );

static uint8_t		debugLog[1000][5]={0};
static uint16_t	debugLogCnt=0;

//********************************************************************************//
// 初期化
//********************************************************************************//
void initDrvUart( void )
{
	unsigned char	i;
	
	USART_1_init();		//defi
	USART_0_init();		//MK-156

	for( uint8_t uartNo=0 ; uartNo <= UART_MAX ; uartNo++ ){
		//送信
		for( i=0 ; i<DRV_UART_RX_BUF_SIZE; i++ ){
			uartData[uartNo].tx.drvUartTx.txData[i]	= 0;
		}
		uartData[uartNo].tx.drvUartTx.txDataNum =	0;
		uartData[uartNo].tx.byteCnt	= 0;
		uartData[uartNo].tx.cnt		= 0;
		uartData[uartNo].tx.reqFlag	= false;

		//受信
		for( i=0 ; i<DRV_UART_RX_BUF_SIZE ; i++ ){
			uartData[uartNo].rx.drvUartRx.rxData[i]	= 0;
		}
		uartData[uartNo].rx.drvUartRx.rxDataNum =	0;
		uartData[uartNo].rx.byteCnt		= 0;
		uartData[uartNo].rx.dataLen		= 0;
		uartData[uartNo].rx.flag		= false;

		uartData[uartNo].uartState		= UART_STATE_STANDBY;
	}

	//送信許可
	USART0.CTRLB |=	1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */

	//受信完了割込み許可
	USART1.CTRLA |= ( USART_RXCIE_bm );
}
//********************************************************************************//
// USART1設定。defi
//********************************************************************************//
static int8_t USART_1_init()
{
	/* 周波数誤差保証 参照元：mega3209.pdf 日本語版 P.48,158 */
	/* 工場で格納された周波数誤差でのボーレート補償 */
	/* 自動ボーレート(同期領域)なしでの非同期通信 */
	
//	volatile	int8_t sigrow_val = SIGROW.OSC20ERR5V; // 符号付き誤差取得
	volatile	int8_t sigrow_val = 0;					// 符号付き誤差取得 ,128DAのSIGROWには周波数誤差レジスタが無い
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
// USART0設定。MK-156
//********************************************************************************//
static int8_t USART_0_init()
{
	/* 周波数誤差保証 参照元：mega3209.pdf 日本語版 P.48,158 */
	/* 工場で格納された周波数誤差でのボーレート補償 */
	/* 自動ボーレート(同期領域)なしでの非同期通信 */
	
//	volatile	int8_t sigrow_val = SIGROW.OSC20ERR5V; // 符号付き誤差取得
volatile	int8_t sigrow_val = 0;					// 符号付き誤差取得 ,128DAのSIGROWには周波数誤差レジスタが無い
volatile	int32_t baud_reg_val = (uint16_t)USART_BAUD_RATE(9600);
	
	assert (baud_reg_val >= 0x4A); // 負の最大比較で正当な最小BAUDレジスタ値を確認
	baud_reg_val *= (1024 + sigrow_val); // (分解能+誤差)で乗算
	baud_reg_val /= 1024; // 分解能で除算
	USART0.BAUD = (int16_t) baud_reg_val; // 補正したボーレート設定

	USART0.CTRLA = 0
//			 | 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
//			 | 1 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
//			 | 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
//			 | USART_RS485_EXT_gc /* RS485 Mode disabled */
//			 | 1 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
//			 | 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
//			 | 1 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */
	;
	USART0.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
//	               | 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
//	               | 1 << USART_RXEN_bp     /* Receiver Enable: enabled */
	               | USART_RXMODE_NORMAL_gc /* Normal mode */
//	               | 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	               | 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */
	;
	USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
			 | USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
			 | USART_PMODE_DISABLED_gc /* no Parity */
			 | USART_SBMODE_1BIT_gc; /* 1 stop bit */

	// USART0.DBGCTRL = 0 << USART_DBGRUN_bp; /* Debug Run: disabled */

	// USART0.EVCTRL = 0 << USART_IREI_bp; /* IrDA Event Input Enable: disabled */

	// USART0.RXPLCTRL = 0x0 << USART_RXPL_gp; /* Receiver Pulse Length: 0x0 */

	// USART0.TXPLCTRL = 0x0 << USART_TXPL_gp; /* Transmit pulse length: 0x0 */

	// PORTMUX
	PORTMUX.USARTROUTEA	= (PORTMUX.USARTROUTEA & (~PORTMUX_USART0_gm)) | PORTMUX_USART0_ALT1_gc;	// RA4をUSARTとして使う
	PORTMUX.SPIROUTEA	= (PORTMUX.SPIROUTEA & (~PORTMUX_SPI0_gm) ) | PORTMUX_SPI0_NONE_gc;			// defaultのspiを無効にする
	SPI0.CTRLB			|=	1 << SPI_SSD_bp;														// エラッタ。2.10.1. SPIROUTEA値=NONE時に設定されなければならないSSDﾋﾞｯﾄ

	return 0;
}
//********************************************************************************//
// 送信セット
//********************************************************************************//
void setDrvUartTx( uint8_t uartNo , DRV_UART_TX *inP )
{
	uartData[uartNo].tx.drvUartTx	= *inP;
	uartData[uartNo].tx.reqFlag		= true;		//送信要求セット

	if( uartNo == UART_0_MK156 ){
		if( uartData[UART_0_MK156].uartState == UART_STATE_STANDBY ){
			uartData[UART_0_MK156].uartState	= UART_STATE_TRANS;
			USART0.CTRLA |= 1 << USART_DREIE_bp;			 /* Data Register Empty Interrupt Enable: disabled */
		}
	}else{
		// USART_1_DEFI は送信しない
	}
}
													
//********************************************************************************//
// 送信レジスタ空割込み
//********************************************************************************//
void interSetUartTxData0(void)
{
	cli();
	uartTx(UART_0_MK156 , &USART0);
	sei();
}
void interSetUartTxData1(void)
{
	cli();
	uartTx(UART_1_DEFI , &USART1);
	sei();
}

static void uartTx( uint8_t uartNo , USART_t* USART_REG )
{
	//長いのでポインタに入れる
	UART_TX_DATA*	tx	= &uartData[uartNo].tx;

	while( USART_REG->STATUS & USART_DREIF_bm ){	//送信レジスタ空の間回す
//		USART_REG->TXDATAH = tx->drvUartTx.txCommand[tx->byteCnt];
		USART0.TXDATAL = tx->drvUartTx.txCommand[tx->commandCnt][tx->byteCnt];
		tx->byteCnt++;

		if( tx->byteCnt >= tx->drvUartTx.length[tx->commandCnt] ){
			tx->byteCnt = 0;
			tx->commandCnt++;	// 次のコマンドへ

			if( tx->commandCnt >= tx->drvUartTx.commandNum ){
				uartData[uartNo].uartState = UART_STATE_STANDBY;
				tx->commandCnt = 0;
				USART_REG->CTRLA &= (~USART_DREIF_bm);		//送信レジスタ空割込み禁止
	//			USART_REG->CTRLA |= ( USART_TXCIE_bm);		//送信完了割込み許可
				break;
			}
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
		//受信完了割込み許可.この関数使ってないと思うのでエラーが出ないように一時的に引数決め打ち

	sei();	//割込み許可
}
//********************************************************************************//
// 受信完了確認
//********************************************************************************//
bool getDrvUartRxFin( uint8_t uartNo )
{
	return( uartData[uartNo].rx.flag );
}
//********************************************************************************//
// 受信データ取得
//********************************************************************************//
DRV_UART_RX getDrvUartRx( uint8_t uartNo )
{
	uartData[uartNo].rx.flag = false;

	return( uartData[uartNo].rx.drvUartRx );
}


//********************************************************************************//
// UART受信データ割り込み処理
//********************************************************************************//
void interGetUartRxData0(void)
{
	cli();
	uartRx(UART_0_MK156 , &USART0);
	sei();
}
void interGetUartRxData1(void)
{
	cli();
	uartRx(UART_1_DEFI , &USART1);
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
		if( ((timerCnt == TIMER_OVER_FLOW ) || (timerCnt > UART_FRAME_TIMEOUT)) ||
			( isDefiNotFirstData( uartNo , rx , rxBuf) )
		){
			*uartState = UART_STATE_STANDBY;	//待機中へリセット
			rx->byteCnt=0;
			rx->dataBufCnt=0;
		}else{
			//受信したので、タイムアウトタイマクリア
			clearTimer( TIMER_DRV_IN_UART_TIMEOUT );
		}

		rx->dataBuf[rx->byteCnt] = rxBuf;
		rx->byteCnt++;
					
		//受信完了
		if( isRxComplete(uartNo , rx->byteCnt , &rx->dataBuf[0] ) ){
			//送信要求有りの場合、送信状態へ移行
			if( tx->reqFlag == true ){	
				tx->reqFlag = false;
				*uartState = UART_STATE_STANDBY;
			}else{
				*uartState = UART_STATE_STANDBY;
			}
			//Lnk取得用配列へコピー
			memcpy( &rx->drvUartRx.rxData[0] , &rx->dataBuf[0] , rx->byteCnt);
			if( debugLogCnt < 1000 ){
				memcpy( &debugLog[debugLogCnt++][0] , &rx->dataBuf[0]  , rx->byteCnt);
			}else{
				debugLogCnt=0;
			}
			rx->drvUartRx.rxDataNum	= rx->byteCnt;
			rx->byteCnt = 0;
			rx->flag = true;
		}
	}
}

static bool isDefiNotFirstData( uint8_t uartNo , UART_RX_DATA* rx , uint8_t rxBuf )
{
	if( uartNo == UART_1_DEFI ){
		if( ((rx->byteCnt==0) && ((rxBuf==0)	 )) ||		// ReciverIDが0x00
			((rx->byteCnt==0) && ((rxBuf&0xF0)!=0)) ||		// ReceiverIDを期待したが上位0以外が来た
			((rx->byteCnt!=0) && ((rxBuf&0xF0)==0))			// ControlかAngleを期待したがIDが来た
		){
			return( true );
		}
	}
	return( false );
}


static bool isRxComplete( uint8_t uartNo , uint8_t rxCnt , uint8_t* rxData )
{
	bool	ret=false;

	switch( uartNo ){
		case UART_1_DEFI:
			if( rxCnt >= UART_LEN_DEFI){
				ret	= true;
			}
		break;
		case UART_0_MK156:
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
