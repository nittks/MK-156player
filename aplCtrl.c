#include <avr/io.h>
#include <stdbool.h>
#include "aplCtrl_inc.h"
#include "aplCtrl.h"
#include "aplData.h"
#include "lnkOutEep.h"
#include "lnkOutCom.h"
#include "hardware.h"
#include "aplCtrlSetting.h"

static APL_CTRL				aplCtrl;
static APL_CTRL_SET_PALSE	aplCtrlSetPalseBak;
static APL_CTRL_SETTING		aplCtrlSetting;

static void stateJudge( void );
static void judgeSetting( void );
static void procSetting( void );
static void chkSetPalse( void );
static void apryEep( void );
//********************************************************************************
// 初期化
//********************************************************************************
void initAplCtrl( void )
{
	aplCtrl.state			= APL_CTRL_STATE_BOOT;
	aplCtrl.stateSet		= APL_CTRL_STATE_SETTING_BRIGHT;
	aplCtrl.stateColorUser	= APL_CTRL_STATE_SET_COLOR_USER_UNSELECT;
	aplCtrlSetPalse.speed	= INIT_PALSE_SPEED;
	aplCtrlSetPalse.rev		= INIT_PALSE_REV;
	
	aplCtrlSetting			= aplCtrlSetting_new( settingContents0 ,  HIERARCHY_NUM );	
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
		if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_DOUBLEON ){
			//通常->テスト表示
			aplCtrl.state		= APL_CTRL_STATE_TESTDISP;
			aplCtrl.stateTest	= APL_CTRL_STATE_TEST_AUTO;
		}else if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_LONGON ){
			//通常->設定
			aplCtrl.state = APL_CTRL_STATE_SETTING;

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
		if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_DOUBLEON ){
			aplCtrl.state = APL_CTRL_STATE_NOMARL;
		}
		switch( aplCtrl.stateTest ){
		case APL_CTRL_STATE_TEST_AUTO:
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateTest = APL_CTRL_STATE_TEST_SPEED;
			}
			break;
		case APL_CTRL_STATE_TEST_SPEED:
			if( inAplDataSw->pushSwSet == APL_DATA_PUSH_SW_ON ){
				aplCtrl.stateTest = APL_CTRL_STATE_TEST_AUTO;
			}
			break;
		}
		break;
		
	//****************************************
	// 設定
	//****************************************
	case APL_CTRL_STATE_SETTING:		//設定
		judgeSetting();
		break;
		
	default:break;
	}
}

void judgeSetting( void )
{
	APL_DATA_SW		*inAplDataSw;
	inAplDataSw		= getAplDataSw();
	bool			isNextHierarchy;
	
	switch( inAplDataSw->pushSwSet ){
		case APL_DATA_PUSH_SW_DOUBLEON:
			if( aplCtrlSetting->getNowHierarchy(aplCtrlSetting) <= 0 ){
				aplCtrlSetting->exitSetting(aplCtrlSetting);
				aplCtrl.state	= APL_CTRL_STATE_NOMARL;
			}else{
				aplCtrlSetting->preSettingRestore(aplCtrlSetting);
				aplCtrlSetting->changeHierarchyUp(aplCtrlSetting);
			}
			break;
		case APL_DATA_PUSH_SW_ON:
			isNextHierarchy = aplCtrlSetting->isNextHierarchy(aplCtrlSetting);
			if( isNextHierarchy == true ){
				aplCtrlSetting->changeHierarchyDown(aplCtrlSetting);
			}else{
				aplCtrlSetting->saveSetting(aplCtrlSetting);
				aplCtrlSetting->changeHierarchyUp(aplCtrlSetting);
			}
			break;
		case  APL_DATA_PUSH_SW_LONGON:
			setLnkOutEep();	//EEPROM書込み要求
			chkSetPalse();	//パルス設定変更有無チェック
			aplCtrl.state	= APL_CTRL_STATE_NOMARL;
			aplCtrlSetting->exitSetting(aplCtrlSetting);
			break;
		default:break;
	}
	
	switch( inAplDataSw->rotEncSet ){
		case APL_DATA_ROT_ENC_UP:	aplCtrlSetting->selectUp(aplCtrlSetting);		break;
		case APL_DATA_ROT_ENC_DOWN:	aplCtrlSetting->selectDown(aplCtrlSetting);	break;
		default:break;
	}
}
//********************************************************************************
//********************************************************************************
void procSetting( void )
{
	aplCtrlSet.dspVal	= aplCtrlSetting->getDspVal(aplCtrlSetting);
	aplCtrl.stateSet	= (APL_CTRL_STATE_SET)aplCtrlSetting->getState(aplCtrlSetting);
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
		aplCtrlSet.colorNo				= inAplDataEep->color7seg;
		aplCtrlSet.colorRGB.red			= inAplDataEep->red;
		aplCtrlSet.colorRGB.green		= inAplDataEep->green;
		aplCtrlSet.colorRGB.blue		= inAplDataEep->blue;
		aplCtrlSet.bright7seg			= inAplDataEep->bright7seg;
		aplCtrlSet.brightDim7seg		= inAplDataEep->brightDim7seg;
		aplCtrlSet.dispcyc7seg			= inAplDataEep->dispcyc7seg;
	}else if( inAplDataEep->read == APL_DATA_EEP_STATE_SUMERROR){
		//SUMエラー時はデフォルト値読込
		aplCtrlSet.colorNo				= eepDefault[EEP_COLOR_7SEG];
		aplCtrlSet.colorRGB.red			= eepDefault[EEP_COLOR_RED];
		aplCtrlSet.colorRGB.green		= eepDefault[EEP_COLOR_GREEN];
		aplCtrlSet.colorRGB.blue		= eepDefault[EEP_COLOR_BLUE];
		aplCtrlSet.bright7seg			= eepDefault[EEP_BRIGHT_7SEG];
		aplCtrlSet.brightDim7seg		= eepDefault[EEP_BRIGHT_DIM_7SEG];
		aplCtrlSet.dispcyc7seg			= eepDefault[EEP_DISPCYC_7SEG];
		setLnkOutEep();	//EEPROM書込み要求
	}
}
