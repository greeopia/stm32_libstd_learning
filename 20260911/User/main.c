#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "tim.h"
#include "usart.h"

int main(void){
	Daytime_t displayTime;

	NVIC_Config();
	OLED_Init();
	TIM1Init();
	USART1_Init();
	
	OLED_ShowString(1, 7, "Time");
	OLED_ShowString(2, 1, "----/--/--");
	OLED_ShowString(3, 1, "--:--:--");
	
	while (1) {
		/* 收到新 JSON 时只负责校准 Nowtime。 */
		USART1_TryCalibrateTime();

		/* 首次校准以及此后每秒钟，仅刷新一次 OLED。 */
		if (Daytime_TakeDisplaySnapshot(&displayTime)) {
			OLED_ShowNum(2, 1, displayTime.year, 4);
			OLED_ShowChar(2, 5, '-');
			OLED_ShowNum(2, 6, displayTime.month, 2);
			OLED_ShowChar(2, 8, '-');
			OLED_ShowNum(2, 9, displayTime.day, 2);

			OLED_ShowNum(3, 1, displayTime.hour, 2);
			OLED_ShowChar(3, 3, ':');
			OLED_ShowNum(3, 4, displayTime.min, 2);
			OLED_ShowChar(3, 6, ':');
			OLED_ShowNum(3, 7, displayTime.s, 2);
		}
	}
}
