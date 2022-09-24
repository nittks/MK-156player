#include <avr/io.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "aplData.h"
#include "drvUart.h"
#include "drvInCarSw.h"
#include "lnkInCom_inc.h"
#include "lnkInCom.h"

static uint8_t		debugLog[1000][5]={0};
static uint16_t		debugLogCnt=0;
static uint8_t		posRead[UART_MAX];

static void procRxMk156( void );
static bool mk156Pick1Frame( uint8_t frame[]);
static void procCarComSw( void );
static void procCarSw( APL_DATA_CAR* aplDataCar );
static bool pickRxFrame( uint8_t* rxFrameData );
static uint16_t rxDataNum( uint8_t uartNo , DRV_UART_RX* inDrvUartRx );
static uint16_t asciiToAngle( uint8_t* angleStr );
static uint8_t asciiHexToDec( uint8_t asciiHex );
static uint16_t angleToSensor( uint16_t angle , uint8_t id );

//**********************************************************************
//初期化
//**********************************************************************
void initLnkInCom( void )
{
	for( uint8_t i = 0 ; i<UART_MAX ; i++ ){
		posRead[i]	= 0;
	}
}
//**********************************************************************
//メイン処理
//**********************************************************************
void lnkInComMain( void )
{
	procRxMk156();

	procCarComSw();
}
//**********************************************************************
// MK-156からのデータ処理
//**********************************************************************
static void procRxMk156( void )
{
	volatile MK156_FRAME	mk156Frame;
	uint8_t	frame[MK156_MAX];

	if( !(mk156Pick1Frame( &frame[0] )) ){
		volatile uint8_t a;
		 return;
	}

	mk156Frame.len			= frame[MK156_FORMAT02_LENGTH];
	mk156Frame.cmd			= frame[MK156_FORMAT03_COMMAND];
	mk156Frame.feedback[0]	= frame[MK156_FORMAT04_FEEDBACK+0];
	mk156Frame.feedback[1]	= frame[MK156_FORMAT04_FEEDBACK+1];
	mk156Frame.feedback[2]	= frame[MK156_FORMAT04_FEEDBACK+2];
	mk156Frame.sum[0]		= frame[MK156_FORMAT07_SUM+0];
	mk156Frame.sum[1]		= frame[MK156_FORMAT07_SUM+1];

	if( mk156Frame.cmd == MK156_CMD_SD_PLAY_FIN ){
		setReturnData( mk156Frame.cmd );
	}else if( *(uint32_t*)(&mk156Frame.cmd) == 0x03000040 ){	// 下位から上位？
		setReturnData( 0x40 );
	}
}
//**********************************************************************
//**********************************************************************
static bool mk156Pick1Frame( uint8_t frame[])
{
	bool		result	= false;
	DRV_UART_RX inDrvUartRx;
	inDrvUartRx = getDrvUartRx(UART_0_MK156);

	if( rxDataNum(UART_0_MK156,&inDrvUartRx) < MK156_MAX ){		// 受信データが1フレーム分ない
		return( false );
	}
	uint8_t	byteCnt	=0;

	uint8_t	pRead	= posRead[UART_0_MK156];
	while( byteCnt < MK156_MAX ){
		// リングバッファデータ全て読んでしまったら終了
		if( inDrvUartRx.posWrite == pRead ){
			result	= false;
			break;
		}
		uint8_t rxData = inDrvUartRx.rxData[pRead++];

		if( byteCnt == 0 ){
			if( rxData != MK156_START_BYTE ){		// スタートバイトが来るまで回す
				continue;
			}else{
				if( rxDataNum(UART_0_MK156,&inDrvUartRx) < MK156_MAX ){
					// スタートバイトの時点で全データ分の長さがない場合は終了
					result = false;
					break;
				}else{
					frame[byteCnt++]	= rxData;
				}
			}
		}else{
			frame[byteCnt++]	= rxData;
			if( rxData == MK156_END_BYTE ){
				result = true;
				break;
			}
		}

		if( pRead >= DRV_UART_RX_RING_BUF_SIZE ){
			pRead = 0;
		}
		posRead[UART_0_MK156]	= pRead;
	}

	return( result );
}
//**********************************************************************
//**********************************************************************
static void procCarComSw( void )
{
	APL_DATA_CAR*	inAplDataCar	= getAplDataCar();	// 現在値を取得し、更新された値だけ書き換え、再セットする
	APL_DATA_CAR	outAplDataCar	= *inAplDataCar;
	
	//車両スイッチ、信号処理
	procCarSw( &outAplDataCar );

	uint8_t	rxFrameData[DEFI_MAX];
/*
	memcpy( debugLog[debugLogCnt] , inDrvUartRx.rxData , 5 );
	debugLogCnt = ( debugLogCnt >= 900 )? 0 : debugLogCnt+1;
*/
	outAplDataCar.rx	= false;		// 多分今は使ってない。見直したい

	while( pickRxFrame( &rxFrameData[0] )){
		switch( rxFrameData[UART_DATAPOS_ID] ){
			case ID_WATER_TEMP:
				if( rxFrameData[UART_DATAPOS_CONTROL] == CONTROL_NORMAL_OPERATION ){
					volatile uint16_t angle = asciiToAngle( &rxFrameData[UART_DATAPOS_ANGLE] );
					outAplDataCar.waterTemp	= angleToSensor( angle , ID_WATER_TEMP );
				}
				break;
			default:
				break;
		}
		outAplDataCar.rx	= true;
	}	
	setAplDataCar( &outAplDataCar );
}

