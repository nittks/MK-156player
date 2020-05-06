
#include "timer.h"
#include "timer_inc.h"

static unsigned char timerCnt[TIMER_NUM];
static TIMER_STATE timerState[TIMER_NUM];

//********************************************************************************
//初期化
//********************************************************************************
void initTimer( void )
{
	unsigned char i;
	for(i=0 ; i<TIMER_NUM ; i++){
		timerCnt[i] = 0;
		timerState[i] = TIMER_STOP;
	}
}
//********************************************************************************
//メイン処理
//********************************************************************************
void timerMain( void )
{
	unsigned char i;

	for(i=0 ; i<TIMER_NUM ; i++){
		if(timerState[i] == TIMER_START ){
			if(timerCnt[i]<=0xFF){
				timerCnt[i]++;
			}else{
				timerState[i] = TIMER_OVER_FLOW;
			}
		}
	}
}

//********************************************************************************
// タイマスタート
//********************************************************************************
void startTimer( TIMER_ID timerId )
{
	if( timerId < TIMER_NUM ){
		timerState[timerId]	= TIMER_START;
		timerCnt[timerId]	= 0;
	}
}
//********************************************************************************
// タイマストップ
//********************************************************************************
void stopTimer( TIMER_ID timerId )
{
	if( timerId < TIMER_NUM ){
		timerState[timerId]	= TIMER_STOP;
	}
}
//********************************************************************************
// タイマクリア
//********************************************************************************
void clearTimer( TIMER_ID timerId )
{
	if( timerId < TIMER_NUM ){
		timerState[timerId]	= TIMER_STOP;
		timerCnt[timerId]	= 0;
	}
}
//********************************************************************************
// タイマ値取得
//********************************************************************************
unsigned char getTimerCnt( TIMER_ID timerId )
{
	if( timerId < TIMER_NUM ){
		return (timerCnt[timerId]);
	}
	return(0);
}

