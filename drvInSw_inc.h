
#define	ROT_ENC_DEBTIME	(1)	//1周期待ち(翌周期まで待つ→10+10x1ms
#define	PUSH_SW_DEBTIME		(60)	//デバウンス待ち時間
#define	PUSH_SW_LONGTIME	(100)	//長押し判定時間(10x100=1000ms

#define	PORT_ROT_ENC0_A		((PORTB & (1<<PB4))>> PB4)
#define	PORT_ROT_ENC0_B		((PORTB & (1<<PB5))>> PB5)

#define	PORT_PUSHSW_0		((PORTC & (1<<PC4))>> PC4)
#define	PORT_PUSHSW_1		((PORTC & (1<<PC5))>> PC5)


#define	ROT_VECT_FORWARD	((signed char)(4))
#define	ROT_VECT_REVERCE	((signed char)(-4))

static const signed char grayCodeTable[0xF+1] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

//ロータリーエンコーダー内部状態
typedef enum{
	ROT_ENC_STATE_WAIT,		//入力監視中
	ROT_ENC_STATE_FORWARD,	//正転検知
	ROT_ENC_STATE_REVERCE,	//逆転検知
	ROT_ENC_STATE_DEBOUNCE	//デバウンスタイムウェイト
}ROT_ENC_STATE;

//プッシュスイッチ内部状態
typedef enum{
	PUSH_SW_STATE_OFF,
	PUSH_SW_STATE_ON,
	PUSH_SW_STATE_DEBOUNCE	//デバウンスタイムウェイト
}PUSH_SW_STATE;

//ポート状態
typedef enum{
	PORT_OFF	= 0,
	PORT_ON		= 1
}SW_PORT_STATE;

typedef enum{
	NO_SET
}ROT_PORT_NO;

typedef enum{
	NO_0,
	NO_1
}PUSH_SW_PORT_NO;

#define		ROT_ENC_0_POS		(0x03)		//bit0,1より入力
#define		ROT_ENC_1_POS		(0x0C)		//bit2,3より入力

//ピン変化割り込みPCINT8-11を使用する
#define	REG_PCIE2	0		//PCINT16-23pinの割り込み許可設定
#define	REG_PCIE1	1		//PCINT08-14pinの割り込み許可設定
#define	REG_PCIE0	1		//PCINT00-07pinの割り込み許可設定


//ピン変化割り込み制御レジスタ
#define	SET_PCICR	(PCICR | (REG_PCIE2<<PCIE2) | (REG_PCIE1<<PCIE1) | (REG_PCIE0<<PCIE0))

//ピン変化割り込みマスク
#define	SET_PCMSK1	(PCMSK1 | 0x30)
#define	SET_PCMSK0	(PCMSK0 | 0x30)





