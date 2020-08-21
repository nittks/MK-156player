#ifndef DRV_UART_INC_H
#define DRV_UART_INC_H

//----------------------------------------
// 全体
//----------------------------------------
typedef enum{
	UART_STATE_STANDBY,		//受信停止(終了
	UART_STATE_TRANS,		//送信中
	UART_STATE_RECEIVE		//受信中
}UART_STATE;

#define RS485PORT	PORTD
#define RS485BIT	PD2
#define RS485_RX	(RS485PORT &= ~(1<<RS485BIT))
#define RS485_TX	(RS485PORT |=  (1<<RS485BIT))

//----------------------------------------
// 送信
//----------------------------------------
#define UART_REG_UDRE	(UCSR0A & (1<<UDRIE0))
#define UDRE_EMPTY		(1<<UDRIE0)
#define UDRE_NOEMPTY	(0<<UDRIE0)
#define DI_UART_TX						(UCSR0B = UCSR0B & (~(1<<TXEN0)))	//送信禁止
#define EN_UART_TX						(UCSR0B = UCSR0B | ( (1<<TXEN0)))	//送信許可
#define DI_INTER_UART_TX_REG_EMPTY		(UCSR0B = UCSR0B & (~(1<<UDRIE0)))	//送信バッファ空割込み禁止
#define EN_INTER_UART_TX_REG_EMPTY		(UCSR0B = UCSR0B | ( (1<<UDRIE0)))	//送信バッファ空割込み許可
#define DI_INTER_UART_TX_FIN			(UCSR0B &= (~(1<<TXCIE0)))		//送信完了割込み許可
#define EN_INTER_UART_TX_FIN			(UCSR0B |= ( (1<<TXCIE0)))		//送信完了割込み許可
//----------------------------------------
// 受信
//----------------------------------------
#define UART_REG_RXC	(UCSR0A & (1<<RXC0))	//UART受信完了フラグ
#define RXC_IN_DATA		(1<<RXC0)		//受信データ有り
#define RXC_NO_DATA		(0<<RXC0)		//受信データ無し

#define RX_BUF_SIZE		0xF;	//受信バッファサイズ

#define UART_DATAPOS_ID			0		//データ位置ID
#define UART_DATAPOS_LENGTH		1		//データ位置レングス
#define UART_FRAME_TIMEOUT		2		//タイムアウト(10(フレーム周期)-7(送信時間)=2(空き時間

#define UART_ID_CARDATA			0x11		//フレームID

#define DI_UART_RX						(UCSR0B = UCSR0B & (~(1<<RXEN0)))	//受信禁止
#define EN_UART_RX						(UCSR0B = UCSR0B | ( (1<<RXEN0)))	//受信許可
#define DI_INTER_UART_RX_COMP			(UCSR0B = UCSR0B & (~(1<<RXCIE0)))	//受信完了割込み禁止
#define EN_INTER_UART_RX_COMP			(UCSR0B = UCSR0B | ( (1<<RXCIE0)))	//受信完了割込み許可

#endif
