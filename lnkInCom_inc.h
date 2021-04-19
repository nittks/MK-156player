#ifndef LNK_IN_COM_INC_H
#define LNK_IN_COM_INC_H

#include "hardware.h"

typedef enum{
	STATE_DOWN,
	STATE_UP
}STATE_CNT;

#define	SPEED_MAX	((unsigned char	)200)
#define	SPEED_MIN	((unsigned char	)0)
#define	SPEED_INIT	((unsigned char	)321)
#define	REV_MAX		((unsigned short)8000)
#define	REV_MIN		((unsigned short)0)
#define	REV_INIT	((unsigned short)2400)

#define	UART_PALSE_SET_SPEED_MIN		1
#define	UART_PALSE_SET_SPEED_MAX		5
#define	UART_PALSE_SET_SPEED_DEFAULT	1
#define	UART_PALSE_SET_REV_MIN			1
#define	UART_PALSE_SET_REV_MAX			9
#define	UART_PALSE_SET_REV_DEFAULT		4

enum{
	UART_NO_ID,
	UART_NO_LEN,
	UART_NO_SPEED,
	UART_NO_REV0,
	UART_NO_REV1,
	UART_NO_PALSE_SET,
	UART_NO_CAR_SIG,
	UART_NO_SUM,
	UART_NO_MAX
};

enum{
	POS_IG,
	POS_ACC,
	POS_ILL,
	POS_VTC,
};

#endif
