#include "stm32f10x.h"                  // Device header
#include <math.h>                       // fabsf
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "VQF_App.h"

/*采样率：100Hz，对应MPU6050的SMPLRT_DIV=0x09和DLPF=0x06配置（传感器输出率100Hz）*/
#define SAMPLE_RATE_HZ		100
/*每10个采样点刷新一次OLED和串口，也就是10Hz，避免刷新动作拖慢采样节拍*/
#define DISPLAY_DIV			10
/*前2秒只显示提示，给VQF留出静止零偏估计的时间*/
#define STARTUP_TICKS		(SAMPLE_RATE_HZ * 2)

int16_t AccRaw[3], GyrRaw[3];			//加速度计和陀螺仪的原始数据
uint16_t CostUs = 0, MaxCostUs = 0;		//单次采样+解算的耗时（微秒），用于评估CPU余量
float GyrLp[3];							//低通后的陀螺读数（度/秒），静止检测用
float RestGyrDev = 0.0f, RestAccDev = 0.0f;	//静止检测的两个相对偏差，都要小于1

/**
  * 函    数：定时器初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：TIM2产生100Hz的更新标志，主循环轮询这个标志来保证采样间隔稳定，
  *           不用中断，也不会有中断和主循环抢I2C的问题
  */
static void Timer2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//开启TIM2的时钟
	
	/*72MHz / (71+1) = 1MHz的计数频率，(9999+1)个计数 = 10ms，也就是100Hz*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);					//先清标志，避免初始化后立刻误判一次
	TIM_Cmd(TIM2, ENABLE);									//使能TIM2
}

/**
  * 函    数：计时用定时器初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：TIM3自由运行在1MHz，一个计数就是1微秒，用来测量每次处理的耗时，
  *           16位计数器约65.5ms回绕一次，单次处理远小于这个时间，直接相减即可
  */
static void Timer3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//开启TIM3的时钟
	
	/*72MHz / (71+1) = 1MHz的计数频率，也就是1个计数1微秒*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);									//使能TIM3，让它自由运行
}

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：USART1，PA9为TX，115200波特率，只发送不接收，
  *           用来把角度和零偏送到上位机记录、画图
  */
static void USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

/**
  * 函    数：串口发送一个字节
  */
static void USART1_SendByte(uint8_t Byte)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, Byte);
}

/**
  * 函    数：串口发送字符串
  */
static void USART1_SendString(char *String)
{
	while (*String != '\0')
	{
		USART1_SendByte(*String);
		String ++;
	}
}

/**
  * 函    数：串口发送无符号整数
  */
static void USART1_SendUInt(uint32_t Number)
{
	char Buffer[10];
	uint8_t i = 0;
	
	do
	{
		Buffer[i] = '0' + (Number % 10);
		Number /= 10;
		i ++;
	} while (Number > 0 && i < 10);
	
	while (i > 0)
	{
		i --;
		USART1_SendByte(Buffer[i]);
	}
}

/**
  * 函    数：串口发送浮点数
  * 参    数：Value 要发送的数，Decimals 小数位数，范围0~2
  * 返 回 值：无
  * 说    明：不用printf，避免半主机模式和重定向带来的麻烦
  */
static void USART1_SendFloat(float Value, uint8_t Decimals)
{
	int32_t Scale = 1;
	int32_t Scaled;
	int32_t Fraction, Divider;
	uint8_t i;
	
	for (i = 0; i < Decimals; i ++)
	{
		Scale *= 10;
	}
	
	Scaled = (int32_t)(Value * (float)Scale + (Value >= 0.0f ? 0.5f : -0.5f));
	if (Scaled < 0)
	{
		USART1_SendByte('-');
		Scaled = -Scaled;
	}
	
	USART1_SendUInt((uint32_t)(Scaled / Scale));
	
	if (Decimals > 0)
	{
		USART1_SendByte('.');
		Fraction = Scaled % Scale;
		Divider = Scale / 10;
		while (Divider > 0)
		{
			USART1_SendByte('0' + (Fraction / Divider) % 10);
			Divider /= 10;
		}
	}
}

/**
  * 函    数：主函数
  * 说    明：MPU6050 -> VQF 6D姿态解算 -> OLED/串口输出
  */
