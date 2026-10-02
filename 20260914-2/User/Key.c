#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "Delay.h"
#include "stdbool.h"
#include <stdio.h>
void KeyInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line12;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
}

KeyState_t KeyState = OFF;
extern volatile bool ClearFlag;

bool KeyScan() {
//	static uint8_t cnt = 1; 
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == SET /*&& cnt == 1*/) {
		Delay_ms(10);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == SET);
		return 1;
	}
	return 0;
}

void EXTI15_10_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line12) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line12);
		ClearFlag = 1;
	}
}
