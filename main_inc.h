#ifndef MAIN_INC_H
#define MAIN_INC_H

typedef struct{
	unsigned char	regist;			//登録状態。タスクが有効か無効か
	unsigned short	currentTime;	//現在時間。0になったらタスク起動
	unsigned short	cycleTime;		//タスク周期。
	void			(*func)(void);
}TASK_PARAMETER;


static void powerLed( void );
#define	TASK_NUM	7
TASK_PARAMETER	taskParameter[TASK_NUM]	={
	//現在時間(開始時オフセット) , 周期 , 関数名
	{	true,	0,	10,	drvInMain	},
	{	true,	1,	10,	lnkInMain	},
	{	true,	2,	10,	aplMain		},
	{	true,	3,	10,	lnkOutMain	},
	{	true,	4,	4,	drvOutMain	},
	{	true,	5,	250,powerLed	},
	{	false,	0,	2,	drvUartChangeTx}		//UARTドライバからの要求により有効化する
};

//CS0 クロック選択0
enum{
	CS0_STOP	= 0,
	CS0_DIV_NO,
	CS0_DIV_8,
	CS0_DIV_64,
	CS0_DIV_256,
	CS0_DIV_1024,
	CS0_DIV_TODOWN,
	CS0_DIV_T0UP
};


//WGM0 波形生成種別
enum{
	WGM_NORMAL	=0,
	WGM_8BIT_PHASE_BASE_PWM,
	WGM_COMP_CTC,
	WGM_8BIT_HIGHT_SPEED_PWM,
	WGM_RESERVE,
	WGM_PHASE_BASE_PWM,
	WGM_RESERVE1,
	WGM_HIGHT_SPEED_PWM
};

//COM0A 比較A出力選択
enum{
	COM0A_NORMAL	= 0,
	COM0A_COMP_TOGLE,
	COM0A_COMP_LOW,
	COM0A_COMP_HIGH
};

//レジスタ設定
#define		REG_CS0		(CS0_DIV_64 & 0x07)	//3bit
#define		REG_WGM		(WGM_COMP_CTC & 0x07)		//3bit
#define		REG_COM0A	(COM0A_NORMAL & 0x03)	//2bit
#define		TIMER_TOP	125		//8us*125=1ms,1ms毎割り込み
#define		REG_OCIE0A	1		//比較一致割込み許可
#define		REG_CAL		171		//内臓クロック校正(デフォルト171)


//レジスタセット用
#define		SET_TCCR0A	(( REG_COM0A << COM0A0 ) | ((REG_WGM & 0x03) << WGM00))	//WGM00,01のみ
#define		SET_TCCR0B	(( (REG_WGM >> 2) << WGM02) | (REG_CS0 << CS00))
#define		SET_OCR0A	TIMER_TOP
#define		SET_TIMSK0	(( REG_OCIE0A << OCIE0A))
#define		SET_OSCCAL	REG_CAL

#endif
