


#define		ON		1
#define		OFF		0

typedef struct{
	uint8_t		on;
	uint8_t		off;
}DEBOUNCE_TIME_CNT;

#define		PORT_VTC	((PORTC.IN >> PIN2_bp) & 0x01)
#define		DEBOUNCE_TIME	((uint8_t)500/10)	// 500ms ( 1cyc 10ms )