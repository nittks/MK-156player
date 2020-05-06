
typedef enum{
	TIMER_DRV_IN_UART_TIMEOUT,
	TIMER_NUM
}TIMER_ID;

typedef enum{
	TIMER_STOP,
	TIMER_START,
	TIMER_OVER_FLOW,
	TIMER_STATE_NUM
}TIMER_STATE;

extern void initTimer( void );
extern void timerMain( void );
extern void startTimer( TIMER_ID timerId );
extern void stopTimer( TIMER_ID timerId );
extern void clearTimer( TIMER_ID timerId );
extern unsigned char getTimerCnt( TIMER_ID timerId );
