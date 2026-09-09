#include "LED.h"
#include "stm32f10x.h"                  // Device header

LEDState_t LEDState = IDLE; 

void LEDInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LEDShow() {
	switch (LEDState) {
		case IDLE:
			GPIO_WriteBit(GPIOA,GPIO_Pin_4, Bit_RESET);
			GPIO_WriteBit(GPIOA,GPIO_Pin_3 | GPIO_Pin_5, Bit_SET);
			break;
		case LEFT:
			GPIO_WriteBit(GPIOA,GPIO_Pin_5, Bit_RESET);
			GPIO_WriteBit(GPIOA,GPIO_Pin_3 | GPIO_Pin_4, Bit_SET);
			break;
		case RIGHT:
			GPIO_WriteBit(GPIOA,GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit(GPIOA,GPIO_Pin_4 | GPIO_Pin_5, Bit_SET);
			break;
	}
}