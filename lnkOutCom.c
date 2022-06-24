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

static void SoundPlaySelect( DRV_UART_TX* out , uint8_t* commandCnt , uint8_t fileNo );
static void SoundPlay( DRV_UART_TX* out , uint8_t* commandCnt );
//********************************************************************************
// 初期化
//********************************************************************************
void initLnkOutCom( void )
{
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

	uint8_t			commandCnt	= 0;
	if( inAplSound->waterOk ){
//		SoundPlaySelect( &outDrvUartTx , &commandCnt  ,FILE_NAME_WATER_READY );
		SoundPlay( &outDrvUartTx , &commandCnt );
	}

	if( inAplSound->vtec ){
		SoundPlaySelect( &outDrvUartTx , &commandCnt  ,FILE_NAME_VTEC );
	}
	
	outDrvUartTx.commandNum	= commandCnt;

	setDrvUartTx( UART_0_MK156 , &outDrvUartTx );
}

static void SoundPlaySelect( DRV_UART_TX* out , uint8_t* commandCnt , uint8_t fileNo )
{
	uint8_t			byteCnt		= 0;

	out->commandLen[*commandCnt]			= 6;
	out->txCommand[*commandCnt][byteCnt++]	= START_BYTE;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND3_LEN;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND3_SELECT_PLAY;
	out->txCommand[*commandCnt][byteCnt++]	= FOLDER_NO;
	out->txCommand[*commandCnt][byteCnt++]	= fileNo;
	out->txCommand[*commandCnt][byteCnt++]	= END_BYTE;
}

static void SoundPlay( DRV_UART_TX* out , uint8_t* commandCnt )
{
	uint8_t			byteCnt		= 0;

	out->commandLen[*commandCnt]			= 4;
	out->txCommand[*commandCnt][byteCnt++]	= START_BYTE;
	out->txCommand[*commandCnt][byteCnt++]	= 2;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND1_PLAY;
	out->txCommand[*commandCnt][byteCnt++]	= END_BYTE;
}