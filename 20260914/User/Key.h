#ifndef __KEY_H
#define __KEY_H

#include <stdbool.h>

void KeyInit();
bool KeyScan();

typedef enum {
	OFF,
	ON = !OFF
} KeyState_t;

extern KeyState_t KeyState;

#endif
