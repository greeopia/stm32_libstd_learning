#include "stm32f10x.h"                  // Device header
#include "tim.h"
#include "OLED.h"

int main(){
	tim3_init();
	OLED_Init();
	extern const int TIM_ARR;
	
	while (1) {
		for (int i = 0; i <= TIM_ARR; i++){ // Duty = CCR/(ARR+1)
		TIM_SetCompare2(TIM3, i);
			Delay_ms(1);
			OLED_Clear();
			OLED_ShowNum(1, 1, (uint32_t)TIM3->CCR2, 4);
		}
		for (int i = TIM_ARR; i >= 0; i--) {// Duty = CCR/(ARR+1)
		TIM_SetCompare2(TIM3, i);
			Delay_ms(1);
			OLED_Clear();
			OLED_ShowNum(1, 1, (uint32_t)TIM3->CCR2, 4);
		}
	}
}
