
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため

#include "drvInSw_inc.h"
#include "drvInSw.h"
#include "hardware.h"

//出力データ
static DRV_IN_SW		drvInSwData;		//スイッチ入力データ

//内部状態
static ROT_ENC_STATE	rotEncState[ROT_ENC_NUM];		//ロータリーエンコーダー状態
static PUSH_SW_STATE	pushSwState[PUSH_SW_NUM];		//プッシュスイッチ状態
static unsigned char	rotEncDebTimeCnt[ROT_ENC_NUM];	//デバウンス経過時間カウント
static unsigned char	pushSwDebTimeCnt[PUSH_SW_NUM];	//デバウンス経過時間カウント
static unsigned char	grayCode[ROT_ENC_NUM];			//ROTENC入力。前回値を保存し、今回値と合わせグレイコード化する
static signed char		rotateVect[ROT_ENC_NUM];		//ROTENC。回転方向変化量カウント。逆方向はマイナス値のためsingedを使用

static void chkRotateVectCnt( unsigned char portNo );
//********************************************************************************//
// 初期化
//********************************************************************************//
void initDrvInSw( void )
{
	unsigned char	i;

	//出力
	for( i=0 ; i<ROT_ENC_NUM; i++ ){
		drvInSwData.rotEncState[i]	= DRV_IN_ROT_ENC_STATE_STOP;	//入力無し
		drvInSwData.pushSwState[i]	= DRV_IN_PUSH_SW_STATE_OFF;	//入力無し

		rotEncState[i]	= ROT_ENC_STATE_WAIT;	//監視開始
		pushSwState[i]	= PUSH_SW_STATE_OFF;
		rotEncDebTimeCnt[i]	= 0;		//デバウンス経過時間カウント
		pushSwDebTimeCnt[i]	= 0;		//デバウンス経過時間カウント
		grayCode[i]		= 0;
		rotateVect[i]	= 0;
	}
	
	//割込みレジスタ設定(_inc.h内で定義
	PCICR	= SET_PCICR;
	PCMSK0	= SET_PCMSK0;
	PCMSK1	= SET_PCMSK1;
	
}

//********************************************************************************//
// データ取得
//********************************************************************************//
DRV_IN_SW *getDrvInSw( void )
{
	return( &drvInSwData );
}

//********************************************************************************//
// メイン処理
//********************************************************************************//
void drvInSwMain( void )
{
	unsigned char	i;
	unsigned char	portTmp;

	cli();
	/****************************************/
	// ロータリーエンコーダー入力検知処理
	/****************************************/
	
	for( i=0 ; i<ROT_ENC_NUM; i++ ){
		//正転
		if( rotEncState[i] == ROT_ENC_STATE_FORWARD ){
			rotEncState[i] = ROT_ENC_STATE_DEBOUNCE;
			rotEncDebTimeCnt[i] = 0;
			drvInSwData.rotEncState[i]	= DRV_IN_ROT_ENC_STATE_FORWARD;
		//逆転
		}else if( rotEncState[i] == ROT_ENC_STATE_REVERCE ){
			rotEncState[i] = ROT_ENC_STATE_DEBOUNCE;
			rotEncDebTimeCnt[i] = 0;
			drvInSwData.rotEncState[i]	= DRV_IN_ROT_ENC_STATE_REVERCE;
		//デバウンス待機
		}else if( rotEncState[i] == ROT_ENC_STATE_DEBOUNCE ){
			
			drvInSwData.rotEncState[i]	= DRV_IN_ROT_ENC_STATE_STOP;
			//デバウンス経過
			if( rotEncDebTimeCnt[i] >= ROT_ENC_DEBTIME ){
				rotEncDebTimeCnt[i] = 0;
				rotEncState[i] = ROT_ENC_STATE_WAIT;
			}else{
				rotEncDebTimeCnt[i]++;
			}
		}
	}
	/****************************************/
	// プッシュスイッチ入力検知処理
	/****************************************/
	portTmp	= ((~PINB)>>PB4) & 0x03;		//2ポート分保存,ONがLOWのため反転

	for( i=0 ; i<PUSH_SW_NUM; i++ ){
		if( pushSwState[i] == PUSH_SW_STATE_OFF ){
			//オフ
			drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_OFF;
		}else if( pushSwState[i] == PUSH_SW_STATE_ON ){
			//オン
			pushSwDebTimeCnt[i] = 0;
			pushSwState[i] = PUSH_SW_STATE_DEBOUNCE;
		}else if( pushSwState[i] == PUSH_SW_STATE_DEBOUNCE ){
			//デバウンス待ち
			pushSwDebTimeCnt[i]++;
			if( ((portTmp & (PORT_ON<<i))>>i) == PORT_OFF ){
				if( pushSwDebTimeCnt[i] >= PUSH_SW_DEBTIME ){
					//短押
					pushSwState[i] = PUSH_SW_STATE_OFF;
					drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_ON;
				}
			}else{
				if( pushSwDebTimeCnt[i] >= PUSH_SW_LONGTIME ){
					//長押し
					pushSwState[i] = PUSH_SW_STATE_OFF;
					drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_LONGON;
				}
			}
		}
	}
	sei();
}

//********************************************************************************//
// ポート変化割り込み
//********************************************************************************//
void interPcInt08_14( void )
{
	unsigned char	portTmp;		//ロータリーエンコーダー全4入力一時保存

	cli();
	portTmp	= ((~PINC) >>PC4) & 0x03;		//4ポート分保存,ONがLOWのため反転

	if( (portTmp&ROT_ENC_0_POS) != (grayCode[NO_SET]&0x03)){	//ポート変化
		grayCode[NO_SET]	= ((grayCode[NO_SET] << 2) | (portTmp & ROT_ENC_0_POS)) & 0x0F;	//4bitのみ使用
		chkRotateVectCnt( NO_SET );
	}


	sei();
}
//********************************************************************************//
// 回転変化量チェック
//********************************************************************************//
static void chkRotateVectCnt( unsigned char portNo )
{
	//グレイコードテーブルをもとに、回転方向変化量を加算していく
	rotateVect[portNo]		+=grayCodeTable[ grayCode[portNo] ];

	//変化量が4or-4で回転方向決定
	if( rotateVect[portNo] >= ROT_VECT_FORWARD ){
		rotEncState[portNo] = ROT_ENC_STATE_FORWARD;
		rotateVect[portNo]	 = 0;
	}else if( rotateVect[portNo] <= ROT_VECT_REVERCE ){
		rotEncState[portNo] = ROT_ENC_STATE_REVERCE;
		rotateVect[portNo]	 = 0;
	}
}

//********************************************************************************//
// ポート変化割り込み
//********************************************************************************//
void interPcInt00_07( void )
{
	unsigned char	portTmp;
	unsigned char	i;

	cli();
	portTmp	= ((~PINB)>>PB4) & 0x03;		//2ポート分保存,ONがLOWのため反転

	for( i=0 ; i<PUSH_SW_NUM ; i++ ){
		if((pushSwState[i] == PUSH_SW_STATE_OFF) &&
		   (((portTmp>>i) & (PORT_ON)) == PORT_ON )){
			pushSwState[i] = PUSH_SW_STATE_ON;
		}
	}
	sei();
}
