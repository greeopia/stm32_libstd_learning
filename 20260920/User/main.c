#include "stm32f10x.h"                  // Device header
#include "tim.h"
#include "OLED.h"
#include "Key.h"

volatile uint32_t Freq, Duty;
volatile uint32_t Freq_kq, Duty_kq;

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	OLED_Init();
	tim1_init();
	tim4_init();
	KeyInit();
	pre = TIM1->PSC;
	
	OLED_ShowString(1, 1, "Freq:");
	OLED_ShowString(2, 1, "Freq_kq:");
	OLED_ShowString(3, 1, "Duty:");
	OLED_ShowString(4, 1, "Duty_kq:");
	
//	volatile uint16_t ccr3_before = TIM4 -> CCR3;
	while (1) {
		volatile uint32_t TIM4_CNT_f = 7.2e7/(TIM4->PSC+1);
		Duty = TIM4->CCR4 * 100/TIM4->CCR3;
		Freq = TIM4_CNT_f/(TIM4->CCR3 + 1);
		
		// _kq为OC数据，从这里应该也能看出想要改变pwm波输出的Freq和Duty最好应该改变psc和ccr的值
		Freq_kq = 7.2e7/((TIM1->PSC + 1)*(TIM1->ARR + 1));
		Duty_kq = TIM1->CCR1 * 100/(TIM1->ARR + 1);
		// 可以弄一个按钮外部中断捕获TIM1->CCR1
		
		OLED_ShowNum(1, 7, Freq, 5);
		OLED_ShowNum(2, 9, Freq_kq, 5);
		OLED_ShowNum(3, 7, Duty, 2);
		OLED_ShowNum(4, 9, Duty_kq, 2);		
		
		KeyScan();
////		if (TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET) {
////			TIM_ClearFlag(TIM1, TIM_FLAG_Update);
////			
////		}
//		if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC3) == SET) {
//			TIM_ClearFlag(TIM4, TIM_FLAG_CC3);
//			ccr3 = TIM4 -> CCR3;
//			
//		}
//		else if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC4) == SET) {
//			TIM_ClearFlag(TIM4, TIM_FLAG_CC4);
//			ccr4 = TIM4 -> CCR4;
//		}
//		// 孩子们我不用PWMI直接用普通的IC模式测量占空比
//		// Duty = CCR/(ARR+1) -> 
//		// Freq = CK_PSC/((PSC+1)*(ARR+1)) -> (72MHz/TIM_GetPrescaler(TIM4))/(TIM_GetCapture3(TIM4)+1)
//		// 那当然只能用IC:TIM4的数据了(
//		Freq = TIM4_CNT_f/(TIM_GetCapture3(TIM4)+1); // TIM4_CNT_f/ccr3;
//		Freq_kq = 7.2e7/((TIM1 -> PSC + 1)*(TIM1 -> ARR + 1));
//		OLED_ShowNum(2, 7, Freq, 4);
//		OLED_ShowNum(4, 10, Freq_kq, 4);
//		
//		ccr3_before = ccr3;
/* 下降沿：CH4间接捕获TI3 */
//for (uint16_t i = 0; i < TIM1->ARR; i++) TIM1->CCR1 = i;
	}
}
