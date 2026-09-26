#include "stm32f10x.h"                  // Device header
#include "key.h"

void key_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line4 | EXTI_Line5;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);	
}

key_mode_t key_mode = KEY;
key_mode2_t key_mode2 = MODE2_DUTY;

extern volatile uint16_t FREQ, DUTY;
//extern volatile uint16_t uart_pwm, uart_freq;

void EXTI0_IRQHandler() { // KEY UART
	if (EXTI_GetITStatus(EXTI_Line0) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line0);
		if (key_mode == KEY) {
			key_mode = UART;
//			
//			uart_pwm = DUTY;
//			uart_freq = FREQ;
		}
		else if (key_mode == UART) {
			key_mode = KEY;
//			
//			DUTY = uart_pwm;
//			FREQ = uart_freq;
		}
	}
}

void EXTI1_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line1) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line1);
		if (key_mode2 == MODE2_DUTY) key_mode2 = MODE2_FREQ;
		else if (key_mode2 == MODE2_FREQ) key_mode2 = MODE2_DUTY;		
	}	
}
//static uint32_t PSC_arr[9] = {0};
//static int8_t pData = 3;
void EXTI4_IRQHandler() { // +
	if (EXTI_GetITStatus(EXTI_Line4) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line4);
		if (key_mode == KEY) {
			if (key_mode2 == MODE2_DUTY) {
				TIM3->CCR1 += 1;
				if (TIM3->CCR1 > 9) TIM3->CCR1 = 1;
			}
			else if (key_mode2 == MODE2_FREQ) {
//				pData++;
//				if (pData == 9) pData = 0;
//				
//				TIM3->PSC = PSC_arr[pData];
////				TIM3->PSC = PSC_arr[pData++];
				FREQ += 1000;
				if (FREQ > 9100) FREQ = 1000;
				TIM3->PSC = 7.2e6/FREQ - 1;
			}
		}
	}	
}

void EXTI9_5_IRQHandler() { // -
	if (EXTI_GetITStatus(EXTI_Line5) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line5);
		if (key_mode == KEY) {
			if (key_mode2 == MODE2_DUTY) {
				TIM3->CCR1 -= 1;
				if (TIM3->CCR1 < 1) TIM3->CCR1 = 9;
			}
			else if (key_mode2 == MODE2_FREQ) {
//				pData--;
//				if (pData == -1) pData = 8;
//				TIM3->PSC = PSC_arr[pData];
				FREQ -= 1000;
				if (FREQ < 1000) FREQ = 9000;
				TIM3->PSC = 7.2e6/FREQ - 1;				
			}			
		}
	}	
}
