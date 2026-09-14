#ifndef __KEY_H
#define __KEY_H

void KeyInit();
void KeyScan();

typedef enum {
	OFF,
	ON
} KeyState_t;

extern KeyState_t KeyState;

#endif
