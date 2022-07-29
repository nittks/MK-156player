#include <avr/io.h>
#include <stdbool.h>

#include "lnkInSw_inc.h"
#include "lnkInSw.h"
#include "aplData.h"
#include "drvInSw.h"

static APL_DATA_PUSH_SW judgePushSw( DRV_IN_PUSH_SW_STATE pushSwState );

//********************************************************************************
// 初期化
//********************************************************************************
void initLnkInSw( void )
{
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkInSwMain( void )
{
	DRV_IN_SW		*inDrvInSw;
	APL_DATA_SW		aplDataSw;

	//SW入力ドライバデータ取得
	inDrvInSw = getDrvInSw();

	//プッシュスイッチ判定
	aplDataSw.pushSwSet		= judgePushSw( inDrvInSw->pushSwState[PUSH_SW_SET] );

	//ポート入力
	aplDataSw.mk156Busy		=  ( inDrvInSw->mk156Busy == DRV_IN_PORT_LEVEL_HIGH )? true : false;	// Hgihがbusy


	setAplDataSw( &aplDataSw );
}

//********************************************************************************
// プッシュスイッチ入力判定
//********************************************************************************
static APL_DATA_PUSH_SW judgePushSw( DRV_IN_PUSH_SW_STATE pushSwState )
{
	APL_DATA_PUSH_SW	ret;

	if( pushSwState == DRV_IN_PUSH_SW_STATE_ON ){
		ret	= APL_DATA_PUSH_SW_ON;
	}else if( pushSwState == DRV_IN_PUSH_SW_STATE_LONGON ){
		ret	= APL_DATA_PUSH_SW_LONGON;
	}else if( pushSwState == DRV_IN_PUSH_SW_STATE_DOUBLEON ){
		ret	= APL_DATA_PUSH_SW_DOUBLEON;
	}else{
		ret	= APL_DATA_PUSH_SW_OFF;
	}
	return( ret );
}

