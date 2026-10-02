#ifndef __LED_H
#define __LED_H

typedef enum {
	IDLE,
	LEFT,
	RIGHT
}LEDState_t;

void LEDInit();
void LEDShow();

#endif