#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "IC.h"
#include "usart.h"
#include "key.h"
#include "OLED.h"

extern key_mode_t key_mode; // KEY UART
extern key_mode2_t key_mode2; // MODE2_FREQ MODE2_DUTY

extern volatile uint16_t uart_pwm, uart_freq;
extern volatile uint16_t FREQ, DUTY;
extern uint32_t IC_FREQ;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	key_init();
	pwm_init();
	IC_init();
	usart_init();
	OLED_Init();
	
	OLED_ShowString(1, 1, "F="); OLED_ShowString(1, 7, "Hz");
	OLED_ShowString(2, 1, "T:"); OLED_ShowString(2, 7, "Hz");
	OLED_ShowString(3, 1, "PWM="); OLED_ShowString(3, 7, "%");
	OLED_ShowString(4, 1, "MODE:");
	while(1)
	{
		IC_FREQ = 7.2e7/((TIM2->PSC+1)*(TIM2->CCR2));
		if (key_mode == KEY) {
			OLED_ShowString(4, 6, "KEY  ");
			DUTY = TIM3->CCR1/(TIM3->ARR+1);
			FREQ = 7.2e6/(TIM3->PSC + 1);
			
//			OLED_ShowNum(1, 3, FREQ, 4);
//			OLED_ShowNum(3, 5, DUTY*100, 2);
		}
		else if (key_mode == UART) {
			OLED_ShowString(4, 6, "UART");
			usart_RxMsg();
			
//			OLED_ShowNum(1, 3, uart_freq, 4);
//			OLED_ShowNum(3, 5, uart_pwm, 2);
		}
		OLED_ShowNum(2, 3, IC_FREQ, 4);
		
		OLED_ShowNum(1, 3, FREQ, 4);
		OLED_ShowNum(3, 5, DUTY, 2);
	}
}
