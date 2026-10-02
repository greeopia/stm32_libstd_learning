#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"

void Key_Init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint16_t ReadKeyNum() {
	uint8_t Keynum = 0;
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == Bit_RESET) { // 因为是上拉
		Delay_ms(10);
		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == Bit_RESET);
		Keynum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == Bit_RESET) { // 因为是上拉
		Delay_ms(10);
		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == Bit_RESET);
		Keynum = 11;
	}
	return Keynum == 0 ? 0:(0x0001 << Keynum); // (0x0001 << Keynum)
}
