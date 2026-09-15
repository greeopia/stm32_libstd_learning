#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "usart.h"
#include "Key.h"
#include <string.h>
#include <stdbool.h>

volatile bool ClearFlag = 0;
volatile uint8_t RxMsg2[50] = "";

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	OLED_Init();
	USART2_Init();
	KeyInit();
	
//	uint8_t msg[] = "Hello World!", msg2[20];
//	uint16_t cnt = 0;
	uint8_t RxMsg[50] = "";
	
	
	printf("wow\r\n");
	while (1) {
//		if (ClearFlag == 0) {
//			Serial_RxMsg(USART2, RxMsg, sizeof(RxMsg));
//			OLED_ShowString(1, 1, (char *)RxMsg);
//		}
//		else {OLED_Clear(); ClearFlag = 0;} // 得两个都改成中断式的才行。。。 
//		uint8_t *RxMsg_bak; RxMsg_bak = (uint8_t *)malloc(sizeof(RxMsg));
//		Serial_RxMsg(USART2, RxMsg, sizeof(RxMsg));
//		
//		if (RxMsg_bak == RxMsg) OLED_ShowString(1, 1, (char *)RxMsg);
//		else {
//			OLED_Clear();
//			OLED_ShowString(1, 1, (char *)RxMsg);
//		}
//		free(RxMsg_bak);
	Serial_RxMsg(USART2, RxMsg, sizeof(RxMsg));
    OLED_Clear();
    OLED_ShowString(1, 1, (char *)RxMsg);

//        if (KeyScan()) {
//            OLED_Clear();
//        }
	}
//	if (KeyScan()) {
////		Serial_SendMsg(USART2, "Hello World!", strlen("Hello World!"));
////		OLED_ShowString(1, 1, "Hello World!");
////		OLED_ShowNum(1, strlen("Hello World!"), ++cnt, 2);
//		sprintf(msg2,  "%s*%2d", msg, ++cnt);
//		printf("你好 %s\r\n", msg2);
//		
//	}
}
