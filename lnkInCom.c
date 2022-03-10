#include <avr/io.h>
#include <assert.h>
#include <stdbool.h>

#include "aplData.h"
#include "drvUart.h"
#include "lnkInCom_inc.h"
#include "lnkInCom.h"

static uint8_t asciiToVal( uint8_t* angleStr );
static uint8_t valToSenser( uint8_t val , uint8_t id );

//初期化
void initLnkInCom( void )
{
}
//メイン処理
void lnkInComMain( void )
{
	DRV_UART_RX		*inDrvUartRx;
	APL_DATA_CAR	aplDataCar;
	
	inDrvUartRx = getDrvUartRx();
	if( inDrvUartRx == NULL ){
		aplDataCar.rx		= false;
		return;
	}

	aplDataCar.rx		= true;
	
	switch( inDrvUartRx->rxData[UART_DATAPOS_ID] ){
	case ID_WATER_TEMP:
		if( inDrvUartRx->rxData[UART_DATAPOS_CONTROL] == CONTROL_NORMAL_OPERATION ){
			uint8_t val = asciiToVal( &inDrvUartRx->rxData[UART_DATAPOS_ANGLE] );
			aplDataCar.waterTemp	= valToSenser( val , ID_WATER_TEMP );
		}
		break;
	}
		
	setAplDataCar( &aplDataCar );
}


static uint8_t asciiToVal( uint8_t* angleStr )
{
	uint8_t	val=0;
	
	for( uint8_t i=0 ; i<ANGLE_DIGIT ; i++ ){
		if( ('0' <= angleStr[i]) && (angleStr[i] <= '9') ){
			val |= angleStr[i] - '0';
		}else if( ('A' <= angleStr[i]) && (angleStr[i] <= 'F') ){
			val |= angleStr[i] - 'A' + 0xA;
		}else{
			//取りえない
		}
		val <<= 4;
	}
	
	return( val );
}

static uint8_t valToSenser( uint8_t val , uint8_t id )
{
	short rangeMin = SENSER_RANGE[ID_WATER_TEMP][MIN];
	short rangeMax = SENSER_RANGE[ID_WATER_TEMP][MAX];
	
	short ret = ( val - ANGLE_VAL_MIN ) * ( rangeMax - rangeMin ) / ( ANGLE_VAL_MAX - ANGLE_VAL_MIN ) + rangeMin;
	
	return( ret );
}
	
	

	
