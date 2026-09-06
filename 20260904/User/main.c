//#include "stm32f10x.h"                  // Device header
//#include "Delay.h"
////volatile uint32_t sys_tick_ms;



////void SysTick_Handler(void)
////{
////    sys_tick_ms++;
////}

////#define PERIODIC_MS(x) \
////    do { \
////        static uint32_t last_time = 0; \
////        uint32_t now = sys_tick_ms; \
////        if ((now - last_time) < (x)) \
////            return; \
////        last_time = now; \
////    } while(0)

//int main(){
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	
////	if (SysTick_Config(SystemCoreClock / 1000))
////	{
////    while(1);
////	}

//	while(1) {
////		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == Bit_SET) {
////			Delay_ms(10);
////			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == Bit_SET) {
////				GPIO_Write(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
////				Delay_s(2);
////				GPIO_Write(GPIOB, ~(GPIO_Pin_10 | GPIO_Pin_11));
////			}
////			
////		}
////		GPIO_Write(GPIOB, GPIO_Pin_10 | (GPIO_Pin_11));
////		Delay_s(1);
////		GPIO_Write(GPIOB, ~(GPIO_Pin_10 | (GPIO_Pin_11)));
////		Delay_s(1);
////		GPIO_Write(GPIOB,0b0000000000000001);
//		for (uint8_t i = 0; i <= 2; i++) {
//			GPIO_Write(GPIOA, ~(0x0001 << i));
//			Delay_s(1);
//		}
//	}
//}
////#include "stm32f10x.h"
////#include <stdbool.h>

////volatile uint32_t sys_tick_ms = 0;   // 全局毫秒计数器

////// SysTick 中断服务函数（在 stm32f10x_it.c 中实现，或直接放在此文件）
//////void SysTick_Handler(void)
//////{
//////    sys_tick_ms++;
//////}

////// GPIO 初始化
////void GPIO_Config(void)
////{
////    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
////    
////    GPIO_InitTypeDef GPIO_InitStruct;
////    
////    // LED1 (PB10), LED2 (PB11) 推挽输出
////    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
////    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
////    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
////    GPIO_Init(GPIOB, &GPIO_InitStruct);
////    
////    // 按键 (PB9) 下拉输入
////    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
////    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
////    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
////    GPIO_Init(GPIOB, &GPIO_InitStruct);
////}

////int main(void)
////{
////    // 1. 系统时钟初始化（一般由 SystemInit 完成）
////    // 2. 配置 SysTick，周期 1ms
////    if (SysTick_Config(SystemCoreClock / 1000))
////    {
////        while(1);  // 初始化失败
////    }
////    
////    // 3. 初始化 GPIO
////    GPIO_Config();
////    
////    // 4. 状态变量
////    bool led1_state = false;          // LED1 亮灭状态
////    uint32_t led1_last_toggle = 0;    // 上次翻转时间
////    
////    bool key_pressed = false;         // 按键防抖标志
////    uint32_t key_last_time = 0;       // 按键上一次高电平时间
////    
////    while(1)
////    {
////        uint32_t now = sys_tick_ms;   // 当前时刻
////        
////        // ========== 任务1：LED1 以 500ms 周期闪烁（非阻塞） ==========
////        if ((now - led1_last_toggle) >= 500)
////        {
////            led1_last_toggle = now;
////            led1_state = !led1_state;
////            GPIO_WriteBit(GPIOB, GPIO_Pin_10, led1_state ? Bit_SET : Bit_RESET);
////        }
////        
////        // ========== 任务2：按键检测（非阻塞防抖） ==========
////        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == Bit_SET)
////        {
////            // 高电平持续超过 10ms 且未处理
////            if ((now - key_last_time) >= 10 && !key_pressed)
////            {
////                // 有效按键：翻转 LED2 (PB11)
////                GPIO_WriteBit(GPIOB, GPIO_Pin_11, 
////                              GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11) ? Bit_RESET : Bit_SET);
////                key_pressed = true;   // 标记已处理，防止重复触发
////            }
////        }
////        else
////        {
////            // 按键释放，重置标志
////            key_pressed = false;
////            key_last_time = now;      // 更新低电平时间（用于防抖）
////        }
////        
////        // ========== 其他任务可以继续添加 ==========
////    }
////}

#include "stm32f10x.h"                  // Device header

int main() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
}



