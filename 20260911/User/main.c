#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "tim.h"
#include "usart.h"
#include "cJSON.h"
#include "cJSON_Utils.h"

int main(){
	NVIC_Config();
	OLED_Init();
	TIM1Init();
	USART1_Init();
	
	OLED_ShowString(1, 7, "Time");
	
	while (1) {
		OLED_ShowNum(2, 1, Nowtime.s, 2);
	}
}
