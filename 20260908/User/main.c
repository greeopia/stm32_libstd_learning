#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Encoder.h"
#include "OLED.h"

volatile int16_t cnt1, cnt2;

int main(){
	NVIC_Config();
	LEDInit();
	EncoderInit();
	OLED_Init();
	
	OLED_ShowString(1, 1, "cnt1:");
	OLED_ShowString(2, 1, "cnt2:");
	while (1) {
		LEDShow();
		OLED_ShowSignedNum(1,6,(int32_t)cnt1,3);
		OLED_ShowSignedNum(2,6,(int32_t)cnt2,3);
	}
}
