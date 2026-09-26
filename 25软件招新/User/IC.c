#include "stm32f10x.h"                  // Device header
#include "IC.h"

uint32_t IC_FREQ;

void IC_init() { // PWMI模式？TIM2CH2:PA1 direct; CH1:indirect
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period =  65536 - 1;//ARR
	TIM_TimeBaseInitStruct.TIM_Prescaler =  72 - 1; //PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	//	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE); // 有PWMI就不需要中断

	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0xF;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_PWMIConfig(TIM2, &TIM_ICInitStruct); // CCR2 CCR1
	
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
//	NVIC_Init(&NVIC_InitStruct);
	
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM2, ENABLE);
} // Duty = TIM2->CCR2 / TIM2->CCR1

//void TIM2_IRQHandler() {
//	if (TIM_GetITStatus(TIM2, TIM_IT_Trigger) == SET) {
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Trigger);
//		
//	}
//}