int main(void)
{
	uint16_t Tick = 0, DisplayTick = 0;
	uint16_t TimeStart;
	uint8_t ID, Rest;
	float Roll = 0.0f, Pitch = 0.0f, Yaw = 0.0f;
	float Bias[3] = {0.0f, 0.0f, 0.0f};
	float BiasSigma = 0.0f;
	float MaxGyrLp;
	
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	MPU6050_Init();		//MPU6050初始化
	Timer2_Init();		//采样节拍初始化
	Timer3_Init();		//计时用定时器初始化
	USART1_Init();		//串口初始化
	
	/*显示ID号，确认I2C通信正常*/
	OLED_ShowString(1, 1, "VQF6D  ID:");
	ID = MPU6050_GetID();
	OLED_ShowHexNum(1, 10, ID, 2);
//	OLED_ShowString(4, 1, "Keep still 2s");
	
	/*VQF初始化，采样率必须和定时器节拍一致，否则姿态会漂*/
	VQF_App_Init(SAMPLE_RATE_HZ);
	
	USART1_SendString("\r\nVQF 6D start, 100Hz\r\n");
	USART1_SendString("R,P,Y unit: degree\r\n");
	
	while (1)
	{
		/*等待10ms节拍，保证VQF的积分时间等于真实采样间隔*/
		if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == RESET)
		{
			continue;
		}
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		
		TimeStart = TIM3->CNT;					//开始计时
		
		/*一次连续读14字节，加速度和角速度来自同一帧*/
		MPU6050_GetDataBurst(&AccRaw[0], &AccRaw[1], &AccRaw[2], &GyrRaw[0], &GyrRaw[1], &GyrRaw[2]);
		
		/*喂给VQF，内部完成LSB到rad/s、m/s²的换算*/
		VQF_App_Update(AccRaw, GyrRaw);
		
		Tick ++;
		DisplayTick ++;
		
		if (DisplayTick >= DISPLAY_DIV)			//10Hz刷新一次显示和串口
		{
			DisplayTick = 0;
			
			VQF_App_GetEuler(&Roll, &Pitch, &Yaw);
			BiasSigma = VQF_App_GetBiasDps(Bias);
			Rest = VQF_App_IsRestDetected();
			VQF_App_GetRestDebug(GyrLp, &RestGyrDev, &RestAccDev);
			
			/*取三个轴里绝对值最大的那个，这是决定Rest能不能成立的量*/
			MaxGyrLp = GyrLp[0];
			if (fabsf(GyrLp[1]) > fabsf(MaxGyrLp)) { MaxGyrLp = GyrLp[1]; }
			if (fabsf(GyrLp[2]) > fabsf(MaxGyrLp)) { MaxGyrLp = GyrLp[2]; }
			
//			OLED_Clear();
			if (Tick >= STARTUP_TICKS)			//前2秒保持提示，让VQF先完成零偏估计
			{
				/*OLED显示，角度放大了10倍显示，也就是最后一位是小数*/
				OLED_ShowString(1, 1, "Roll: ");
				OLED_ShowSignedNum(1, 7, (int32_t)(Roll * 10.0f), 5);
				OLED_ShowString(2, 1, "Pitch:");
				OLED_ShowSignedNum(2, 7, (int32_t)(Pitch * 10.0f), 5);
				OLED_ShowString(3, 1, "Yaw:  ");
				OLED_ShowSignedNum(3, 7, (int32_t)(Yaw * 10.0f), 5);
				/*第四行：R=Rest，S=零偏不确定度×100，G=低通后陀螺读数最大分量的绝对值×10
				  G这一栏如果一直大于biasClip×10（默认20），Rest就永远不会变成1*/
				OLED_ShowString(4, 1, "R:");
				OLED_ShowNum(4, 3, Rest, 1);
				OLED_ShowString(4, 5, "S:");
				OLED_ShowNum(4, 7, (uint32_t)(BiasSigma * 100.0f), 2);
				OLED_ShowString(4, 10, "G:");
				OLED_ShowSignedNum(4, 12, (int32_t)(MaxGyrLp * 10.0f), 4);
			}
			
			/*串口输出，格式：R=.. P=.. Y=.. BX=.. BY=.. BZ=.. SIG=.. RST=.*/
			USART1_SendString("R=");
			USART1_SendFloat(Roll, 1);
			USART1_SendString(" P=");
			USART1_SendFloat(Pitch, 1);
			USART1_SendString(" Y=");
			USART1_SendFloat(Yaw, 1);
			USART1_SendString(" BX=");
			USART1_SendFloat(Bias[0], 2);
			USART1_SendString(" BY=");
			USART1_SendFloat(Bias[1], 2);
			USART1_SendString(" BZ=");
			USART1_SendFloat(Bias[2], 2);
			USART1_SendString(" SIG=");
			USART1_SendFloat(BiasSigma, 2);
			USART1_SendString(" RST=");
			USART1_SendUInt(Rest);
			USART1_SendString(" CLIP=");
			USART1_SendFloat(VQF_App_GetBiasClip(), 1);
			USART1_SendString(" DEV=");
			USART1_SendFloat(RestGyrDev, 2);		//陀螺偏差/阈值，必须<1
			USART1_SendString("/");
			USART1_SendFloat(RestAccDev, 2);		//加速度偏差/阈值，必须<1
			USART1_SendString(" GLP=");
			USART1_SendFloat(GyrLp[0], 2);
			USART1_SendString(",");
			USART1_SendFloat(GyrLp[1], 2);
			USART1_SendString(",");
			USART1_SendFloat(GyrLp[2], 2);
			USART1_SendString(" US=");
			USART1_SendUInt(CostUs);
			USART1_SendString(" MAXUS=");
			USART1_SendUInt(MaxCostUs);
			USART1_SendString("\r\n");
			
			MaxCostUs = 0;						//每输出一次就重置最大值，方便观察负载波动
		}
		
		/*统计本轮循环的总耗时（含OLED和串口的刷新），串口输出的US和MAXUS因此滞后一轮显示周期。
		  如果MAXUS长期接近10000微秒，说明10ms的采样节拍顶不住了，需要提高优化等级、
		  提高I2C速率或者降低显示刷新频率*/
		CostUs = (uint16_t)(TIM3->CNT - TimeStart);
		if (CostUs > MaxCostUs)
		{
			MaxCostUs = CostUs;
		}
	}
}
