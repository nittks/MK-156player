#ifndef LNK_IN_COM_INC_H
#define LNK_IN_COM_INC_H

#include "hardware.h"

typedef enum{
	STATE_DOWN,
	STATE_UP
}STATE_CNT;

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
