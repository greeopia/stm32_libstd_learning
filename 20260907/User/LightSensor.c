#include "LightSensor.h"
#include "stm32f10x.h"

void SensorInit() {
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 默认低电平，检测高电平
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 默认低电平，检测高电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Config() {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

volatile uint32_t SensorCnt = 0;
void EXTI15_10_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line12) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line12);
//		// your logic here
//		++SensorCnt; // 疑似抖动得过于强烈了 
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_RESET /*因为默认低电平？*/) ++SensorCnt;
		// 破案了，检测有无光的这一个过程，这个传感器会传送多次脉冲。
	}
}
