#pragma once

typedef enum {
	UART,
	KEY
} key_mode_t;
extern key_mode_t key_mode;

typedef enum {
	MODE2_FREQ,
	MODE2_DUTY
} key_mode2_t;
extern key_mode2_t key_mode2;

void key_init();