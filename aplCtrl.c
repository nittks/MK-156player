#include <avr/io.h>
#include <stdbool.h>
#include "aplCtrl_inc.h"
#include "aplCtrl.h"
#include "aplData.h"
#include "lnkOutEep.h"
#include "lnkOutCom.h"
#include "hardware.h"

static APL_CTRL				aplCtrl;
static APL_CTRL_SET			aplCtrlSet;
static APL_CTRL_SET			aplCtrlSetBak;		//設定保存せず抜ける祭に戻す、元の値保持用
static APL_CTRL_SET_PALSE	aplCtrlSetPalse;
static APL_CTRL_SET_PALSE	aplCtrlSetPalseBak;

static void stateJudge( void );
static void procSetting( void );
static void changeSettingVal( unsigned char *val );
static void changeSettingItem( unsigned char *setNo , SET_ITEM_NO no );
static void chkSetPalse( void );
static void apryEep( void );
//********************************************************************************
// 初期化
//********************************************************************************
void initAplCtrl( void )
{
	aplCtrl.state			= APL_CTRL_STATE_BOOT;
	aplCtrl.stateSet		= APL_CTRL_STATE_SET_BRIGHT_7SEG;
	aplCtrlSetPalse.speed	= INIT_PALSE_SPEED;
	aplCtrlSetPalse.rev		= INIT_PALSE_REV;
	
}
//********************************************************************************
// 制御状態取得
//********************************************************************************
APL_CTRL *getAplCtrl( void )
{
	return( &aplCtrl );
}
//********************************************************************************
// 設定値取得
//********************************************************************************
APL_CTRL_SET *getAplCtrlSet( void )
{
	return( &aplCtrlSet );
}
//********************************************************************************
// パルス設定値取得
//********************************************************************************
APL_CTRL_SET_PALSE *getAplCtrlSetPalse( void )
{
	return( &aplCtrlSetPalse );
}
//********************************************************************************
// メイン処理
//********************************************************************************
void aplCtrlMain( void )
{
//	PORTB = (PORTB & 0xFE) | (~(PORTB & 0x01) );

	//状態判定、状態遷移
	stateJudge();

	//設定処理
	procSetting();
}
//********************************************************************************
// 状態判定、状態遷移
//********************************************************************************
static void stateJudge( void )
{
	APL_DATA_CAR	*inAplDataCar;
	APL_DATA_SW		*inAplDataSw;
	APL_DATA_EEP	*inAplDataEep;

	inAplDataCar	= getAplDataCar();
	inAplDataSw		= getAplDataSw();
	inAplDataEep	= getAplDataEep();

	switch( aplCtrl.state ){
	//****************************************
	// 初回起動
	//****************************************
	case APL_CTRL_STATE_BOOT:		//起動初回
		if( inAplDataEep->read != APL_DATA_EEP_STATE_UNREAD ){		//EEPROM読み込み済み
			apryEep();		//初回はEEPROMのデータを適用する
			aplCtrl.state = APL_CTRL_STATE_NOMARL;
		}
		break;

	//****************************************
	// 通常
	//****************************************
	case APL_CTRL_STATE_NOMARL:		//通常
		if( inAplDataSw->pushSwTest == APL_DATA_PUSH_SW_LONGON ){
			//通常->テスト表示
			aplCtrl.state = APL_CTRL_STATE_TESTDISP;
			aplCtrl.stateTest = APL_CTRL_STATE_TEST_AUTO;
		}else if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_LONGON ){
			//通常->設定
			aplCtrl.state = APL_CTRL_STATE_SETTING;
			if( inAplDataCar->ill == APL_DATA_ILL_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG;
			}else{
				aplCtrl.stateSet = APL_CTRL_STATE_SET_BRIGHT_7SEG;
			}
			//入力ユニットから受信しているパルス設定値を現在設定値として取得
			aplCtrlSetPalse.speed	= inAplDataCar->palseSetSpeed;
			aplCtrlSetPalse.rev		= inAplDataCar->palseSetRev;

			//変更前の値を保存(キャンセル時戻すため
			//設定
			aplCtrlSetBak = aplCtrlSet;
			//パルス設定
			aplCtrlSetPalse.speed		= inAplDataCar->palseSetSpeed;
			aplCtrlSetPalse.rev			= inAplDataCar->palseSetRev;
			aplCtrlSetPalseBak.speed	= inAplDataCar->palseSetSpeed;
			aplCtrlSetPalseBak.rev		= inAplDataCar->palseSetRev;

		}
		break;

	//****************************************
	// テスト表示
	//****************************************
	case APL_CTRL_STATE_TESTDISP:		//テストモード
		if( inAplDataSw->pushSwTest == APL_DATA_PUSH_SW_LONGON ){
			aplCtrl.state = APL_CTRL_STATE_NOMARL;
		}
		switch( aplCtrl.stateTest ){
		case APL_CTRL_STATE_TEST_AUTO:
			if( inAplDataSw->pushSwTest == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateTest = APL_CTRL_STATE_TEST_SPEED;
			}
			break;
		case APL_CTRL_STATE_TEST_SPEED:
			if( inAplDataSw->pushSwTest == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateTest = APL_CTRL_STATE_TEST_AUTO;
			}
			break;
		}
		break;
		
	//****************************************
	// 設定
	//****************************************
	case APL_CTRL_STATE_SETTING:		//設定
		switch( aplCtrl.stateSet ){
		case APL_CTRL_STATE_SET_BRIGHT_7SEG:		//調光(7セグ
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_DISPCYC_7SEG;
			}else if( inAplDataCar->ill == APL_DATA_ILL_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG;
			}
			break;
		case APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG:	//調光減光(7セグ
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_DISPCYC_7SEG;
			}else if( inAplDataCar->ill == APL_DATA_ILL_OFF ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_DISPCYC_7SEG;
			}
			break;
		case APL_CTRL_STATE_SET_DISPCYC_7SEG:		//表示更新速度(7セグ
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_PALSE_SPEED;
			}
			break;
		case APL_CTRL_STATE_SET_PALSE_SPEED:		//パルス仕様車速 
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateSet = APL_CTRL_STATE_SET_PALSE_REV;
			}
			break;
		case APL_CTRL_STATE_SET_PALSE_REV:			//パルス仕様回転数 
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				if( inAplDataCar->ill == APL_DATA_ILL_ON ){
					aplCtrl.stateSet = APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG;
				}else{
					aplCtrl.stateSet = APL_CTRL_STATE_SET_BRIGHT_7SEG;
				}
			}
			break;
		default:
			break;
		}

		//設定から出る
		if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_LONGON ){
			setLnkOutEep();	//EEPROM書込み要求
			chkSetPalse();	//パルス設定変更有無チェック
			aplCtrl.state	= APL_CTRL_STATE_NOMARL;
		}else if( inAplDataSw->pushSwTest == APL_DATA_PUSH_SW_LONGON ){
			aplCtrl.state	= APL_CTRL_STATE_NOMARL;
			aplCtrlSet		= aplCtrlSetBak;		//変更前の値に戻す
		}
		break;
	}
}
//********************************************************************************
// 設定サブルーチン
//********************************************************************************
static void procSetting( void )
{
	switch( aplCtrl.state ){
	case APL_CTRL_STATE_BOOT:		//起動初回
		//無処理
		break;

	case APL_CTRL_STATE_NOMARL:		//通常
		//無処理
		break;

	case APL_CTRL_STATE_TESTDISP:		//テストモード
		//無処理
		break;
		
	case APL_CTRL_STATE_SETTING:		//設定
		switch( aplCtrl.stateSet ){
		case APL_CTRL_STATE_SET_COLOR_7SEG:			//調色
			changeSettingItem( &aplCtrlSet.color7seg ,SET_COLOR);
			break;
		case APL_CTRL_STATE_SET_BRIGHT_7SEG:			//調光(7セグ
			changeSettingVal( &aplCtrlSet.bright7seg );
			break;
		case APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG:		//調光減光(7セグ
			changeSettingVal( &aplCtrlSet.brightDim7seg );
			break;
		case APL_CTRL_STATE_SET_DISPCYC_7SEG:		//表示更新速度(7セグ
			changeSettingVal( &aplCtrlSet.dispcyc7seg );
			break;
		case APL_CTRL_STATE_SET_PALSE_SPEED:			//パルス仕様車速 
			changeSettingItem(	&aplCtrlSetPalse.speed ,SET_PALSE_SPEED);
			break;
		case APL_CTRL_STATE_SET_PALSE_REV:			//パルス仕様回転数 
			changeSettingItem(	&aplCtrlSetPalse.rev,SET_PALSE_REV);
			break;
		default:
			break;
		}
		break;
	}
}

