#ifndef __KEY_H
#define __KEY_H

#include <stdbool.h>

typedef enum {
	OFF,
	ON = !OFF
} KeyState_t;

void KeyInit();
bool KeyScan();

extern KeyState_t KeyState;

#endif
