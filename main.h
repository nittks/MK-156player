#ifndef MAIN_H
#define MAIN_H

#define F_CPU	((uint32_t)20000000)		// delay.hを使用する場合は、先にmain.hをincludeする
#define F_PDIV	((uint8_t)1)
	// ↑SerialLedの信号幅の400nsをタイマで作るため、カウント数が整数になる分周値で最も低速になる倍率を選択。

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
