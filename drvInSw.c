
#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため

#include "main.h"
#include "drvInSw_inc.h"
#include "drvInSw.h"
#include "hardware.h"

//出力データ
static DRV_IN_SW		drvInSwData;		//スイッチ入力データ

//内部状態
static ROT_ENC_STATE	rotEncState[ROT_ENC_NUM];		//ロータリーエンコーダー状態
static PORT_PUSH_SW		portPushSw[PUSH_SW_NUM];
static unsigned char	rotEncDebTimeCnt[ROT_ENC_NUM];	//デバウンス経過時間カウント
static unsigned char	grayCode[ROT_ENC_NUM];			//ROTENC入力。前回値を保存し、今回値と合わせグレイコード化する
static signed char		rotateVect[ROT_ENC_NUM];		//ROTENC。回転方向変化量カウント。逆方向はマイナス値のためsingedを使用

static void chkRotateVectCnt( unsigned char portNo );
static void inputRotEnc( void );
static void inputPushSw( void );
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
		rotEncDebTimeCnt[i]	= 0;		//デバウンス経過時間カウント
		grayCode[i]		= 0;
		rotateVect[i]	= 0;
	}
	for( i=0 ; i<PUSH_SW_NUM; i++ ){
		portPushSw[i].portIn		= PORT_OFF;
		portPushSw[i].portInLatch	= PORT_OFF;
		portPushSw[i].state			= PUSH_SW_STATE_OFF;
		portPushSw[i].cntTimeDebounce	= 0;
	}
	
	PORTD.DIRCLR	= 0x07;
	PORTD.PIN0CTRL	|= ( PORT_INVEN_bm | PORT_PULLUPEN_bm );	//反転設定で従来動作
	PORTD.PIN1CTRL	|= ( PORT_INVEN_bm | PORT_PULLUPEN_bm );
	PORTD.PIN2CTRL	|= ( PORT_PULLUPEN_bm );
	
	//割込みレジスタ設定(
	PORTD.INTFLAGS	= 0x03;		//PD0-1

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
	cli();

	inputRotEnc();
	inputPushSw();

	sei();
}

//********************************************************************************//
// ロータリーエンコーダー入力検知処理
//********************************************************************************//
static void inputRotEnc( void )
{
	uint8_t	i;
	
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
}

//********************************************************************************//
// プッシュスイッチ入力検知処理
//********************************************************************************//
static void inputPushSw( void )
{
	uint8_t			i;
	PORT_PUSH_SW*	pSw;

	//ポート入力
	portPushSw[NO_0].portIn	= (PORTD.IN>>PIN2_bp) & 0x01;
	
	//チャタリングキャンセル(トリガで拾って、ラッチで止める
	for( i=0 ; i<PUSH_SW_NUM; i++ ){
		pSw	= &portPushSw[i];		//記述が長いので短くする
		if( pSw->cntTimeDebounce <= PUSH_SW_DEBOUNCE_10MS ){
			pSw->cntTimeDebounce++;
		}else{
			if( pSw->portInLatch	!= pSw->portIn ){
				pSw->portInLatch		= pSw->portIn;
				pSw->cntTimeDebounce	= 0;
			}
		}
	}

	//判定状態遷移(わかりにくい、読みにくい
	for( i=0 ; i<PUSH_SW_NUM; i++ ){
		pSw	= &portPushSw[i];		//記述が長いので短くする
		switch( pSw->state ){
			case PUSH_SW_STATE_OFF:
				if( pSw->portInLatch == PORT_ON ){
					pSw->state	= PUSH_SW_STATE_ON;
				}
				drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_OFF;
				pSw->cntTimeJudge	= 0;
				break;

			case PUSH_SW_STATE_ON:
				if( pSw->portInLatch == PORT_ON ){
					if( pSw->cntTimeJudge <= PUSH_SW_LONGON_10MS ){
						pSw->cntTimeJudge++;
						if( pSw->cntTimeJudge == PUSH_SW_LONGON_10MS ){	//確定時、1発だけ送る
							drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_LONGON;
						}
					}else{
						drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_OFF;
					}
				}else{
					if( pSw->cntTimeJudge >= PUSH_SW_LONGON_10MS ){
						pSw->state	= PUSH_SW_STATE_OFF;
						drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_OFF;
					}else{
						pSw->state	= PUSH_SW_STATE_WAIT_DOUBLE;
						pSw->cntTimeJudge	= 0;
					}
				}
				break;

			case PUSH_SW_STATE_WAIT_DOUBLE:
				if( pSw->portInLatch == PORT_OFF ){
					if( pSw->cntTimeJudge <= PUSH_SW_DOUBLE_TIMEOUT_10MS ){
						pSw->cntTimeJudge++;
					}else{
						pSw->state	= PUSH_SW_STATE_OFF;
						drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_ON;	//短押し確定
					}
				}else{
						pSw->state	= PUSH_SW_STATE_DOUBLE;
						drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_DOUBLEON;
						pSw->cntTimeJudge	= 0;
				}
				break;
			case PUSH_SW_STATE_DOUBLE:
				drvInSwData.pushSwState[i] = DRV_IN_PUSH_SW_STATE_OFF;	//遷移時単発送信のため、遷移後OFF
				if( pSw->portInLatch == PORT_OFF ){
					pSw->state	= PUSH_SW_STATE_OFF;
				}
				break;
			default:
				break;
		}
	}
}

//********************************************************************************//
// ポート変化割り込み
//********************************************************************************//
void interPortD( void )
{
	unsigned char	portTmp;		//ロータリーエンコーダー全4入力一時保存

	cli();
	portTmp	= (~PORTD.IN) & 0x03;

	if( portTmp != (grayCode[NO_SET]&0x03)){	//ポート変化
		grayCode[NO_SET]	= (((grayCode[NO_SET] << 2) | portTmp) & 0x0F);
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

