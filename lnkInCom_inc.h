#ifndef LNK_IN_COM_INC_H
#define LNK_IN_COM_INC_H

#include "hardware.h"

typedef enum{
	STATE_DOWN,
	STATE_UP
}STATE_CNT;

//*******************************************************
// MK-156
//*******************************************************
#define MK156_MAX			10
#define HEADER_BYTE_NUM		6
typedef struct{
	uint8_t	len;
	uint8_t	cmd;
	uint8_t	feedback[MK156_MAX-HEADER_BYTE_NUM];
	uint8_t	sum[2];
}MK156_FRAME;

// データ位置
enum{
	MK156_FORMAT00_STARTBYTE	= 0,
	MK156_FORMAT01_VERSION		= 1,
	MK156_FORMAT02_LENGTH		= 2,
	MK156_FORMAT03_COMMAND		= 3,
	MK156_FORMAT04_FEEDBACK		= 4,
	MK156_FORMAT07_SUM			= 7,
	MK156_FORMAT09_ENDBYTE		= 9,
};

#define	MK156_START_BYTE		0x7E
#define MK156_VERSION			0xFF
#define MK156_END_BYTE			0xEF

#define MK156_CMD_SD_PLAY_FIN	0x3D


//*******************************************************
// Defi
//*******************************************************
enum{
	DEFI_ID,
	DEFI_CTRL,
	DEFI_ANGLE_0,
	DEFI_ANGLE_1,
	DEFI_ANGLE_2,
	DEFI_MAX,
};
#define BUF_FRAME_SIZE		16
#define	CONTROL_NORMAL_OPERATION	(0x31)
enum{
	MIN,
	MAX,
	MINMAX,
};
enum{
	NO_TURBO		,
	NO_TACHO		,
	NO_OIL_PRESS	,
	NO_FUEL_PRESS	,
	NO_EXT_TEMP		,
	NO_OIL_TEMP		,
	NO_WATER_TEMP	,
	NO_MAX			
};
const short SENSER_RANGE[ID_MAX][MINMAX] = {
	{ -1	, 2		},		// 実装時要検討(ブースト計。負圧正圧の取り扱い)
	{  0	, 9000	},
	{  0	, 10	},
	{  0	, 6		},
	{  2	, 11	},
	{ 50	, 150	},
	{ 20	, 120	}		//水温　今回はこれだけ
};


#define	ANGLE_VAL_MIN	((uint16_t)0)
#define	ANGLE_VAL_MAX	((uint16_t)0x930)
#define ANGLE_DATA_ZERO	((uint8_t )0x30)

#define ASCII_NUM		((uint8_t)16)
enum{
	ASCII_CODE,
	HEX_VAL,
	ASCII_TABLE_MAX,
};
const uint8_t ASCII_TABLE[ASCII_NUM][ASCII_TABLE_MAX] = {
	{ 0x30	, 0x0 },
	{ 0x31	, 0x1 },
	{ 0x32	, 0x2 },
	{ 0x33	, 0x3 },
	{ 0x34	, 0x4 },
	{ 0x35	, 0x5 },
	{ 0x36	, 0x6 },
	{ 0x37	, 0x7 },
	{ 0x38	, 0x8 },
	{ 0x39	, 0x9 },
	{ 0x41	, 0xA },
	{ 0x42	, 0xB },
	{ 0x43	, 0xC },
	{ 0x44	, 0xD },
	{ 0x45	, 0xE },
	{ 0x46	, 0xF }
};



#endif
