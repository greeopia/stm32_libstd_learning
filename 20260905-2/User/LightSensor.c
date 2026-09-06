#include "LightSensor.h"
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void init1() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 默认低电平，检测高电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//extern LEDStatus now;
void ReadSensor(LEDStatus *status) {
//	uint8_t SensorNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == Bit_SET) { 
//		Delay_ms(10);
//		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == Bit_SET);
//		SensorNum = 12;
		*status = PLAY_ON;
	}
	else if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == Bit_RESET) { 
//		Delay_ms(10);
//		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == Bit_RESET);
//		SensorNum = 0;
		*status = PLAY_OFF;
	}
//	return SensorNum == 0 ? 0:(0x0001 << SensorNum); // (0x0001 << Keynum)
}
