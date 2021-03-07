
#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdbool.h>

#include "drvEep_inc.h"
#include "drvEep.h"
#include "hardware.h"

//出力データ
static DRV_EEP_READ		drvEepRead;
static DRV_EEP_WRITE	drvEepWrite;
static unsigned char	eepAddrCnt;

static unsigned char	eepCache[DRV_EEP_MAP_MAX];
static EEP_STATE		state;

static void eepRead( void );
static void eepWrite( void );
//********************************************************************************//
// 初期化
//********************************************************************************//
void initDrvEep( void )
{
	unsigned char		i;

	drvEepRead.readState = true;
	for( i=0; i<DRV_EEP_MAP_MAX ; i++ ){
		drvEepRead.val[i] = 50;
	}

	drvEepRead.readState = DRV_EEP_READ_STATE_UNREAD;	//未読み込み
	state = EEP_STATE_BOOT;
	//起動時読込
	eepAddrCnt = 0;
	eepRead();

}

//********************************************************************************//
// データ取得
//********************************************************************************//
DRV_EEP_READ *getDrvEep( void )
{
	return( &drvEepRead );
}
//********************************************************************************//
// データ書込み
//********************************************************************************//
unsigned char setDrvEep( DRV_EEP_WRITE *inP )
{
	unsigned char		ret;

	if( state == EEP_STATE_READY ){
		state = EEP_STATE_WRITE;
		memcpy( drvEepWrite.val , inP->val , DRV_EEP_MAP_MAX );
		eepAddrCnt=0;
		eepWrite();

		ret = true;
	}else{
		ret = false;
	}

	return( ret );
}
//********************************************************************************//
// EEPROMアクセス可能割り込み
//********************************************************************************//
void interEepRedy( void )
{
	if( state == EEP_STATE_READ ){		//読み込み途中
		eepRead();
	}else if( state == EEP_STATE_WRITE ){	//書込み途中
		eepWrite();
	}
}
//********************************************************************************//
// 読み込み
//********************************************************************************//
static void eepRead( void )
{
	//EEPROMアクセス可能な間、読込続ける。アクセス不可でbreakする
	while( 1 ){
		if( EEPROM_IS_READY == EEP_READY_FAIL ){
			//EEPROMアクセス不可の場合抜ける
			state = EEP_STATE_READ;
			INT_EEP_ENABLE;		//EEPROMアクセス可能割込み許可
			break;
		}else{
			eepCache[ eepAddrCnt ] =  eeprom_read_byte( (uint8_t *)eepAddrCnt );
			if( eepAddrCnt < DRV_EEP_MAP_MAX ){
				eepAddrCnt++;
			}else{
				//全データ読み込み完
				state = EEP_STATE_READY;
				drvEepRead.readState = DRV_EEP_READ_STATE_READED;
				INT_EEP_DISABLE;		//EEPROMアクセス可能割込み禁止
				memcpy( drvEepRead.val ,eepCache , DRV_EEP_MAP_MAX );	//LNK読込用変数へコピー

				break;
			}
		}
	}
}
//********************************************************************************//
// 書込み
//********************************************************************************//
static void eepWrite( void )
{
	//EEPROMアクセス可能な間、読込続ける。アクセス不可でbreakする
	while( 1 ){
		if( EEPROM_IS_READY == EEP_READY_FAIL ){
			//EEPROMアクセス不可の場合抜ける
			state = EEP_STATE_WRITE;
			INT_EEP_ENABLE;		//EEPROMアクセス可能割込み許可
			break;
		}else{
			//値が異なっていたら書込む
			if( drvEepWrite.val[ eepAddrCnt ] != eepCache[ eepAddrCnt ] ){
				eeprom_write_byte( (uint8_t*)eepAddrCnt ,drvEepWrite.val[eepAddrCnt] );
				eepCache[ eepAddrCnt ] =  drvEepWrite.val[ eepAddrCnt ];
			}
			
			eepAddrCnt++;
			if( eepAddrCnt >= DRV_EEP_MAP_MAX ){
				//全データ書込み完
				state = EEP_STATE_READY;
				INT_EEP_DISABLE;		//EEPROMアクセス可能割込み禁止
				break;
			}
		}
	}
}
