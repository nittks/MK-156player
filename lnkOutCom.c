#include <avr/io.h>
#include <string.h>
#include <stdbool.h>

#include "aplData.h"
#include "aplCtrl.h"
#include "aplSound.h"
#include "drvUart.h"

#include "lnkOutCom_inc.h"
#include "lnkOutCom.h"

static unsigned char	txReqFlag;
static uint8_t			sendBuf[RING_BUF_SIZE][COMMAND_LEN_MAX]={0};	// MK156へ送信するデータのリングバッファ
static uint8_t			sendLen[RING_BUF_SIZE]={0};						// MK156へ送信するデータサイズのリングバッファ
static uint8_t			posRead;										// リングバッファ読込位置
static uint8_t			posWrite;										// リングバッファ書込位置


static uint8_t createCmd( uint8_t* out , uint8_t fileNo );
static void SoundPlay( DRV_UART_TX* out , uint8_t* commandCnt );
//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutCom( void )
{
	posRead		= 0;
	posWrite	= 0;
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
	APL_SOUND			*inAplSound			= getAplSound();
	DRV_UART_TX			outDrvUartTx;

	if( inAplSound->waterOk ){
		sendLen[posWrite] = createCmd( sendBuf[posWrite]	, FILE_NAME_WATER_READY );
	}

	if( inAplSound->vtec ){ 
		sendLen[posWrite] = createCmd( sendBuf[posWrite]	, FILE_NAME_VTEC );
	}
	
	APL_DATA_SW*		inAplDataSw = getAplDataSw();
	if( ( posRead != posWrite ) &&							// 未送信データがある
		( ! inAplDataSw->mk156Busy )						// MK156受付可
	){
		// setDrvUartTxの使用に合わせて値を作る。仕様変更したい
		outDrvUartTx.commandNum		= 1;
		outDrvUartTx.length[0]		= sendLen[posRead];
		memcpy( &outDrvUartTx.txCommand[0][0] , sendBuf[posRead]	, sendLen[posRead] );

		setDrvUartTx( UART_0_MK156 , &outDrvUartTx	);

		posRead = (posRead >= RING_BUF_SIZE)? 0 : posRead+1;
	}
}

static uint8_t createCmd( uint8_t* out , uint8_t fileNo )
{
	uint8_t			byteCnt		= 0;

	out[byteCnt++]	= START_BYTE;
	out[byteCnt++]	= VERSION_INFO;
	out[byteCnt++]	= COMMAND_LEN;
	out[byteCnt++]	= COMMAND_PLAY_TRACK;
	out[byteCnt++]	= ACK_OFF;
	out[byteCnt++]	= fileNo>>8;
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

static void SoundPlay( DRV_UART_TX* out , uint8_t* commandCnt )
{
	uint8_t			byteCnt		= 0;

	out->length[*commandCnt]				= 4;
	out->txCommand[*commandCnt][byteCnt++]	= START_BYTE;
	out->txCommand[*commandCnt][byteCnt++]	= 2;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND_PLAY_TRACK;
	out->txCommand[*commandCnt][byteCnt++]	= END_BYTE;

	commandCnt++;

}