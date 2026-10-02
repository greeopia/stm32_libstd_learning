#include "stm32f10x.h"                  // Device header
#include "Key.h"

void KeyInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line12 | EXTI_Line15;
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

volatile Key_t keystate = idle;
uint32_t pre;

void KeyScan() {
	switch (keystate){
		case idle: {
			
			break;
		}
		case change_duty: {
			TIM1->CCR1 += (TIM1->ARR + 1 )/10;
			if (TIM1->CCR1 > TIM1->ARR) TIM1->CCR1 = 0;
			keystate = idle;
			break;
		}
		case change_freq: {
			pre = (pre + 1UL) * 10U;
			if (pre > 65536) pre = 72;
			TIM_PrescalerConfig(TIM1, pre - 1, TIM_PSCReloadMode_Update);			
			keystate = idle;
			break;
		}
	}
}



void EXTI15_10_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line12) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line12);
//		// TIM1->CCR1 -> Duty
//		TIM1->CCR1 += TIM1->ARR/10;
//		if (TIM1->CCR1 > TIM1->ARR) TIM1->CCR1 = 0;
		keystate = change_duty;
	}
	if (EXTI_GetITStatus(EXTI_Line15) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line15);
//		// TIM_PrescalerConfig
//		// TIM1->PSC -> Freq
//		 pre = (pre + 1UL) * 10U;
//		if (pre > 65536) pre = 72;
//		TIM_PrescalerConfig(TIM1, pre - 1, TIM_PSCReloadMode_Update);
		keystate = change_freq;
	}	
}
