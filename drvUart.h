#ifndef DRV_UART_H
#define DRV_UART_H


//----------------------------------------
// ëóêM
//----------------------------------------
#define DRV_UART_TX_BUF_SIZE	8
typedef struct{
	unsigned char	txData[DRV_UART_TX_BUF_SIZE];
	unsigned char	txDataNum;
}DRV_UART_TX;

//----------------------------------------
// éÛêM
//----------------------------------------
#define DRV_UART_RX_BUF_SIZE	8
#define UART_RX_ID			(0x11)

typedef struct{
	unsigned char	rxData[DRV_UART_RX_BUF_SIZE];
	unsigned char	rxDataNum;
}DRV_UART_RX;


//----------------------------------------
// åˆäJä÷êî
//----------------------------------------
extern void initDrvUart( void );
extern void drvUartMain( void );
extern void interSetUartTxData(void);
extern void interGetUartRxData(void);
extern void drvUartChangeTx( void );
extern void setDrvUartTx( DRV_UART_TX *inP );
extern DRV_UART_RX *getDrvUartRx( void );
#endif
