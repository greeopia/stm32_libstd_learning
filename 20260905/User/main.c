#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Delay.h"
#include "Key.h"

uint16_t keynum;

int main(){
	
	LED_Init();
	Key_Init();
	while(1) {
		keynum = ReadKeyNum();
		if (keynum == GPIO_Pin_1) GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
		if (keynum == GPIO_Pin_11) GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);
	}
}
