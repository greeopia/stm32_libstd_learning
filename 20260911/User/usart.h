#ifndef __USART_H
#define __USART_H

#include <stdint.h>
#include "stm32f10x.h"                  // Device header

typedef struct {
	volatile uint8_t year,month,day;
	volatile uint32_t hour,min,s;
} Daytime_t;
extern Daytime_t Nowtime;

void USART1_Init();
void Serial_SendByte(USART_TypeDef* USARTx, uint8_t byte);

#endif