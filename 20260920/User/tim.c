#include "stm32f10x.h"                  // Device header
#include "tim.h"

/*

PWM_f = TIM_f/((PSC+1)*(ARR+1))
Duty = CCR/(ARR+1)
// 所以一般普遍来说 改PSC和CCR会好一些 
// void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode); // TIM_PSCReloadMode can be TIM_PSCReloadMode_Update or TIM_PSCReloadMode_Immediate
// uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx); // PSC

// void TIM_SetComparek(TIM_TypeDef* TIMx, uint16_t Comparek); (where x can be 1-4)
// uint16_t TIM_GetCapturek(TIM_TypeDef* TIMx); (where x can be 1-4) // CCR

*/

void tim4_init() { // IC -> TIM4CH3:PB8, TIM4CH4:PB9
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 配置为输入捕获感觉还是浮空输入比较好 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65536 - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
	
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_3;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM4, &TIM_ICInitStruct); // 通常ICinit
	// TIM_PWMIConfig(TIM4, &TIM_ICInitStruct); // PWMI模式 -> 占用且仅占用CH1 && CH2
	// 也可以分别CH1 CH2初始化:比如一个上升沿直连另一个下降沿交叉
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI; // 表示接到FP1
	TIM_ICInit(TIM4, &TIM_ICInitStruct);
	
//	TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1); // TRGI
//	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset); // ? 其实还是不能太懂上面两行
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM4, ENABLE);
}

void TIM4_IRQHandler() { // IC -> TIM4CH3:PB8(RisingDirect), TIM4CH4:PB9(FallingIndirect)
	// 模仿PWMI模式...不过是软件方式2333
	if (TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
		// TIM_GetCounter(TIM4)
		TIM4->CCR3 = TIM_GetCounter(TIM4);
		TIM4->CNT = 0;
	}
	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
		TIM4->CCR4 = TIM_GetCounter(TIM4);
	}
}

void tim1_init() { // OC -> TIM1CH1:PA8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1; // ARR
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1; // PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_Pulse = 5000; // CCR
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
//	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update); // TRGO ?
	
	TIM_Cmd(TIM1, ENABLE);
}
