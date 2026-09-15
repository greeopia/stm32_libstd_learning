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

//typedef struct {
//	uint8_t *pData;
//	uint16_t len;
//} msg_t;

void Serial_SendByte(USART_TypeDef* USARTx, uint16_t data) { // 阻塞式发送底层
	USART_SendData(USARTx, data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) != SET);
}

int fputc(int ch, FILE* f) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
	USART_SendData(USART2, ch);
	return ch;
}

void Serial_SendMsg(USART_TypeDef* USARTx, uint8_t data[], uint16_t len) {
	for (uint16_t i = 0; i < len && data[i] != '\0'; i++) Serial_SendByte(USARTx,data[i]);
	Serial_SendByte(USARTx,'\r');
	Serial_SendByte(USARTx,'\n');
}

uint16_t Serial_RxByte(USART_TypeDef* USARTx) { // 阻塞式接收底层
	while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != SET);
	uint16_t Rx = USART_ReceiveData(USARTx);
	return Rx;
}

//void Serial_RxMsg(USART_TypeDef* USARTx, uint8_t RxMsg[]) { // ...要检测到 \r\n 才行
////	uint8_t i = 0
//	for (uint8_t i = 0; ; i++) {
//		if (RxMsg[i] == '\r') {
//			if (RxMsg[i+1] == '\n') {
//				return;
//			}
//		}
//		RxMsg[i] = Serial_RxByte(USARTx);
//	}
//}

void Serial_RxMsg(USART_TypeDef* USARTx, uint8_t RxMsg[], size_t len) { // ...要检测到 \n 才行，(win下只有enter的\n ?)
	for (size_t i = 0; i + 1 < len; i++) {
		RxMsg[i] = (uint8_t)Serial_RxByte(USARTx);
//		if (RxMsg[i] == '\n') { // 定义帧尾？
//			if (RxMsg[i-1] == '\r' && i-1 >= 0){
//				RxMsg[i-1] = '\0';
//				return;
//			}
//		}
		if (i > 0 /*&& RxMsg[i - 1] == '\r' */&& RxMsg[i] == '\n') {
//            RxMsg[i - 1] = '\0';
			RxMsg[i] = '\0';
            return;
        }
    }
//	RxMsg[len - 1] = '\0';
}
extern volatile uint8_t RxMsg2[50];
void USART2_IRQHandler() {
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		uint16_t Rx = USART_ReceiveData(USART2);
		for (size_t i = 0; i + 1 < sizeof(RxMsg2); i++) {
		RxMsg2[i] = (uint8_t)Serial_RxByte(USART2);
		if (i > 0 && RxMsg2[i] == '\n') {
			RxMsg2[i] = '\0';
            return;
        }
    }
	}
}
