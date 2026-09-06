#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "LightSensor.h"
#include "Delay.h"
#include "Key.h"

uint16_t SensorNum = 0;

LEDStatus now = PLAY_OFF;

int main(){
	
	LED_Init();
	init1();
	Key_Init();
	while(1) {
		ReadSensor(&now);
		switch (now) {
			case PLAY_OFF: {
				if (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1) == Bit_SET) {
				GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_RESET);
				Delay_ms(200);
				GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_SET);
				Delay_ms(200);
				} else {
					GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_SET);
				Delay_ms(200);
				GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_RESET);
				Delay_ms(200);
				}
			}	
			break;
			
			case PLAY_ON: {
				GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_SET);
				for (int i = 1; i <= 3; i++) {
					GPIO_WriteBit(GPIOA, GPIO_Pin_(i), Bit_RESET);
					Delay_s(1);
					GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_SET);
				}
			}
			break;
		}
	}
}
