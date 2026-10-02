#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "usart.h"
#include "Key.h"

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART2_Init();
	OLED_Init();
	KeyInit();
	while (1) {
		if (KeyScan() == 1) {
//		switch (KeyState) {
//			case ON: {
//				Serial_SendByte(USART2, 0x55);
//				break;
//			}
//			case OFF: {
//				Serial_SendByte(USART2, 0x56);
//				break;
//			}
//		}
			Serial_SendByte(USART2, 0x55); // 应该算是一种新的状态机的形式了吧
	}
}
}
