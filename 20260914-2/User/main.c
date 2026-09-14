#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "usart.h"
#include "Key.h"
#include <string.h>
#include <stdio.h>

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	OLED_Init();
	USART2_Init();
	KeyInit();
	
	uint8_t msg[] = "Hello World!", msg2[20];
	uint16_t cnt = 0;
	
	printf("wow\r\n");
	while (1)
	if (KeyScan()) {
//		Serial_SendMsg(USART2, "Hello World!", strlen("Hello World!"));
//		OLED_ShowString(1, 1, "Hello World!");
//		OLED_ShowNum(1, strlen("Hello World!"), ++cnt, 2);
		sprintf(msg2,  "%s*%2d", msg, ++cnt);
		printf("你好 %s\r\n", msg2);
		
	}
}
