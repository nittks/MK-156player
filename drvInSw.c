
#include <avr/io.h>
#include <avr/interrupt.h>  //割り込みを使用するため

#include "main.h"
#include "drvInSw_inc.h"
#include "drvInSw.h"
#include "hardware.h"

//出力データ
static DRV_IN_SW		drvInSwData;		//スイッチ入力データ

//内部状態
static PORT_PUSH_SW		portPushSw[PUSH_SW_NUM];
static unsigned char	grayCode[ROT_ENC_NUM];			//ROTENC入力。前回値を保存し、今回値と合わせグレイコード化する

static void inputPushSw( void );
//********************************************************************************//
// 初期化
//********************************************************************************//
void initDrvInSw( void )
{
	unsigned char	i;

	//出力
	for( i=0 ; i<ROT_ENC_NUM; i++ ){
		drvInSwData.pushSwState[i]	= DRV_IN_PUSH_SW_STATE_OFF;	//入力無し
	}
	for( i=0 ; i<PUSH_SW_NUM; i++ ){
		portPushSw[i].portIn		= PORT_OFF;
		portPushSw[i].portInLatch	= PORT_OFF;
		portPushSw[i].state			= PUSH_SW_STATE_OFF;
		portPushSw[i].cntTimeDebounce	= 0;
	}
	drvInSwData.mk156Busy	= DRV_IN_PORT_LEVEL_LOW;
	
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
	return( &drvInSwData );	// 値渡しにしたい。
}

//********************************************************************************//
// メイン処理
//********************************************************************************//
void drvInSwMain( void )
{
	cli();
	inputPushSw();
	drvInSwData.mk156Busy	= ( PORTC.IN & PIN3_bm ) >> PIN3_bp;
	sei();
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
					if( pSw->cntTimeJudge < PUSH_SW_LONGON_10MS ){
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
}
