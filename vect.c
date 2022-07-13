#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

#include "main.h"
#include "drvUart.h"
#include "drvInSw.h"
#include "drvEep.h"
#include "vect_inc.h"

static void debugVect( void );

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

//********************************************************************************
// All vect
// 全割り込み。使用時はここより上に記載し、下の一覧をコメントアウトする
//********************************************************************************
/* CRCSCAN interrupt vectors */
ISR(CRCSCAN_NMI_vect)	{	debugVect();	}

/* BOD interrupt vectors */
ISR(BOD_VLM_vect)		{	debugVect();	}

/* RTC interrupt vectors */
ISR(RTC_CNT_vect)		{	debugVect();	}
ISR(RTC_PIT_vect)		{	debugVect();	}

/* CCL interrupt vectors */
ISR(CCL_CCL_vect)		{	debugVect();	}

/* PORTA interrupt vectors */
ISR(PORTA_PORT_vect)	{	debugVect();	}

/* TCA0 interrupt vectors */
//ISR(TCA0_LUNF_vect)		{	debugVect();	}	//  _VECTOR(7)

//ISR(TCA0_OVF_vect)		{	debugVect();	}	//	_VECTOR(7)
ISR(TCA0_HUNF_vect)		{	debugVect();	}
ISR(TCA0_CMP0_vect)		{	debugVect();	}	//  _VECTOR(9)
//ISR(TCA0_LCMP0_vect)	{	debugVect();	}	//  _VECTOR(9)
ISR(TCA0_CMP1_vect)		{	debugVect();	}	//  _VECTOR(10)
//ISR(TCA0_LCMP1_vect)	{	debugVect();	}	//  _VECTOR(10)
ISR(TCA0_CMP2_vect)		{	debugVect();	}	//  _VECTOR(11)
//ISR(TCA0_LCMP2_vect)	{	debugVect();	}	//  _VECTOR(11)

/* TCB0 interrupt vectors */
ISR(TCB0_INT_vect)		{	debugVect();	}
/* TCB1 interrupt vectors */
ISR(TCB1_INT_vect)		{	debugVect();	}
/* TWI0 interrupt vectors */
ISR(TWI0_TWIS_vect)		{	debugVect();	}
ISR(TWI0_TWIM_vect)		{	debugVect();	}
/* SPI0 interrupt vectors */
ISR(SPI0_INT_vect)		{	debugVect();	}

/* USART0 interrupt vectors */
//ISR(USART0_RXC_vect)	{	debugVect();	}
//ISR(USART0_DRE_vect)	{	debugVect();	}
//ISR(USART0_TXC_vect)	{	debugVect();	}

/* USART1 interrupt vectors */
//ISR(USART1_RXC_vect)	{	debugVect();	}
//ISR(USART1_DRE_vect)	{	debugVect();	}
//ISR(USART1_TXC_vect)	{	debugVect();	}

/* USART2 interrupt vectors */
ISR(USART2_RXC_vect)	{	debugVect();	}
ISR(USART2_DRE_vect)	{	debugVect();	}
ISR(USART2_TXC_vect)	{	debugVect();	}
/* PORTD interrupt vectors */
//ISR(PORTD_PORT_vect)	{	debugVect();	}
/* AC0 interrupt vectors */
ISR(AC0_AC_vect)		{	debugVect();	}
/* ADC0 interrupt vectors */
ISR(ADC0_RESRDY_vect)	{	debugVect();	}
ISR(ADC0_WCOMP_vect)	{	debugVect();	}

/* PORTC interrupt vectors */
ISR(PORTC_PORT_vect)	{	debugVect();	}
/* TCB2 interrupt vectors */
ISR(TCB2_INT_vect)		{	debugVect();	}
/* PORTF interrupt vectors */
ISR(PORTF_PORT_vect)	{	debugVect();	}

/* NVMCTRL interrupt vectors */
//ISR(NVMCTRL_EE_vect)	{	debugVect();	}
/* PORTB interrupt vectors */
ISR(PORTB_PORT_vect)	{	debugVect();	}
/* PORTE interrupt vectors */
ISR(PORTE_PORT_vect)	{	debugVect();	}

static void debugVect( void )
{
	volatile	bool	brakepoint = true;
}
