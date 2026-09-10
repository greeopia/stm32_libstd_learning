#include "tim.h"
#include "stm32f10x.h"                  // Device header
#include "usart.h"

void TIM1Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_InternalClockConfig(TIM1); //设为内部时钟
	
	TIM_TimeBaseInitTypeDef TimeBaseInitstructure;
	TimeBaseInitstructure.TIM_ClockDivision = TIM_CKD_DIV4;
	TimeBaseInitstructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInitstructure.TIM_Period = 10000 - 1;
	TimeBaseInitstructure.TIM_Prescaler = 7200 - 1;
	TimeBaseInitstructure.TIM_RepetitionCounter = 0; // 1s计数
	TIM_TimeBaseInit(TIM1, &TimeBaseInitstructure);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler() {
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		Nowtime.s++;
		if (Nowtime.s == 60) {
			Nowtime.s = 0;
			Nowtime.min++;
			if (Nowtime.min == 60) {
				Nowtime.min = 0;
				Nowtime.hour++;
				if (Nowtime.hour == 24) Nowtime.hour = 0;
			}
		}
	}
}

void NVIC_Config() {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}