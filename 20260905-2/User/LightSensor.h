#ifndef __LIGHTSENSOR_H
#define __LIGHTSENSOR_H

#include <stdint.h>

typedef enum {
	PLAY_OFF = 0,
	PLAY_ON
} LEDStatus;

void init1();
void ReadSensor(LEDStatus *status);

#endif
