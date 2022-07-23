
#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdbool.h>

enum{
	UART_0_MK156,
	UART_1_DEFI,
	UART_MAX,
};
//----------------------------------------
// 送信
//----------------------------------------
#define DRV_UART_TX_BUF_SIZE	8
#define COMMAND_NUM_MAX			4
#define	COMMAND_LEN_MAX			10
typedef struct{
	uint8_t			commandNum;
	uint8_t			length[COMMAND_NUM_MAX];
	uint8_t			txCommand[COMMAND_NUM_MAX][COMMAND_LEN_MAX];
	unsigned char	txData[DRV_UART_TX_BUF_SIZE];
	unsigned char	txDataNum;
}DRV_UART_TX;

//----------------------------------------
// 受信
//----------------------------------------
#define DRV_UART_RX_BUF_SIZE	8
#define UART_RX_ID			(0x11)

typedef struct{
	unsigned char	rxData[DRV_UART_RX_BUF_SIZE];
	unsigned char	rxDataNum;
}DRV_UART_RX;



enum{	// LnkでWATER_TEMPだけ欲しい
	ID_TURBO		= 0x01,
	ID_TACHO		= 0x02,
	ID_OIL_PRESS	= 0x03,
	ID_FUEL_PRESS	= 0x04,
	ID_EXT_TEMP		= 0x05,
	ID_OIL_TEMP		= 0x07,
	ID_WATER_TEMP	= 0x0f,
	ID_MAX			= 7,
};
enum{
	UART_DATAPOS_ID,			//データ位置ID
	UART_DATAPOS_CONTROL,		//コマンド
	UART_DATAPOS_ANGLE,			//角度データ
};
#define ANGLE_DIGIT		((uint8_t)3)

#define UART_LEN_DEFI				((uint8_t)5)
#define UART_LEN_MK156(len)			((uint8_t)2+len)	//2:スタートバイト+エンドバイト
#define UART_MK156_LEN_POS			((uint8_t)2)

//----------------------------------------
// 公開関数
//----------------------------------------
extern void initDrvUart( void );
extern void drvUartMain( void );
extern void interSetUartTxData0(void);
extern void interSetUartTxData1(void);
extern void interGetUartRxData0(void);
extern void interGetUartRxData1(void);
extern void setDrvUartTx( uint8_t uartNo , DRV_UART_TX *inP );
extern bool getDrvUartRxFin( uint8_t uartNo );
extern DRV_UART_RX getDrvUartRx( uint8_t uartNo );

#endif
