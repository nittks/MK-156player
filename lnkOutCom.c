#include <avr/io.h>
#include <string.h>
#include <stdbool.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplIndicator.h"
#include "drvUart.h"

#include "lnkOutCom_inc.h"
#include "lnkOutCom.h"

static unsigned char	txReqFlag;
static uint8_t			sendBuf[RING_BUF_SIZE][COMMAND_LEN_MAX]={0};	// MK156へ送信するデータのリングバッファ
static uint8_t			sendLen[RING_BUF_SIZE]={0};						// MK156へ送信するデータサイズのリングバッファ
static uint8_t			posRead;										// リングバッファ読込位置
static uint8_t			posWrite;										// リングバッファ書込位置
static INDICATOR_LIST		playingList;
static INDICATOR_LIST		requestList;
static DRV_UART_TX		outDrvUartTx;
static bool				resend;

static uint8_t createCmd( uint8_t* out , uint8_t fileNo );
//********************************************************************************
// 再生完了を受信したら、再生中リストの対象項目をクリアする
//********************************************************************************
void setReturnData( uint8_t cmd )
{
	// Folder = 01 , file = 01 でコマンド送信したら
	// 003 で再生完了レスポンスが帰ってきた。
	// 番号による判定は無しにして、再生完了のみで再生中リストをクリアする
	//playingList.byte &= ~(1<<(fileNo-1));
	if( cmd == 0x3D ){
		playingList.byte	= 0x00;
	}else if( cmd == 0x40 ){
		resend	= true;
	}
}
//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutCom( void )
{
	posRead		= 0;
	posWrite	= 0;

	playingList.byte	= 0x00;
	requestList.byte	= 0x00;

	for( uint8_t i=0 ; i<COMMAND_LEN_MAX ; i++ ){
		outDrvUartTx.txCommand[i] = 0x00;
	}
	resend = false;

}
//********************************************************************************
// 入力ユニット設定変更要求フレーム送信
//********************************************************************************
void setLnkOutCom( void )
{
	txReqFlag = true;
}
//********************************************************************************
// メイン処理
//********************************************************************************
void lnkOutComMain( void )
{
	APL_INDICATOR			*inAplIndicator			= getAplIndicator();

	if( inAplIndicator->waterOk ){
		requestList.waterTemp	= true;
	}

	if( inAplIndicator->vtec ){
		requestList.vtec	= true;
	}
/*	
	if( resend ){		// エラーのため、前回送信コマンドを再送
		setDrvUartTx( UART_0_MK156 , &outDrvUartTx	);
	}else
*/
	 if(	( requestList.byte != 0x00 ) //&&		// 再生要求が有る
//				( playingList.byte == 0x00 )		// 再生中ではない
	){
		uint8_t	bitNo;
		for( bitNo=0 ; bitNo<INDICATOR_NUM ; bitNo++ ){				// 再生要求があるbitNoを番号として抽出
			if( (requestList.byte & (1<<bitNo)) != 0x00 ){
				break;;
			}
		}
		requestList.byte &= ~(1<<bitNo);
//		playingList.byte |=  (1<<bitNo);
		sendLen[posWrite] = createCmd( sendBuf[posWrite]	, (uint8_t)(1<<bitNo) );

		// setDrvUartTxの使用に合わせて値を作る。仕様変更したい
		memcpy( &outDrvUartTx.txCommand[0] , sendBuf[posRead]	, sendLen[posRead] );

		setDrvUartTx( UART_0_MK156 , &outDrvUartTx	);

		posRead = (posRead >= RING_BUF_SIZE)? 0 : posRead+1;
	}
}

static uint8_t createCmd( uint8_t* out , uint8_t fileNo )
{
	uint8_t			byteCnt		= 0;
	volatile	static uint8_t		DEBUG_ACK	= ACK_OFF;

	out[byteCnt++]	= START_BYTE;
	out[byteCnt++]	= VERSION_INFO;
	out[byteCnt++]	= COMMAND_LEN;
	out[byteCnt++]	= COMMAND_PLAY_TRACK;
	out[byteCnt++]	= DEBUG_ACK;		//ACK_ON;
	out[byteCnt++]	= DEFAULT_FOLDER;
	out[byteCnt++]	= fileNo;

	uint16_t	sumCalc = 0;
	for(uint8_t i=0 ; i< COMMAND_LEN ; i++){
		sumCalc += out[i+1];	// +1 : START_BYTEは入らないのでversionから
	}
	sumCalc = 0xFFFF - sumCalc;
	sumCalc += 1;
	out[byteCnt++]	= (uint8_t)(( sumCalc >> 8 ) & 0x00FF);
	out[byteCnt++]	= (uint8_t)(( sumCalc      ) & 0x00FF);
	out[byteCnt++]	= END_BYTE;

	posWrite = (posWrite >= RING_BUF_SIZE)? 0 : posWrite + 1;

	return( byteCnt );

}

