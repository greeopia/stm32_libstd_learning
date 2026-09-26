#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "string.h"

void usart_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1, ENABLE);
}

volatile uint8_t RxMsg[15] = "";
static uint8_t pData = 0;

void USART1_IRQHandler() {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		uint16_t temp = (uint8_t)USART_ReceiveData(USART1);
		if (temp == '\n') {
			RxMsg[pData] = '\0';
			pData = 0;
			return;
		}
		if (pData  < sizeof(RxMsg) - 1) RxMsg[pData++] = temp;
		else pData = 0;
	}
}

//uint8_t px, py, fx, fy;
//volatile uint16_t uart_pwm = 0, uart_freq = 0;
extern volatile uint16_t FREQ, DUTY;
void usart_RxMsg() {
	if (RxMsg[0] == 'P') { // PWM
		volatile uint8_t px = RxMsg[4] - '0', py = RxMsg[5] - '0';
//		uart_pwm = 10*px + py;
		DUTY = 10*px + py;
	}
	else if (RxMsg[0] == 'F') { // F
		volatile uint8_t fx = RxMsg[2] - '0', fy = RxMsg[3] - '0', fz = RxMsg[4] - '0', fa = RxMsg[5] - '0';
//		uart_freq = 1000*fx + 100*fy + 10*fz + fa;	
		// 孩子们记住数组下标从0开始
		FREQ = 1000*fx + 100*fy + 10*fz + fa;	
	}
//	TIM3->CCR1 = uart_pwm/10;
//	TIM3->PSC = 7.2e6/uart_freq - 1;
	TIM3->CCR1 = DUTY/10;
	TIM3->PSC = 7.2e6/FREQ - 1;
}
