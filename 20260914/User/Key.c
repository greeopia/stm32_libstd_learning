#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "Delay.h"
#include "stdbool.h"
void KeyInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

KeyState_t KeyState = OFF;

bool KeyScan() {
//	static uint8_t cnt = 1; 
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == SET /*&& cnt == 1*/) {
		Delay_ms(10);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == SET);
		KeyState = !KeyState;
//		cnt = 0;
		return 1;
	}
	
}
