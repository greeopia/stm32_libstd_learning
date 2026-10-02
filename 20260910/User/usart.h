#ifndef __USART_H
#define __USART_H

#include "time.h"
#include <stdint.h>

typedef struct {
	volatile uint8_t year,month,day;
	volatile uint32_t hour,min,s;
} Daytime_t;

extern Daytime_t Nowtime;

#endif