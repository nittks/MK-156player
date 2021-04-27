#ifndef DRV_OUT_SERIAL_LED_INC_H
#define DRV_OUT_SERIAL_LED_INC_H


#define LED_STARTUP_TIME	(1000/10)	// 1sec/1周期	, 1sec:根拠無し
// WS2812B parameters
#define T0H_NS      400
#define T1H_NS		800
#define T_WIDTH_NS  1250
#define T_RESET_US  50
#define CNTMAX		((unsigned char)10)	//8bit
#define CNT100MSMAX	((unsigned char)20)	//5ms割り込み20回で100ms

//bitPos 0-7
//********************************************************************************
// シフトレジスタ
//********************************************************************************
#define	CPLD_SEND_DATA_NUM	6


//出力bitポジション
#define	SRCLR_POS	3
#define	SRCLK_POS	4
#define	RCLK_POS	5
#define	OE_POS		1
#define	SER_POS		2

//カソード側各ポート
#define	SERIAL_LED_PORT	PORTC.IN
#define	SRCLR_ON	(SERIAL_LED_PORT &= ~(1<<SRCLR_POS))	//Lowアクティブ
#define	SRCLR_OFF	(SERIAL_LED_PORT |=  (1<<SRCLR_POS))
#define	SRCLK_ON	(SERIAL_LED_PORT |=  (1<<SRCLK_POS))
#define	SRCLK_OFF	(SERIAL_LED_PORT &= ~(1<<SRCLK_POS))
#define	RCLK_ON		(SERIAL_LED_PORT |=  (1<<RCLK_POS))
#define	RCLK_OFF	(SERIAL_LED_PORT &= ~(1<<RCLK_POS))
#define	OE_ON		(SERIAL_LED_PORT &= ~(1<<OE_POS))	//Lowアクティブ
#define	OE_OFF		(SERIAL_LED_PORT |=  (1<<OE_POS))
#define	SER_ON		(SERIAL_LED_PORT |=  (1<<SER_POS))
#define	SER_OFF		(SERIAL_LED_PORT &= ~(1<<SER_POS))
#define	SER_DATA_NUM	((unsigned char)18)		//1フレームでシフトレジスタへセットするデータbit数
#define	SER_SET( data )	(SERIAL_LED_PORT = ((SERIAL_LED_PORT & (~(1<<SER_POS))) | ((data&0x01)<<SER_POS)))
												//SERIAL_LED_PORT-SER_POS bitにdataをセットする
//アノード側
#define	ANODE_PORT	PORTC
#define	ANODE_PORT_MASK		0xF0		//PORTC上位は入力、内部プルアップのため1設定を維持
#define	ANODE_POS	0
#define	ANODE_SET( data )	(ANODE_PORT = (ANODE_PORT_MASK | (data << ANODE_POS)))
#define	ALL_OFF				(ANODE_PORT &= (ANODE_PORT_MASK & 0xF0))		//アノード側をLowにする(PB4-7

#define	WAIT_SREG	(_delay_us( 1 ))	//信号変化後のウェイト


//********************************************************************************
// PWM
//********************************************************************************
//共通
#define	TOP				0xFF	//TOP値=255

//7セグ用PWM設定 タイマ1B
#define	REG_WGM1		0x5		//8bit高速PWM動作
#define	REG_COM1B		0x2		//非反転動作
#define	REG_CS1			0x3		//8分周(8MHz/8=1Mhz
#define	REG_ICR1		TOP
#define	OCR1B_MAX		TOP

#define	SET_COM1B			(TCCR1A |= (REG_COM1B&0x03)<<COM1B0)
#define	SET_WGM1			(TCCR1A |= (REG_WGM1&0x03)<<WGM10);(TCCR1B |= ((REG_WGM1>>2)<<WGM12))

#define START_PWM1			(TCCR1B|=  (REG_CS1<<CS10))
#define STOPT_PWM1			(TCCR1B&= ~(REG_CS1<<CS10))

#define SET_ICR1			(ICR1H |= REG_ICR1>>8);(ICR1L = REG_ICR1&0x0F)
#define	SET_OCR1B( data )	(OCR1B = (((unsigned short)data*OCR1B_MAX)/100))


//バーLED用PWM設定 タイマ2A
#define	REG_WGM2		0x3		//8bit高速PWM動作
#define	REG_COM2A		0x2		//非反転動作
#define	REG_CS2			0x3		//8分周(8MHz/8=1Mhz
#define REG_ICR2		TOP
#define	OCR2A_MAX		TOP

#define	SET_COM2A		(TCCR2A |= (REG_COM2A&0x03)<<COM2A0)
#define	SET_WGM2		(TCCR2A |= (REG_WGM2&0x03)<<WGM20);(TCCR2B = ((REG_WGM2>>2)<<WGM22))

#define START_PWM2		(TCCR2B|=  (REG_CS2<<CS20))
#define STOPT_PWM2		(TCCR2B&= ~(REG_CS2<<CS20))

//#define SET_ICR2		((ICR2 = REG_ICR1>>8);(ICR1L = REG_ICR1&0x0F))
#define	SET_OCR2A( data )	(OCR2A = (((unsigned long)data*OCR2A_MAX)/100))

#endif
