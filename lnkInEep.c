#include <avr/io.h>
#include <stdbool.h>

#include "lnkInEep_inc.h"
#include "lnkInEep.h"

#include "hardware.h"
#include "aplData.h"
#include "drvEep.h"

static unsigned char	eepReadFlag;		//初回EEPデータ反映済みフラグ

//********************************************************************************
// 初期化
//********************************************************************************
void initLnkInEep( void )
{
	eepReadFlag = false;
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkInEepMain( void )
{
	DRV_EEP_READ	*inDrvEep;
	APL_DATA_EEP	aplDataEep;
	unsigned char	i;
	unsigned char	sum;

	//SW入力ドライバデータ取得
	inDrvEep = getDrvEep();

	if( eepReadFlag == false ){		//未反映
		if( inDrvEep->readState == DRV_EEP_READ_STATE_READED ){
			eepReadFlag = true;		//反映済み

			sum = 0;
			for( i=0 ; i<DRV_EEP_MAP_MAX-1 ; i++ ){	//SUMを除くデータ
				sum += inDrvEep->val[i];
			}
			//SUM正常
			if( sum == inDrvEep->val[DRV_EEP_MAP_SUM] ){
				i=0;
				aplDataEep.read				= APL_DATA_EEP_STATE_READED;
				aplDataEep.color7seg		= inDrvEep->val[i++];
				aplDataEep.bright7seg		= inDrvEep->val[i++];
				aplDataEep.brightDim7seg	= inDrvEep->val[i++];
				aplDataEep.dispcyc7seg		= inDrvEep->val[i++];
			}else{
				aplDataEep.read				= APL_DATA_EEP_STATE_SUMERROR;
				aplDataEep.color7seg		= 0;
				aplDataEep.bright7seg		= 0;
				aplDataEep.brightDim7seg	= 0;
				aplDataEep.dispcyc7seg		= 0;
			}
			setAplDataEep( &aplDataEep );
		}
	}
}
