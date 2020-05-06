#include <avr/io.h>

#include "lnkInSw_inc.h"
#include "lnkInSw.h"
#include "aplData.h"
#include "drvInSw.h"

static APL_DATA_ROT_ENC judgeRotEnc( DRV_IN_ROT_ENC_STATE rotEncState );
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

	//ロータリーエンコーダー判定
	aplDataSw.rotEncSet		= judgeRotEnc( inDrvInSw->rotEncState[ROT_ENC_SET] );
	//プッシュスイッチ判定
	aplDataSw.pushSwSet		= judgePushSw( inDrvInSw->pushSwState[PUSH_SW_SET] );
	aplDataSw.pushSwTest	= judgePushSw( inDrvInSw->pushSwState[PUSH_SW_TEST] );

	setAplDataSw( &aplDataSw );
}

//********************************************************************************
// ロータリーエンコーダー入力判定
//********************************************************************************
static APL_DATA_ROT_ENC judgeRotEnc( DRV_IN_ROT_ENC_STATE rotEncState )
{
	APL_DATA_ROT_ENC	ret;

	//ロータリーエンコーダー判定
	if( rotEncState == DRV_IN_ROT_ENC_STATE_FORWARD ){
		ret	= APL_DATA_ROT_ENC_UP;	
	}else if( rotEncState == DRV_IN_ROT_ENC_STATE_REVERCE ){
		ret	= APL_DATA_ROT_ENC_DOWN;
	}else{
		ret	= APL_DATA_ROT_ENC_STOP;
	}
	return( ret );
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
	}else{
		ret	= APL_DATA_PUSH_SW_OFF;
	}
	return( ret );
}
