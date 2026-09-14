#ifndef __USART_H
#define __USART_H

#include <stdint.h>
#include "stm32f10x.h"                  // Device header

void USART2_Init();
void Serial_SendByte(USART_TypeDef* USARTx, uint16_t data);
void Serial_SendMsg(USART_TypeDef* USARTx, uint8_t data[], uint16_t len);

#endif
