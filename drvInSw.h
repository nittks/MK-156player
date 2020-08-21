#ifndef DRV_IN_SW_H
#define DRV_IN_SW_H

#define	ROT_ENC_NUM	1		//ロータリーエンコーダー数
#define	PUSH_SW_NUM	1		//プッシュスイッチ数

//ロータリーエンコーダー状態
typedef enum{
	DRV_IN_ROT_ENC_STATE_STOP,	//停止、変化なし
	DRV_IN_ROT_ENC_STATE_FORWARD,	//正転
	DRV_IN_ROT_ENC_STATE_REVERCE	//逆転
}DRV_IN_ROT_ENC_STATE;

//プッシュスイッチ状態
typedef enum{
	DRV_IN_PUSH_SW_STATE_OFF,
	DRV_IN_PUSH_SW_STATE_ON,
	DRV_IN_PUSH_SW_STATE_LONGON,
}DRV_IN_PUSH_SW_STATE;

typedef struct{
	DRV_IN_ROT_ENC_STATE	rotEncState[ROT_ENC_NUM];
	DRV_IN_PUSH_SW_STATE	pushSwState[PUSH_SW_NUM];
}DRV_IN_SW;

extern void initDrvInSw( void );
extern DRV_IN_SW *getDrvInSw( void );
extern void drvInSwMain( void );
extern void interSwInput( void );
extern void interPcInt00_07( void );
extern void interPcInt08_14( void );

#endif
