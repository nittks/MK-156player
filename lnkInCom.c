#include <avr/io.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "aplData.h"
#include "drvUart.h"
#include "drvInCarSw.h"
#include "lnkInCom_inc.h"
#include "lnkInCom.h"

static void procCarSw( APL_DATA_CAR* aplDataCar );
static uint16_t asciiToAngle( uint8_t* angleStr );
static uint8_t asciiHexToDec( uint8_t asciiHex );
static uint16_t angleToSensor( uint16_t angle , uint8_t id );

//初期化
void initLnkInCom( void )
{
}
//メイン処理
void lnkInComMain( void )
{
	volatile	DRV_UART_RX		*inDrvUartRx;
	APL_DATA_CAR	*inAplDataCar	= getAplDataCar();	// 現在値を取得し、更新された値だけ書き換え、再セットする
	APL_DATA_CAR	outAplDataCar	= *inAplDataCar;
	
	//車両スイッチ、信号処理
	procCarSw( &outAplDataCar );
	
	inDrvUartRx = getDrvUartRx( UART_1_DEFI );
	if( inDrvUartRx == NULL ){
		outAplDataCar.rx		= false;
		return;
	}

	outAplDataCar.rx		= true;
	
	switch( inDrvUartRx->rxData[UART_DATAPOS_ID] ){
	case ID_WATER_TEMP:
		if( inDrvUartRx->rxData[UART_DATAPOS_CONTROL] == CONTROL_NORMAL_OPERATION ){
			volatile uint16_t angle = asciiToAngle( &inDrvUartRx->rxData[UART_DATAPOS_ANGLE] );
			outAplDataCar.waterTemp	= angleToSensor( angle , ID_WATER_TEMP );
		}
		break;
	default:
		break;
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

static uint16_t asciiToAngle( uint8_t angleStr[] )
{
	uint16_t	val=0;
	
	for( uint8_t i=0 ; i<ANGLE_DIGIT ; i++ ){
		val <<= 4;
		val |= asciiHexToDec( angleStr[i] );
	}
	
	return( val );
}
static uint8_t asciiHexToDec( uint8_t asciiHex )
{
	for( uint8_t i=0 ; i<ASCII_NUM ; i++ ){				//8bit範囲桁数。2桁。magicnumber
		if( asciiHex == ASCII_TABLE[i][ASCII_CODE] ){
			return( ASCII_TABLE[i][HEX_VAL] );
		}
	}
	return( 0xFF );		// 異常。取り得ない
}

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
	
	
