#ifndef DRV_UART_INC_H
#define DRV_UART_INC_H

#include "main.h"

#define USEBAUD	((unsigned short)9600)
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

//----------------------------------------
// 全体
//----------------------------------------
typedef enum{
	UART_STATE_STANDBY,		//受信停止(終了
	UART_STATE_TRANS,		//送信中
	UART_STATE_RECEIVE		//受信中
}UART_STATE;

#define	RS485_AUTO_XDIR OFF	(0)
#define	RS485_AUTO_XDIR_ON	(1)
#define	RS485_AUTO_TX_OUTPUT_OFF	(0)
#define	RS485_AUTO_TX_OUTPUT_ON		(1)

enum{
	RXMODE_NORMAL	= 0x0,
	RXMODE_CLK2X	= 0x1,
	RXMODE_GENAUTO	= 0x2,
	RXMODE_LINAUTO	= 0x3,
};
#define	SET_CTRLA( xdir , txd )		( (USART1.CTRLA & (~USART_RS485_gm)) | (( txd << USART_RS4851_bp ) | ( xdir << USART_RS4850_bp)))
#define	SET_CTRLB( rxmode )			( (USART1.CTRLB & USART_RXMODE_gm) | ( rxmode << USART_RXMODE_gp ))

enum{
	CMODE_ASYNCHRONOUS	= 0x0,
	CMODE_SYNCHRONOUS	= 0x1,
	CMODE_IRCOM			= 0x2,
	CMODE_MSPI			= 0x3,
};

enum{
	PMODE_DISABLED	= 0x0,
	PMODE_EVEN		= 0x2,
	PMODE_ODD		= 0x3,
};

enum{
	SBMODE_1BIT	= 0,
	SBMODE_2BIT	= 1,
};
enum{
	CHSIZE_5BIT		= 0x0,
	CHSIZE_6BIT		= 0x1,
	CHSIZE_7BIT		= 0x2,
	CHSIZE_8BIT		= 0x3,
	CHSIZE_9BITL	= 0x6,
	CHSIZE_9BITH	= 0x7,
};
#define	SET_CTRLC( cmode , pmode , sbmode , chsize )		((cmode<<USART_CMODE_gp)|(pmode<<USART_PMODE_gp)|(sbmode<<USART_SBMODE_bp)|(chsize<<USART_CHSIZE_gp))


//----------------------------------------
// 送信
//----------------------------------------
#define UART_DATA_REG_EMP_FLG			((USART1.STATUS & USART_DREIF_bm) >> USART_DREIF_bp)
#define DREIF_EMPTY		(1)
#define DREIF_NOEMPTY	(0)
#define DI_UART_TX						(USART1.CTRLB = USART1.CTRLB & (~USART_RXEN_bm))	//送信禁止
#define EN_UART_TX						(USART1.CTRLB = USART1.CTRLB | (USART_RXEN_bm))		//送信許可
#define DI_INTER_UART_TX_REG_EMPTY		(USART1.CTRLA = USART1.CTRLA & (~USART_DREIF_bm))	//送信バッファ空割込み禁止
#define EN_INTER_UART_TX_REG_EMPTY		(USART1.CTRLA = USART1.CTRLA | (USART_DREIF_bm))	//送信バッファ空割込み許可
#define DI_INTER_UART_TX_FIN			(USART1.CTRLA = USART1.CTRLA & (~USART_TXCIE_bm))	//送信完了割込み許可
#define EN_INTER_UART_TX_FIN			(USART1.CTRLA = USART1.CTRLA | (USART_TXCIE_bm))	//送信完了割込み許可
//----------------------------------------
// 受信
//----------------------------------------
#define UART_REG_RXIC					((USART1.RXDATAH & USART_RXCIF_bm) >> USART_RXCIF_bp)		//UART受信完了フラグ
#define RXC_IN_DATA						(1)		//受信データ有り
#define RXC_NO_DATA						(0)		//受信データ無し

#define RX_BUF_SIZE		0xF;	//受信バッファサイズ

#define UART_DATAPOS_ID			0		//データ位置ID
#define UART_DATAPOS_LENGTH		1		//データ位置レングス
#define UART_FRAME_TIMEOUT		2		//タイムアウト(10(フレーム周期)-7(送信時間)=2(空き時間

#define UART_ID_CARDATA			0x11		//フレームID

#define DI_UART_RX						(USART1.CTRLB = USART1.CTRLB & (~USART_RXEN_bm))	//受信禁止
#define EN_UART_RX						(USART1.CTRLB = USART1.CTRLB | (USART_RXEN_bm))	//受信許可
#define DI_INTER_UART_RX_COMP			(USART1.CTRLA = USART1.CTRLA & (~USART_RXCIE_bm))	//受信完了割込み禁止
#define EN_INTER_UART_RX_COMP			(USART1.CTRLA = USART1.CTRLA | (USART_RXCIE_bm))	//受信完了割込み許可

#endif
