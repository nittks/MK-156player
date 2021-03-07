#include <avr/interrupt.h>
#include <avr/io.h>
#include "vect_inc.h"
#include "main.h"
#include "drvUart.h"
#include "drvInSw.h"
#include "drvEep.h"


ISR(TCA0_OVF_LUNF)	//タイマ割り込み
{
	interTaskTime();
}
ISR(USART_RX_vect)		//UART受信割込み
{
	interGetUartRxData();
}
ISR(USART_UDRE_vect)	//UART DataRegisterEmpty送信レジスタ空割込み
{
	interSetUartTxData();
}
ISR(PCINT0_vect){		//ポート変化割り込み
	interPcInt00_07();
}
ISR(PCINT1_vect){		//ポート変化割り込み
	interPcInt08_14();
}
ISR(EE_READY_vect){		//EEPROMアクセス可能割込み
	interEepRedy();
}
