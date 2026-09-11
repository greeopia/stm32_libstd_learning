#ifndef __USART_H
#define __USART_H

#include <stdint.h>
#include "stm32f10x.h"                  // Device header

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t s;
} Daytime_t;

extern volatile Daytime_t Nowtime;
extern volatile uint8_t Daytime_Valid;
extern volatile uint8_t Daytime_DisplayDirty;

void USART1_Init(void);
void Serial_SendByte(USART_TypeDef* USARTx, uint8_t byte);
uint8_t USART1_TryCalibrateTime(void);
uint8_t Daytime_TakeDisplaySnapshot(Daytime_t *snapshot);

#endif
