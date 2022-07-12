#include <avr/io.h>
#include <assert.h>
#include <stdbool.h>

#include "aplData.h"
#include "drvUart.h"
#include "drvInCarSw.h"
#include "lnkInCom_inc.h"
#include "lnkInCom.h"

static void procCarSw( APL_DATA_CAR* aplDataCar );
static uint16_t asciiToAngle( uint8_t* angleStr );
static uint16_t angleToSensor( uint16_t angle , uint8_t id );

//初期化
void initLnkInCom( void )
{
}
//メイン処理
void lnkInComMain( void )
{
	DRV_UART_RX		*inDrvUartRx;
	APL_DATA_CAR	aplDataCar;
	
	//車両スイッチ、信号処理
	procCarSw( &aplDataCar );
	
	inDrvUartRx = getDrvUartRx( UART_1_DEFI );
	if( inDrvUartRx == NULL ){
		aplDataCar.rx		= false;
		return;
	}

	aplDataCar.rx		= true;
	
	switch( inDrvUartRx->rxData[UART_DATAPOS_ID] ){
	case ID_WATER_TEMP:
		if( inDrvUartRx->rxData[UART_DATAPOS_CONTROL] == CONTROL_NORMAL_OPERATION ){
			volatile uint16_t angle = asciiToAngle( &inDrvUartRx->rxData[UART_DATAPOS_ANGLE] );
			aplDataCar.waterTemp	= angleToSensor( angle , ID_WATER_TEMP );
		}
		break;
	}
		
	setAplDataCar( &aplDataCar );
}

//**********************************************************************
//車両スイッチ、信号処理
//**********************************************************************
static void procCarSw( APL_DATA_CAR* aplDataCar )
{
	DRV_IN_CAR_SW	*inDrvInCarSw;
	inDrvInCarSw	= getDrvInCarSw();
	aplDataCar->vtc	= (inDrvInCarSw->vtc == DRV_IN_CAR_SW_ON)? true : false;
}

static uint16_t asciiToAngle( uint8_t* angleStr )
{
	uint16_t	val=0;
	
	for( uint8_t i=0 ; i<ANGLE_DIGIT ; i++ ){
		val <<= 4;
		val |= angleStr[i] & 0x0F;
	}
	
	return( val );
}

static uint16_t angleToSensor( uint16_t angle , uint8_t id )
{
	volatile short rangeMin = SENSER_RANGE[NO_WATER_TEMP][MIN];
	volatile short rangeMax = SENSER_RANGE[NO_WATER_TEMP][MAX];
	
	volatile uint16_t	sensorRange	= ( rangeMax - rangeMin ) ;
	volatile uint16_t	angleRange	= ( ANGLE_VAL_MAX - ANGLE_VAL_MIN );
	volatile double		exRaito		= (double)sensorRange / angleRange;

	volatile uint16_t	inMinOffset	= angle - ANGLE_VAL_MIN;

	volatile uint16_t ret = ( inMinOffset * exRaito ) + rangeMin;
	
	return( ret );
}
	
	
