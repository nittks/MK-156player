
typedef enum{
	DRV_IN_CAR_SW_OFF,
	DRV_IN_CAR_SW_ON
}DRV_IN_CAR_SW_STATE;

typedef struct{
	DRV_IN_CAR_SW_STATE	vtc;
}DRV_IN_CAR_SW;

extern void initDrvInCarSw( void );
extern void drvInCarSwMain( void );
extern DRV_IN_CAR_SW *getDrvInCarSw( void );
