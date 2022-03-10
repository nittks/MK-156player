#ifndef LNK_IN_COM_INC_H
#define LNK_IN_COM_INC_H

#include "hardware.h"

typedef enum{
	STATE_DOWN,
	STATE_UP
}STATE_CNT;

#define	CONTROL_NORMAL_OPERATION	('1')
enum{
	MIN,
	MAX,
	MINMAX,
};
const short SENSER_RANGE[ID_MAX][MINMAX] = {
	{ -1	, 2		},		// é¿ëïéûóvåüì¢
	{  0	, 9000	},
	{  0	, 10	},
	{  0	, 6		},
	{  2	, 11	},
	{ 50	, 150	},
	{ 20	, 120	}		//êÖâ∑Å@ç°âÒÇÕÇ±ÇÍÇæÇØ
};


#define	ANGLE_VAL_MIN	((uint16_t)0)
#define	ANGLE_VAL_MAX	((uint16_t)0x930)


#endif
