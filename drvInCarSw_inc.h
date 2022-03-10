


#define		ON		1
#define		OFF		0


#define		PORT_VTC	((PORTF.IN >> PIN5_bp) & 0x01)

#define		DEBOUNCE_TIME	((uint8_t)100/10)	// 100ms ( 1cyc 10ms )