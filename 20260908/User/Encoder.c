#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "LED.h"

void EncoderInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // 左右转
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 中间按键
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; // 孩子们这个编码器没有引出sw引脚（不过想要可以自己试着焊或者用手对着接）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

extern LEDState_t LEDState;
extern volatile int16_t cnt1, cnt2;

void EXTI15_10_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line12) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line12);
		++cnt2;
		// LEDState = IDLE;
	}
}

void EXTI0_IRQHandler() { // A
	if (EXTI_GetITStatus(EXTI_Line0) == SET) { 
		EXTI_ClearITPendingBit(EXTI_Line0);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET)
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_RESET)

{++cnt1; LEDState = LEFT; }
	}
}

void EXTI1_IRQHandler() { // B
	if (EXTI_GetITStatus(EXTI_Line1) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line1);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_RESET) 
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET)

{--cnt1; LEDState = RIGHT; }
	}
}

void NVIC_Config() {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}