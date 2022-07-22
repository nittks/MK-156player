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
		SoundPlaySelect( &outDrvUartTx , &commandCnt  ,FILE_NAME_WATER_READY );
		SoundPlay( &outDrvUartTx , &commandCnt );
	}

	if( inAplSound->vtec ){
		SoundPlaySelect( &outDrvUartTx , &commandCnt  ,FILE_NAME_VTEC );
	}
	
	if( commandCnt >= 1 ){
		outDrvUartTx.commandNum	= commandCnt;

		setDrvUartTx( UART_0_MK156 , &outDrvUartTx );
	}
}

static void SoundPlaySelect( DRV_UART_TX* out , uint8_t* commandCnt , uint8_t fileNo )
{
	uint8_t			byteCnt		= 0;

	out->txCommand[*commandCnt][byteCnt++]	= START_BYTE;
	out->txCommand[*commandCnt][byteCnt++]	= VERSION_INFO;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND_LEN;
	out->txCommand[*commandCnt][byteCnt++]	= COMMAND_PLAY_TRACK;
	out->txCommand[*commandCnt][byteCnt++]	= ACK_OFF;
	out->txCommand[*commandCnt][byteCnt++]	= fileNo>>8;
	out->txCommand[*commandCnt][byteCnt++]	= fileNo;

	uint16_t	sumCalc = 0;
	for(uint8_t i=0 ; i< COMMAND_LEN ; i++){
		sumCalc += out->txCommand[*commandCnt][i+1];	// START_BYTEは入らないのでversionから
	}
	sumCalc = 0xFFFF - sumCalc;
	sumCalc += 1;
	out->txCommand[*commandCnt][byteCnt++]	= (uint8_t)(( sumCalc >> 8 ) & 0x00FF);
	out->txCommand[*commandCnt][byteCnt++]	= (uint8_t)(( sumCalc      ) & 0x00FF);
	out->txCommand[*commandCnt][byteCnt++]	= END_BYTE;

	out->length[*commandCnt]				= byteCnt;
	(*commandCnt)++;
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