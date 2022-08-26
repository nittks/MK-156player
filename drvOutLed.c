#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  //割り込みを使用するため
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "drvOutLed_inc.h"
#include "drvOutLed.h"

//LINKよりセットされる
static DRV_OUT_LED		preDrvOutLed;

//********************************************************************************
// 初期化
//********************************************************************************
void initDrvOutLed( void )
{
	// AVR128DA28/32/48/64
	// 23.3.2 初期化

	// 1. 制御A(TCDn.CTRLA)ﾚｼﾞｽﾀでｸﾛｯｸ元と前置分周器を選んでください。
	TCD0.CTRLA	= TCD_CLKSEL_OSCHF_gc | TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV8_gc;
	// 2. 制御B(TCDn.CTRLB)ﾚｼﾞｽﾀで波形生成動作を選んでください。
	TCD0.CTRLB	= TCD_WGMODE_ONERAMP_gc;
	// 3. 任意選択: 他の静的ﾚｼﾞｽﾀを望む機能に構成設定してください。
	TCD0.CTRLC	= TCD_CMPCSEL_PWMA_gc | TCD_CMPDSEL_PWMB_gc;
	_PROTECTED_WRITE( TCD0.FAULTCTRL , 0x30 );		// エラッタ対応
	// 4. 比較(TCDn.CMPxSET/CLR)ﾚｼﾞｽﾀに初期値を書いてください。
	TCD0.CMPASET	= TCD0.CMPACLR	= PWM_TOP;		// TOPとWOBのOFFカウントがCMPBCLRで共通のため、TOP値を起点にする
	TCD0.CMPBSET	= TCD0.CMPBCLR	= PWM_TOP;
	// 5. 任意選択: 他の2重緩衝されたﾚｼﾞｽﾀに望む値を書いてください。
	// 多分ない
	// 6. 状態(TCDn.STATUS)ﾚｼﾞｽﾀの許可準備可(ENRDY)ﾋﾞｯﾄが’1’に設定されているのを確実にしてください。
	while( !(TCD0.STATUS & TCD_ENRDY_bm) ){
		;
	}
	// 7. 制御A(TCDn.CTRLA)ﾚｼﾞｽﾀの許可(ENABLE)ﾋﾞｯﾄに’1’を書くことによってTCDを許可してください。
	TCD0.CTRLA	|= TCD_ENABLE_bm;

	TCD0.CTRLE	|= TCD_SYNC_bm;		// 値をセットしたのでダブルバッファ更新指示。CMPxSET、
}

//********************************************************************************
// Lnkからセット
//********************************************************************************
void setDrvOutLed( DRV_OUT_LED *inP )
{
	if( inP->green != preDrvOutLed.green ){
		TCD0.CMPASET	= PWM_TOP - (((uint32_t)inP->green * PWM_TOP) / 100);
	}

	if( inP->blue != preDrvOutLed.blue ){
		TCD0.CMPBSET	= PWM_TOP - (((uint32_t)inP->blue * PWM_TOP) / 100);
	}

	TCD0.CTRLE	|= TCD_SYNC_bm;		// 値をセットしたのでダブルバッファ更新指示。CMPxSET
	preDrvOutLed = *inP;
}

//********************************************************************************
// メイン処理
//********************************************************************************
void drvOutLedMain( void )
{
	
}

