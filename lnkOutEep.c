#include <stdbool.h>
#include "lnkOutEep_inc.h"
#include "lnkOutEep.h"
#include "aplCtrl.h"
#include "drvEep.h"

static unsigned char	eepReq;

//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutEep( void )
{
	eepReq = false;
}
//********************************************************************************
// APLよりセット
//********************************************************************************
void setLnkOutEep( void )
{
	eepReq = true;
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkOutEepMain( void )
{
	APL_CTRL_SET	*inAplCtrlSet;
	DRV_EEP_WRITE	outDrvEepWrite;
	unsigned char	i;
	
	inAplCtrlSet = getAplCtrlSet();

	//書込み要求有
	if( eepReq == true ){
		eepReq = false;

		i=0;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->bright7seg;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->brightDim7seg;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->dispcyc7seg;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->colorNo;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->colorRGB.red;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->colorRGB.green;
		outDrvEepWrite.val[i++]	= inAplCtrlSet->colorRGB.blue;
		
		//sum値計算
		outDrvEepWrite.val[DRV_EEP_MAP_SUM] = 0;
		for( i=0 ; i<DRV_EEP_MAP_SUM ; i++ ){
			outDrvEepWrite.val[DRV_EEP_MAP_SUM] += outDrvEepWrite.val[i];
		}
		setDrvEep( &outDrvEepWrite );
	}

}
