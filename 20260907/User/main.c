#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LightSensor.h"
int main(){
	NVIC_Config();
	SensorInit();
	OLED_Init();
	
	extern uint32_t SensorCnt;
	OLED_ShowString(1, 1, "Hello my friends");
	OLED_ShowString(3, 2, "cnt:");
	while (1) {
		OLED_ShowNum(3, 6, SensorCnt, 3);
	}
}
