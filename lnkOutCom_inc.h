#ifndef LNK_OUT_COM_INC_H
#define LNK_OUT_COM_INC_H

#define RING_BUF_SIZE			((uint8_t)10)
#define	UART_TX_ID	0x21
#define	UART_TX_LENGTH	4

#define	START_BYTE				((uint8_t)0x7E)
#define	VERSION_INFO			((uint8_t)0xFF)
#define COMMAND1_LEN			((uint8_t)0x02)
#define COMMAND2_LEN			((uint8_t)0x03)
#define COMMAND3_LEN			((uint8_t)0x04)
#define COMMAND_LEN				((uint8_t)0x06)

#define	REPRESENTATIVE_NO		((uint8_t)0x03)
#define	ACK_OFF					((uint8_t)0x00)
#define	ACK_ON					((uint8_t)0x01)


#define	COMMAND_PLAY			((uint8_t)0x0D)
#define	COMMAND_STOP			((uint8_t)0x0E)
#define	COMMAND_PLAY_MODE		((uint8_t)0x33)
#define	COMMAND_PLAY_TRACK		((uint8_t)0x03)

#define FOLDER_NO				((uint8_t)0x00)
#define FILE_NAME_WATER_READY	((uint8_t)0x01)
#define FILE_NAME_VTEC			((uint8_t)0x02)


#define	END_BYTE				((uint8_t)0xEF)



#endif
