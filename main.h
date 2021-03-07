#ifndef MAIN_H
#define MAIN_H

#define FOSC	((unsigned long)16000000)

enum{
	TASK_DRV_IN_MAIN,
	TASK_LINK_IN,
	TASK_APL,
	TASK_LINK_OUT,
	TASK_DRV_OUT,
	TASK_POWER_LED,
	TASK_UART_CHANGE_TX,
	TASK_MAX
};

extern void interTaskTime( void );
extern void initMain( void );
extern void enableTask( unsigned char taskNo );
extern void disableTask( unsigned char taskNo );

#endif
