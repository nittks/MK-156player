#include <avr/interrupt.h>
#include <avr/io.h>

#include "main.h"
#include "drvUart.h"
#include "drvInSw.h"
#include "drvEep.h"
#include "vect_inc.h"

ISR(TCA0_OVF_vect)	//タイマ割り込み
{
	interTaskTime();
}
ISR(USART0_RXC_vect)		//UART受信割込み
{
	interGetUartRxData0();
}

ISR(USART1_RXC_vect)		//UART受信割込み
{
	interGetUartRxData1();
}

ISR(USART0_DRE_vect)
{
	interSetUartTxData0();
}
ISR(USART1_DRE_vect)
{
	interSetUartTxData1();
}

ISR(PORTD_PORT_vect){		//ポート変化割り込み
	interPortD();
}
ISR(NVMCTRL_EE_vect){		//EEPROMアクセス可能割込み
	interEepRedy();
}
