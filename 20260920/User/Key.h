#pragma once

void KeyInit();

typedef enum {
	change_freq,
	change_duty,
	idle
} Key_t;
extern volatile Key_t keystate;
extern uint32_t pre;

void KeyScan();