//********************************************************************************
// 設定値変更
//********************************************************************************
void changeSettingVal( unsigned char *val )
{
	APL_DATA_SW		*inAplDataSw;
	inAplDataSw		= getAplDataSw();

	if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_UP ){
		//上限ブロック
		if( *val < ( SETTING_VAL_MAX - SETTING_VAL_INTERVAL ) ){
			*val += SETTING_VAL_INTERVAL;
		}else{
			*val = SETTING_VAL_MAX;
		}
	}else if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_DOWN ){
		//下限ブロック
		if( *val > ( SETTING_VAL_MIN + SETTING_VAL_INTERVAL ) ){
			*val -= SETTING_VAL_INTERVAL;
		}else{
			*val = SETTING_VAL_MIN;
		}
	}
}
//********************************************************************************
// パルス設定値変更
//********************************************************************************
void changeSettingItem( unsigned char *setNo , SET_ITEM_NO no )
{
	APL_DATA_SW		*inAplDataSw;
	inAplDataSw		= getAplDataSw();

	if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_UP ){
		//上限ブロック
		if( *setNo < PALSE_ITEM_MAX[no] ){
			*setNo += 1;
		}else{
			*setNo = PALSE_ITEM_MIN[no];
		}
	}else if( inAplDataSw->rotEncSet == APL_DATA_ROT_ENC_DOWN ){
		//下限ブロック
		if( *setNo > PALSE_ITEM_MIN[no] ){
			*setNo -= 1;
		}else{
			*setNo = PALSE_ITEM_MAX[no];
		}
	}
}

