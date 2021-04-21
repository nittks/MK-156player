#include <avr/interrupt.h>
#include <avr/io.h>
#include "vect_inc.h"
#include "main.h"
#include "drvUart.h"
#include "drvInSw.h"
#include "drvEep.h"
#include "drvOutSerialLed.h"


ISR(TCA0_OVF_vect)	//タイマ割り込み
{
	interTaskTime();
}
ISR(USART0_RXC_vect)		//UART受信割込み
{
	interGetUartRxData();
}
ISR(USART0_DRE_vect)	//UART DataRegisterEmpty送信レジスタ空割込み
{
	interSetTxBuffer();
}
ISR(USART0_TXC_vect)
{
	interChangeNextCCLPort();
}

ISR(PORTD_PORT_vect){		//ポート変化割り込み
	interPortD();
}
ISR(NVMCTRL_EE_vect){		//EEPROMアクセス可能割込み
	interEepRedy();
}
