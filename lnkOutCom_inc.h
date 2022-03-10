#ifndef LNK_OUT_COM_INC_H
#define LNK_OUT_COM_INC_H

#define	UART_TX_ID	0x21
#define	UART_TX_LENGTH	4

#define	START_BYTE				((uint8_t)0x7E)
#define COMMAND1_LEN			((uint8_t)0x02)
#define COMMAND2_LEN			((uint8_t)0x03)
#define COMMAND3_LEN			((uint8_t)0x04)


#define	COMMAND1_PLAY			((uint8_t)0x01)
#define	COMMAND1_STOP			((uint8_t)0x0E)
#define	COMMAND2_PLAY_MODE		((uint8_t)0x33)
#define	COMMAND3_SELECT_PLAY	((uint8_t)0x42)

#define FOLDER_NO				((uint8_t)0x00)
#define FILE_NAME_WATER_READY	((uint8_t)0x01)
#define FILE_NAME_VTEC			((uint8_t)0x02)


#define	END_BYTE				((uint8_t)0xEF)



#endif
