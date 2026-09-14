#ifndef __USART_H
#define __USART_H

#include <stdint.h>
#include "stm32f10x.h"                  // Device header

void USART2_Init();
void Serial_SendByte(USART_TypeDef* USARTx, uint16_t data);

#endif
