#include "tim.h"
#include "stm32f10x.h"                  // Device header
#include "usart.h"

static uint8_t IsLeapYear(uint16_t year) {
	return (uint8_t)(((year % 4 == 0) && (year % 100 != 0)) ||
		(year % 400 == 0));
}

static uint8_t GetMonthDays(uint16_t year, uint8_t month) {
	static const uint8_t monthDays[12] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	if ((month == 2) && IsLeapYear(year)) {
		return 29;
	}

	return monthDays[month - 1];
}

void TIM1Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_InternalClockConfig(TIM1); //设为内部时钟
	
	TIM_TimeBaseInitTypeDef TimeBaseInitstructure;
	TimeBaseInitstructure.TIM_ClockDivision = TIM_CKD_DIV4;
	TimeBaseInitstructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInitstructure.TIM_Period = 10000 - 1;
	TimeBaseInitstructure.TIM_Prescaler = 7200 - 1;
	TimeBaseInitstructure.TIM_RepetitionCounter = 0; // 1s计数
	TIM_TimeBaseInit(TIM1, &TimeBaseInitstructure);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void) {
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		Nowtime.s++;
		if (Nowtime.s >= 60) {
			Nowtime.s = 0;
			Nowtime.min++;
			if (Nowtime.min >= 60) {
				Nowtime.min = 0;
				Nowtime.hour++;
				if (Nowtime.hour >= 24) {
					Nowtime.hour = 0;

					if (Daytime_Valid != 0) {
						Nowtime.day++;
						if (Nowtime.day > GetMonthDays(Nowtime.year, Nowtime.month)) {
							Nowtime.day = 1;
							Nowtime.month++;
							if (Nowtime.month > 12) {
								Nowtime.month = 1;
								Nowtime.year++;
							}
						}
					}
				}
			}
		}

		if (Daytime_Valid != 0) {
			Daytime_DisplayDirty = 1;
		}
	}
}

void NVIC_Config(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
