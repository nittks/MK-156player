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
static uint8_t		posRead;

static void procCarSw( APL_DATA_CAR* aplDataCar );
static bool pickRxFrame( uint8_t* rxFrameData );
static uint16_t rxDataNum( void );
static uint16_t asciiToAngle( uint8_t* angleStr );
static uint8_t asciiHexToDec( uint8_t asciiHex );
static uint16_t angleToSensor( uint16_t angle , uint8_t id );

//**********************************************************************
//初期化
//**********************************************************************
void initLnkInCom( void )
{
	posRead		= 0;

}
//**********************************************************************
//メイン処理
//**********************************************************************
void lnkInComMain( void )
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
				PORTF.OUT = (~(PORTF.IN & (0x30))) | (PORTF.IN & (~0x30));

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
	DRV_UART_RX_DEFI inDrvUartRx;
	inDrvUartRx = getDrvUartRxDefi();

	if( rxDataNum() < DEFI_MAX ){		// 受信データが1フレーム分ない
		return( false );
	}

	uint8_t	byteCnt=0;
	while( byteCnt < DEFI_MAX ){
		uint8_t rxData = inDrvUartRx.rxData[posRead++];
		if( posRead >= DRV_UART_RX_RING_BUF_SIZE ){
			posRead = 0;
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
	return( true );
}
//**********************************************************************
//**********************************************************************
static uint16_t rxDataNum( void )
{
	DRV_UART_RX_DEFI	inDrvUartRx;
	inDrvUartRx = getDrvUartRxDefi();
	uint8_t				ret;

	if( posRead <= inDrvUartRx.posWrite ){
		ret	= inDrvUartRx.posWrite - posRead;
	}else{
		ret	= (DRV_UART_RX_RING_BUF_SIZE - posRead) + inDrvUartRx.posWrite;
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

	volatile uint16_t ret = ( inMinOffset * exRaito ) + rangeMin;
	
	return( ret );
}
	
	
