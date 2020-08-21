#ifndef APL_CTRL_H
#define APL_CTRL_H

//状態
typedef enum{
	APL_CTRL_STATE_BOOT,		//起動初回
	APL_CTRL_STATE_NOMARL,		//通常
	APL_CTRL_STATE_TESTDISP,	//テスト表示
	APL_CTRL_STATE_SETTING		//設定
}APL_CTRL_STATE;

//状態(テスト表示内
typedef enum{
	APL_CTRL_STATE_TEST_AUTO,
	APL_CTRL_STATE_TEST_SPEED,
}APL_CTRL_STATE_TEST;

//状態(設定内
typedef enum{
	APL_CTRL_STATE_SET_COLOR_7SEG,			//調色
	APL_CTRL_STATE_SET_BRIGHT_7SEG,			//調光(7セグ
	APL_CTRL_STATE_SET_BRIGHT_DIM_7SEG,		//調光減光(7セグ
	APL_CTRL_STATE_SET_DISPCYC_7SEG,		//表示更新速度(7セグ
	APL_CTRL_STATE_SET_PALSE_SPEED,			//パルス仕様車速
	APL_CTRL_STATE_SET_PALSE_REV,			//パルス仕様回転数
	APL_CTRL_STATE_SET_MAX
}APL_CTRL_STATE_SET;


typedef struct {
	APL_CTRL_STATE		state;
	APL_CTRL_STATE_TEST	stateTest;
	APL_CTRL_STATE_SET	stateSet;
}APL_CTRL;

typedef struct {
	unsigned char	color7seg;			//調色
	unsigned char	bright7seg;			//調光(7セグ
	unsigned char	brightBarled;		//調光(バーLED
	unsigned char	brightDim7seg;		//調光減光(7セグ
	unsigned char	brightDimBarled;	//調光減光(バーLED
	unsigned char	dispcyc7seg;		//表示更新速度(7セグ
	unsigned char	dispcycBarled;		//表示更新速度(バーLED
}APL_CTRL_SET;

typedef struct {
	unsigned char	speed;			//パルス仕様車速   
	unsigned char	rev;			//パルス仕様回転数 
}APL_CTRL_SET_PALSE;



typedef enum{
	SET_COLOR,
	SET_PALSE_SPEED,
	SET_PALSE_REV,
	SET_ITEM_NO_MAX
}SET_ITEM_NO;

enum{
	SETTING_COLOR_RED,
	SETTING_COLOR_GREEN,
	SETTING_COLOR_BLUE,
	SETTING_COLOR_WHITE,
	SETTING_COLOR_MAX = SETTING_COLOR_WHITE
};

enum{
	SETTING_PALSE_SPEED_04	= 1,
	SETTING_PALSE_SPEED_08,
	SETTING_PALSE_SPEED_16,
	SETTING_PALSE_SPEED_20,
	SETTING_PALSE_SPEED_25,
	SETTING_PALSE_SPEED_MAX = SETTING_PALSE_SPEED_25	//1始まりのため-1
};
enum{
	SETTING_PALSE_REV_01	= 1,
	SETTING_PALSE_REV_02,
	SETTING_PALSE_REV_03,
	SETTING_PALSE_REV_04,
	SETTING_PALSE_REV_05,
	SETTING_PALSE_REV_06,
	SETTING_PALSE_REV_08,
	SETTING_PALSE_REV_10,
	SETTING_PALSE_REV_MAX = SETTING_PALSE_REV_10	//1始まりのため-1
};



extern void initAplCtrl( void );
extern void aplCtrlMain( void );
extern APL_CTRL *getAplCtrl( void );
extern APL_CTRL_SET *getAplCtrlSet( void );
extern APL_CTRL_SET_PALSE *getAplCtrlSetPalse( void );


#endif
