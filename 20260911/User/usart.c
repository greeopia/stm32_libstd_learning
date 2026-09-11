#include "usart.h"
#include "stm32f10x.h"                  // Device header
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "time.h"

Daytime_t Nowtime = {0,0,0,0,0,0};

void USART1_Init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //666 ctrl+alt+space
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 为了接收才有的ITConfig和NVIC
//	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(USART_TypeDef* USARTx, uint8_t byte) { // 阻塞式发送
	USART_SendData(USARTx, byte);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

void USART1_IRQHandler() {
	if (USART_GetITStatus(USART1,USART_IT_RXNE) == SET) { // 收
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		// logic
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) { // 发
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
		
	}
}