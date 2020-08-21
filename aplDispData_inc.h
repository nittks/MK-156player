#ifndef APL_DISP_DATA_INC_H
#define APL_DISP_DATA_INC_H

//********************************************************************************
// 標準動作用定数
//********************************************************************************
#define		SPEED_MAX		((unsigned char)200)
#define		SPEED_MIN		((unsigned char)0)
#define		DISPCYC_7SEG_DIGIT			((unsigned short)1)	//1%辺りの周期[10ms](100%で1000ms)
#define		SPEED_DIGIT		((unsigned char)1)		//1%辺りの表示値

//********************************************************************************
// テストモード
//********************************************************************************
//テストデータ計算周期
#define		TEST_CYC_SPEED	((unsigned char)10-1)	//10x10ms=100ms毎処理
//計算毎の変化値
#define		CHG_VAL_SPEED	((unsigned char)2)
//ロータリークリック毎の変化値
#define		CHG_VAL_SPEED_MANUAL	((unsigned char)5)


typedef enum{
	TEST_STATE_UP,
	TEST_STATE_DOWN
}TEST_STATE;


//********************************************************************************
// 設定　パルス設定　表示値
//********************************************************************************
const unsigned char SETTING_PALSE_SPEED[]=	{0,4,8,16,20,25};		//車速パルス仕様
const unsigned char SETTING_PALSE_REV[]={0,1,2,3,4,5,6,8,10,12};	//回転数パルス仕様(気筒数

#define SET_BRIGHT_7SEG_DISP	((unsigned char)180)


#endif
