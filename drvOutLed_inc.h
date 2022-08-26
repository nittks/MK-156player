#ifndef DRV_OUT_LED_INC_H
#define DRV_OUT_LED_INC_H

#define LED_BRIGHT_MAX	(100)

/*
	 PWMのTOP値は、120-240FPS相当にしたい。8-4ms
	 20MHzCLKを前置分周8分周と4分周に通すと、周期1.6us
	 8ms=5000cnt , 4ms=2500cnt
*/
#define PWM_TOP			(2500)
//------------------------------------------------------------
#define LED_ON	(1)
#define LED_OFF	(0)

#define LED_PORT	(PORTF)
#define BP_GREEN	(2)
#define BP_BLUE		(3)
#define	SET_LED_GREEN( data )	(LED_PORT.OUT = ((LED_PORT.OUT & (~(1<<BP_GREEN))) | (data<<BP_GREEN)))
#define	SET_LED_BLUE ( data )	(LED_PORT.OUT = ((LED_PORT.OUT & (~(1<<BP_BLUE ))) | (data<<BP_BLUE )))



#endif