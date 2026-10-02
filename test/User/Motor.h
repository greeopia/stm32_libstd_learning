#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

/*PWM满量程，等于ARR+1，对应72MHz下的20kHz*/
#define MOTOR_PWM_MAX		3600

/*电机编号*/
#define MOTOR_A				0		//IN1=PB12  IN2=PB13
#define MOTOR_B				1		//IN3=PB14  IN4=PB15

/*转动方向*/
#define MOTOR_DIR_STOP		0
#define MOTOR_DIR_FORWARD	1
#define MOTOR_DIR_REVERSE	2

void Motor_Init(void);
void Motor_SetDuty(uint16_t Duty);
void Motor_SetDir(uint8_t Motor, uint8_t Dir);

#endif
