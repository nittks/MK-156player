#ifndef APL_DATA_H
#define APL_DATA_H

#include <stdbool.h>
#include <stdlib.h>
//********************************************************************************
// 車両情報
//********************************************************************************
enum{
	APL_DATA_ILL_OFF,
	APL_DATA_ILL_ON,
};
enum{
	APL_DATA_IG_OFF,
	APL_DATA_IG_ON,
};
enum{
	APL_DATA_ACC_OFF,
	APL_DATA_ACC_ON,
};

typedef struct{
	bool			rx;
	uint8_t			waterTemp;
	
	unsigned char	sumerr;
	
	unsigned char	speed;
	unsigned short	rev;
	unsigned char	palseSetSpeed;
	unsigned char	palseSetRev;
	unsigned char	ig;
	unsigned char	acc;
	unsigned char	ill;
	unsigned char	vtc;
}APL_DATA_CAR;

//********************************************************************************
// スイッチ入力
//********************************************************************************
//パルス間隔変更入力
typedef enum{
	APL_DATA_ROT_ENC_STOP,		//停止
	APL_DATA_ROT_ENC_UP,		//増
	APL_DATA_ROT_ENC_DOWN		//減
}APL_DATA_ROT_ENC;

//プッシュスイッチ入力
typedef enum{
	APL_DATA_PUSH_SW_OFF,
	APL_DATA_PUSH_SW_ON,
	APL_DATA_PUSH_SW_LONGON,
	APL_DATA_PUSH_SW_DOUBLEON
}APL_DATA_PUSH_SW;

enum{
	APL_DATA_ROT_ENC_SET,
	APL_DATA_ROT_ENC_NUM
};
enum{
	APL_DATA_PUSH_SW_SET,

	APL_DATA_PUSH_SW_NUM
};

typedef struct{
	APL_DATA_ROT_ENC	rotEncSet;
	APL_DATA_PUSH_SW	pushSwSet;
	bool				mk156Busy;
}APL_DATA_SW;

//********************************************************************************
// EEPROM
//********************************************************************************
typedef enum{
	APL_DATA_SET_COLOR_7SEG,
	APL_DATA_SET_BRIGHT_7SEG,
	APL_DATA_SET_BRIGHT_DIM_7SEG,
	APL_DATA_SET_DISPCYC_7SEG,
	APL_DATA_SET_SETTING_ITEM_MAX
}APL_DATA_SET_SETTING_ITEM;

typedef enum{
	APL_DATA_EEP_STATE_UNREAD,
	APL_DATA_EEP_STATE_READED,
	APL_DATA_EEP_STATE_SUMERROR
}APL_DATA_EEP_STATE;

typedef struct{
	APL_DATA_EEP_STATE	read;
	unsigned char		color7seg;
	unsigned char		h;
	unsigned char		s;
	unsigned char		v;
	unsigned char		bright7seg;
	unsigned char		brightDim7seg;
	unsigned char		dispcyc7seg;
}APL_DATA_EEP;


//********************************************************************************
// EEPROM
//********************************************************************************
extern void initAplData( void );
extern void aplDataMain( void );
extern void setAplDataCar(APL_DATA_CAR * inData );
extern void setAplDataSw(APL_DATA_SW * inData );
extern void setAplDataEep( APL_DATA_EEP * inData );
extern APL_DATA_CAR *getAplDataCar(void);
extern APL_DATA_SW *getAplDataSw(void);
extern APL_DATA_EEP *getAplDataEep(void);
#endif
