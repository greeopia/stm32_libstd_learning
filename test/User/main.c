#include "stm32f10x.h"                  // Device header
#include "Motor.h"

int main(void)
{
	Motor_Init();		//PA8输出20kHz、占空比20%的PWM，方向引脚全部拉低
	Motor_SetDir(MOTOR_A, MOTOR_DIR_FORWARD);	//让A电机转起来，不想让它转就删掉这一行
	Motor_SetDuty(MOTOR_PWM_MAX / 2);  // 改为50%占空比
	
	while (1)
	{
		
	}
} 
