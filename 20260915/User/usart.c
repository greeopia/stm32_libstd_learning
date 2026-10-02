#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include <stdio.h>

void USART2_Init() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 神了长得太像了 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // TX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // uart空闲时高电平
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; // RX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); // 相当于中断式发送/接收吗？
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2, ENABLE);
}

extern volatile uint8_t RxMsg[50];
static volatile uint8_t pData = 0;
extern volatile uint8_t RxFlag;
void USART2_IRQHandler() {
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) { // 读数据会清除
//		if (pData < sizeof(RxMsg) - 1) {
//			RxMsg[pData] = (uint8_t)USART_ReceiveData(USART2);
//			if (RxMsg[pData] == '\n') {
//				RxMsg[pData] = '\0';
//				RxFlag = 1;
//				pData = 0;
//				return;
//			}
//			pData++;
//		}
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	}
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) { // 以后或许可以改造一下RxMsg[]，手动维护一个环形队列 
//		RxMsg[pData] = (uint8_t)USART_ReceiveData(USART2);
		uint8_t temp = (uint8_t)USART_ReceiveData(USART2);
		if (temp == '\n') {
			temp = '\0';
			RxMsg[pData] = '\0'; // 要添加这个标志才不会乱显...
			RxFlag = 1;
			pData = 0;
			return;
		}
		if (pData < sizeof(RxMsg) - 1) RxMsg[pData++] = temp;
		else pData = 0;
	}
}
