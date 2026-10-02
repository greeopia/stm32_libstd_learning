#include "stm32f10x.h"
#include "Motor.h"

/**
  * 函    数：电机初始化
  * 参    数：无
  * 返 回 值：无
  * 注意事项：PWM用TIM1_CH1，引脚固定为PA8，不需要重映射
  *           TIM1挂在APB2上，别和TIM2~TIM4（APB1）搞混
  *           方向引脚PB12~PB15上电全部拉低，电机保持不动
  */
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM1, ENABLE);
	
	/*PA8复用推挽输出，作为TIM1_CH1的PWM输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*PB12~PB15推挽输出，作为方向引脚，先全部拉低*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	// PB12->IN1, PB13->IN2, PB14->IN3, PB15->IN4
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	
	/*时基单元，72MHz/(0+1)/(3599+1)=20kHz*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = MOTOR_PWM_MAX - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	/*输出比较通道1，PWM1模式*/
	TIM_OCStructInit(&TIM_OCInitStructure);					//TIM1会读结构体里所有字段，先填默认值防止随机数
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = MOTOR_PWM_MAX / 5;		//20%占空比
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);		//改CCR1在下个周期生效，不会产生毛刺
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);						//高级定时器必须这一句，否则PA8没有波形
	TIM_Cmd(TIM1, ENABLE);
}

/**
  * 函    数：设置PWM占空比
  * 参    数：Duty 0~MOTOR_PWM_MAX，超出范围会被截断
  * 返 回 值：无
  */
void Motor_SetDuty(uint16_t Duty)
{
	if (Duty > MOTOR_PWM_MAX)
	{
		Duty = MOTOR_PWM_MAX;
	}
	
	TIM_SetCompare1(TIM1, Duty);
}

/**
  * 函    数：设置某个电机的转动方向
  * 参    数：Motor 选 MOTOR_A 或 MOTOR_B
  *           Dir 选 MOTOR_DIR_STOP / MOTOR_DIR_FORWARD / MOTOR_DIR_REVERSE
  * 返 回 值：无
  */
// typedef enum
// {
//     MOTOR_A = 0,
//     MOTOR_B
// } Motor_TypeDef;

void Motor_SetDir(uint8_t Motor, uint8_t Dir)
{
	uint16_t Pin1;
	uint16_t Pin2;
	
	if (Motor == MOTOR_A)
	{
		Pin1 = GPIO_Pin_12;			//IN1
		Pin2 = GPIO_Pin_13;			//IN2
	}
	else
	{
		Pin1 = GPIO_Pin_14;			//IN3
		Pin2 = GPIO_Pin_15;			//IN4
	}
	
	if (Dir == MOTOR_DIR_FORWARD)
	{
		GPIO_SetBits(GPIOB, Pin1);
		GPIO_ResetBits(GPIOB, Pin2);
	}
	else if (Dir == MOTOR_DIR_REVERSE)
	{
		GPIO_ResetBits(GPIOB, Pin1);
		GPIO_SetBits(GPIOB, Pin2);
	}
	else
	{
		GPIO_ResetBits(GPIOB, Pin1 | Pin2);
	}
}
