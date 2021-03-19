#ifndef LNK_OUT_LED_INC_H
#define LNK_OUT_LED_INC_H

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

//7セグ、セグメントパターン
// 結線順序 3,4,5,0,1,2,6
static const unsigned char	led7SegBit[LED_7SEG_MAX]={
//上位=a,下位=.
	0b11100111,
	0b00100010,
	0b11001011,
	0b01101011,
	0b00101110,
	0b01101101,
	0b11101101,
	0b00100011,
	0b11101111,
	0b00101111,
	0b00000000,
};

#endif
