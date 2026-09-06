#include "stm32f10x.h"  // Device header，标准外设库只需要包含这一个头文件

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 

/**
 * @brief  简单的阻塞延时
 * @note   i 用 volatile 声明，防止编译器（当前工程 -O1）把空循环优化掉。
 *         计数约 100 万次：72MHz 时约 10~15ms，8MHz(HSI) 时约 100ms，
 *         肉眼都能明显看到闪烁。
 */
static void Delay(uint32_t count)
{
    volatile uint32_t i;
    for (i = 0; i < count; i++);
}
void GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ,BitAction bits);

static void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // 打开 GPIOC 时钟
	// SYSCLK ---> HCLK ---> PCLK2 -> GPIOC // 时钟树的一支
	//        AHB       APB2

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;      // 推挽输出
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main(void)
{
    LED_Init();

    while (1)
    {
        // 注意：这类最小系统板/蓝丸上的 LED 一般是低电平点亮（另一端接 3.3V）
        //GPIO_ResetBits(GPIOC, GPIO_Pin_13);  // PC13 输出低电平 -> LED 亮
		GPIO_WritePin(GPIOC,GPIO_Pin_13,Bit_SET);
        Delay(2000000);

        //GPIO_SetBits(GPIOC, GPIO_Pin_13);    // PC13 输出高电平 -> LED 灭
		GPIO_WritePin(GPIOC,GPIO_Pin_13,Bit_RESET);
        Delay(2000000);
    }
}
void GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ,BitAction bits) {
	/* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));
	// 其实GPIOx的Pin只有低16位有用（小端，32位）
	if (bits == Bit_SET) GPIOx->BSRR = GPIO_Pin;
	else if (bits == Bit_RESET) /*GPIOx->BRR = GPIO_Pin;*/  GPIOx->BSRR = GPIO_Pin << 16u;
	else return;
}