//********************************************************************************
// パルス仕様設定変更チェック&変更要求
//********************************************************************************
static void chkSetPalse( void )
{
	//変更値チェック
	if(( aplCtrlSetPalse.speed != aplCtrlSetPalseBak.speed ) ||
	   ( aplCtrlSetPalse.rev != aplCtrlSetPalseBak.rev ))
	{
		//入力ユニットへ設定値変更通信送信
		setLnkOutCom();
	}
}
		
//********************************************************************************
// EEPROMデータ適用
//********************************************************************************
static void apryEep( void )
{
	APL_DATA_EEP	*inAplDataEep;

	inAplDataEep	= getAplDataEep();

	if( inAplDataEep->read == APL_DATA_EEP_STATE_READED){
		//読込済みなら反映
		aplCtrlSet.color7seg			= inAplDataEep->color7seg;
		aplCtrlSet.bright7seg			= inAplDataEep->bright7seg;
		aplCtrlSet.brightDim7seg		= inAplDataEep->brightDim7seg;
		aplCtrlSet.dispcyc7seg			= inAplDataEep->dispcyc7seg;
	}else if( inAplDataEep->read == APL_DATA_EEP_STATE_SUMERROR){
		//SUMエラー時はデフォルト値読込
		aplCtrlSet.color7seg			= eepDefault[COLOR_7SEG];
		aplCtrlSet.bright7seg			= eepDefault[BRIGHT_7SEG];
		aplCtrlSet.brightDim7seg		= eepDefault[BRIGHT_DIM_7SEG];
		aplCtrlSet.dispcyc7seg			= eepDefault[DISPCYC_7SEG];
		setLnkOutEep();	//EEPROM書込み要求
	}
}
