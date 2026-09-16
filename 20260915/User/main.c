#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "usart.h"
#include "Key.h"
#include <string.h>
#include <stdbool.h>

volatile bool ClearFlag = OFF;
volatile uint8_t RxMsg[50] = "";
volatile uint8_t RxFlag = 0;

int main(){ // 大概就是中断式串口收发数据包
	OLED_Init();
	KeyInit();
	USART2_Init();
	
//	OLED_ShowString(1, 1, "OLED OK");
	while (1) {
		if (ClearFlag == 1) {
			ClearFlag = 0;
			OLED_Clear();
		}
		if (RxFlag == 1) {
			RxFlag = 0;
			OLED_Clear();
			OLED_ShowString(1, 1, (char *)RxMsg);
		}
	}
}