//**********************************************************************
//車両スイッチ、信号処理
//**********************************************************************
static void procCarSw( APL_DATA_CAR* outAplDataCar )
{
	DRV_IN_CAR_SW	*inDrvInCarSw;
	inDrvInCarSw	= getDrvInCarSw();
	outAplDataCar->vtc	= (inDrvInCarSw->vtc == DRV_IN_CAR_SW_ON)? true : false;
}
//**********************************************************************
//**********************************************************************
static bool pickRxFrame( uint8_t* rxFrameData )
{
	DRV_UART_RX inDrvUartRx;
	inDrvUartRx = getDrvUartRx(UART_1_DEFI);

	if( rxDataNum(UART_1_DEFI,&inDrvUartRx) < DEFI_MAX ){		// 受信データが1フレーム分ない
		return( false );
	}

	bool	result	= true;
	uint8_t	byteCnt=0;
	while( byteCnt < DEFI_MAX ){
		uint8_t rxData = inDrvUartRx.rxData[posRead[UART_1_DEFI]++];
		if( posRead[UART_1_DEFI] >= DRV_UART_RX_RING_BUF_SIZE ){
			posRead[UART_1_DEFI] = 0;
		}else if( posRead[UART_1_DEFI] >= inDrvUartRx.posWrite ){
			result	= false;
			break;
		}

		if( byteCnt == 0 ){
			if( ((rxData & 0xF0) == 0x00) &&	// IDのとき
				((rxData & 0x0F) != 0x00)
			){
				rxFrameData[byteCnt]	= rxData;
			}else{
				continue;
			}
		}else{
			rxFrameData[byteCnt]	= rxData;	// ID以外のとき
		}

		byteCnt++;
	}
	return( result );
}
//**********************************************************************
//**********************************************************************
static uint16_t rxDataNum( uint8_t uartNo , DRV_UART_RX* inDrvUartRx )	// データの値渡しが重そうなので、ポインタで引き継ぐ
{
	uint8_t		ret;

	if( posRead[uartNo] <= inDrvUartRx->posWrite ){
		ret	= inDrvUartRx->posWrite - posRead[uartNo];
	}else{
		ret	= (DRV_UART_RX_RING_BUF_SIZE - posRead[uartNo]) + inDrvUartRx->posWrite;
	}

	return( ret );
}

//**********************************************************************
//**********************************************************************
static uint16_t asciiToAngle( uint8_t angleStr[] )
{
	uint16_t	val=0;
	
	for( uint8_t i=0 ; i<ANGLE_DIGIT ; i++ ){
		val <<= 4;
		val |= asciiHexToDec( angleStr[i] );
	}
	
	return( val );
}
//**********************************************************************
//**********************************************************************
static uint8_t asciiHexToDec( uint8_t asciiHex )
{
	for( uint8_t i=0 ; i<ASCII_NUM ; i++ ){				//8bit範囲桁数。2桁。magicnumber
		if( asciiHex == ASCII_TABLE[i][ASCII_CODE] ){
			return( ASCII_TABLE[i][HEX_VAL] );
		}
	}
	return( 0xFF );		// 異常。取り得ない
}
//**********************************************************************
//**********************************************************************
static uint16_t angleToSensor( uint16_t angle , uint8_t id )
{
	volatile short rangeMin = SENSER_RANGE[NO_WATER_TEMP][MIN];
	volatile short rangeMax = SENSER_RANGE[NO_WATER_TEMP][MAX];
	
	volatile uint16_t	sensorRange	= ( rangeMax - rangeMin ) ;
	volatile uint16_t	angleRange	= ( ANGLE_VAL_MAX - ANGLE_VAL_MIN );
	volatile double		exRaito		= ((double)sensorRange) / angleRange;

	volatile uint16_t	inMinOffset	= angle - ANGLE_VAL_MIN;

	volatile uint16_t	ret			= ( inMinOffset * exRaito ) + rangeMin;
	
	return( ret );
}
	
	
