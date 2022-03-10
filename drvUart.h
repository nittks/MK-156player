
#ifndef DRV_UART_H
#define DRV_UART_H

//----------------------------------------
// 送信
//----------------------------------------
#define DRV_UART_TX_BUF_SIZE	8
#define COMMAND_BUF_SIZE		4
#define	COMMAND_LEN_MAX			6
typedef struct{
	uint8_t			commandNum;
	uint8_t			commandLen[COMMAND_BUF_SIZE];
	uint8_t			txCommand[COMMAND_BUF_SIZE][COMMAND_LEN_MAX];
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

//----------------------------------------
// 公開関数
//----------------------------------------
extern void initDrvUart( void );
extern void drvUartMain( void );
extern void interSetUartTxData(void);
extern void interGetUartRxData(void);
extern void drvUartChangeTx( void );
extern void setDrvUartTx( DRV_UART_TX *inP );
extern DRV_UART_RX *getDrvUartRx( void );

#endif
