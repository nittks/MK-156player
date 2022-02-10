#include <avr/io.h>
#include <stdbool.h>

#include "aplData.h"
#include "aplData_inc.h"
#include "hardware.h"

static APL_DATA_CAR		aplDataCar;
static APL_DATA_SW		aplDataSw;
static APL_DATA_EEP		aplDataEep;


//********************************************************************************
// 初期化
//********************************************************************************
void initAplData( void )
{
	
	aplDataCar.rx				= false;
	aplDataCar.sumerr			= false;
	
	aplDataCar.ig				= APL_DATA_IG_OFF;
	aplDataCar.acc				= APL_DATA_ACC_OFF;
	aplDataCar.ill				= APL_DATA_ILL_OFF;
	aplDataCar.speed			= 0;
	aplDataCar.rev				= 0;
	
	aplDataSw.rotEncSet			= APL_DATA_ROT_ENC_STOP;
	aplDataSw.pushSwSet			= APL_DATA_PUSH_SW_OFF;

	aplDataEep.read				= APL_DATA_EEP_STATE_UNREAD;
	aplDataEep.bright7seg		= 0;
	aplDataEep.brightDim7seg	= 0;
	aplDataEep.dispcyc7seg		= 0;
	aplDataEep.h				= 0;
	aplDataEep.s				= 0;
	aplDataEep.v				= 0;
}
//********************************************************************************
// メイン処理
//********************************************************************************
void aplDataMain( void )
{

}
//********************************************************************************
// LNKからセット
//********************************************************************************
void setAplDataCar( APL_DATA_CAR *inData )
{
	aplDataCar = *inData;
}
//********************************************************************************
// LNKからセット
//********************************************************************************
void setAplDataSw( APL_DATA_SW *inData )
{
	aplDataSw = *inData;
}
//********************************************************************************
// LNKからセット
//********************************************************************************
void setAplDataEep( APL_DATA_EEP *inData )
{
	aplDataEep = *inData;
}
//********************************************************************************
// APL_DATA取得
//********************************************************************************
APL_DATA_CAR *getAplDataCar(void)
{
	return( &aplDataCar );
}
//********************************************************************************
// APL_DATA取得
//********************************************************************************
APL_DATA_SW *getAplDataSw(void)
{
	return( &aplDataSw );
}
//********************************************************************************
// APL_DATA取得
//********************************************************************************
APL_DATA_EEP *getAplDataEep(void)
{
	return( &aplDataEep );
}
