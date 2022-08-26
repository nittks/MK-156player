#ifndef LNK_OUT_COM_INC_H
#define LNK_OUT_COM_INC_H

#include <stdbool.h>

#define RING_BUF_SIZE			((uint8_t)10)
#define	UART_TX_ID	0x21
#define	UART_TX_LENGTH	4

#define	START_BYTE				((uint8_t)0x7E)
#define	VERSION_INFO			((uint8_t)0xFF)
#define COMMAND1_LEN			((uint8_t)0x02)
#define COMMAND2_LEN			((uint8_t)0x03)
#define COMMAND3_LEN			((uint8_t)0x04)
#define COMMAND_LEN				((uint8_t)0x06)
#define DEFAULT_FOLDER			((uint8_t)0x01)
#define	REPRESENTATIVE_NO		((uint8_t)0x03)
#define	ACK_OFF					((uint8_t)0x00)
#define	ACK_ON					((uint8_t)0x01)


#define	COMMAND_PLAY			((uint8_t)0x0D)
#define	COMMAND_STOP			((uint8_t)0x0E)
#define	COMMAND_PLAY_MODE		((uint8_t)0x33)
#define	COMMAND_PLAY_TRACK		((uint8_t)0x0F)

#define FOLDER_NO				((uint8_t)0x00)
#define FILE_NAME_WATER_READY	((uint8_t)0x01)
#define FILE_NAME_VTEC			((uint8_t)0x02)


#define	END_BYTE				((uint8_t)0xEF)

//************************************
#define  INDICATOR_NUM			((uint8_t)2)
typedef union{
	uint8_t		byte;
	struct{
		bool		waterTemp	: 1;
		bool		vtec		: 1;
	};
}INDICATOR_LIST;


/*
再生したい音声案

水温
	80℃付近まで上昇
	100℃超え
	110℃超え
	100℃まで低下

油温
	80℃付近まで上昇
	100℃超え
	110℃超え
	120℃超え
	110℃まで低下
	100℃まで低下
油圧
	高回転時の油圧低下検知


VTEC
	VTEC起動

*/

#endif
