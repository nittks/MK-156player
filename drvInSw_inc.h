#ifndef DRV_IN_SW_INC_H
#define DRV_IN_SW_INC_H

#define	ROT_ENC_DEBTIME	(1)	//1周期待ち(翌周期まで待つ→10+10x1ms
#define	PUSH_SW_DEBTIME		(60)	//デバウンス待ち時間
#define	PUSH_SW_LONGTIME	(100)	//長押し判定時間(10x100=1000ms

#define	PORT_ROT_ENC0_A		((PORTD.IN & >> PIN0_bp)& 0x01)
#define	PORT_ROT_ENC0_B		((PORTD.IN & >> PIN1_bp)& 0x01)

#define	PORT_PUSHSW_0		((~(PORTD.IN & >> PIN2_bp)) & 0x01)		//Lowアクティブ反転

//時間設定。なんとなく。
#define	PUSH_SW_DEBOUNCE_10MS		((uint8_t)( 100/10))	//  100ms/10[ms/cyc]
#define	PUSH_SW_LONGON_10MS			((uint8_t)(1000/10))	// 1000ms/10[ms/cyc]
#define	PUSH_SW_DOUBLE_TIMEOUT_10MS	((uint8_t)( 500/10))	//  500ms/10[ms/cyc]


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
	PUSH_SW_STATE_WAIT_DOUBLE,
	PUSH_SW_STATE_DOUBLE,
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

typedef struct{
	uint8_t			portIn;
	uint8_t			portInLatch;
	PUSH_SW_STATE	state	;
	uint8_t			cntTimeDebounce;
	uint8_t			cntTimeJudge;
}PORT_PUSH_SW;

#endif
