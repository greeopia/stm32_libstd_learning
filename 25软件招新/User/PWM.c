#include "stm32f10x.h"                  // Device header
#include "PWM.h"

volatile uint16_t FREQ, DUTY;

void pwm_init() { // TIM3CH1: PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM3);
	
	// 7.2e7=72000000
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period =  10 - 1; // ARR x
	TIM_TimeBaseInitStruct.TIM_Prescaler =  720 - 1; // PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 4; // CCR
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	
	TIM_Cmd(TIM3, ENABLE);
} // OC: Freq = ; Duty = CCR/(ARR+1);
// duty = TIM3->CCR/(TIM3->ARR+1) // TIM3->CCR1: 10~90
// freq = 7.2e7/((TIM3->PSC+1)*(TIM3->ARR+1))=1000~9000 ->  (TIM3->PSC+1)*(TIM3->ARR+1) = 8000 ~ 72000 // TIM3->PSC+1: 80~720
// PSC: freq; CCR: duty


