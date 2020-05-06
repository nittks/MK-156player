
#define CNTMAX		((unsigned char)10)	//8bit
#define CNT100MSMAX	((unsigned char)20)	//5ms割り込み20回で100ms

#define DOT_ON( inData )	( inData|0x01)		//7セグの点(dot)を表示する

enum{
	LED_7SEG_0,
	LED_7SEG_1,
	LED_7SEG_2,
	LED_7SEG_3,
	LED_7SEG_4,
	LED_7SEG_5,
	LED_7SEG_6,
	LED_7SEG_7,
	LED_7SEG_8,
	LED_7SEG_9,
	LED_7SEG_BLANK,
	LED_7SEG_MAX
};

//HUD反射表示
static const unsigned char	led7SegBit[LED_7SEG_MAX]={
//上位=a,下位=.
	0b11111100,
	0b01100000,
	0b10110110,
	0b11110010,
	0b01101010,
	0b11011010,
	0b11011110,
	0b01110000,
	0b11111110,
	0b11111010,
	0b00000000,
};

/*
//通常表示
static const unsigned char	led7SegBit[LED_7SEG_MAX]={
	//上位=a,下位=.
	0b11111100,
	0b01100000,
	0b11011010,
	0b11110010,
	0b01100110,
	0b10110110,
	0b10111110,
	0b11100000,
	0b11111110,
	0b11110110,
	0b00000000,
};
*/