#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

#define GPIO_Pin_(i) (((uint16_t)0x0001)<<(i))

void Key_Init();
uint16_t ReadKeyNum();

#endif
