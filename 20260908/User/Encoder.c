#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "LED.h"

#define ENCODER_SAMPLE_HZ        1000U
#define ENCODER_STEPS_PER_DETENT 4
#define BUTTON_DEBOUNCE_MS       20U

extern LEDState_t LEDState;
extern volatile int16_t cnt1, cnt2;

static uint8_t Encoder_ReadAB(void)
{
	uint8_t state = 0;

	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_SET)
	{
		state |= 0x02;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_SET)
	{
		state |= 0x01;
	}

	return state;
}

void EncoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	uint32_t timerClock;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_GetClocksFreq(&RCC_Clocks);
	timerClock = RCC_Clocks.PCLK1_Frequency;
	if (RCC_Clocks.PCLK1_Frequency != RCC_Clocks.HCLK_Frequency)
	{
		timerClock *= 2U;
	}

	/* TIM2 interrupts every 1 ms. */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = (uint16_t)(timerClock / 1000000U - 1U);
	TIM_TimeBaseInitStructure.TIM_Period = (uint16_t)(1000000U / ENCODER_SAMPLE_HZ - 1U);
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
	/* Index is previous AB in bits 3:2 and current AB in bits 1:0. */
	static const int8_t transitionTable[16] = {
		 0, -1,  1,  0,
		 1,  0,  0, -1,
		-1,  0,  0,  1,
		 0,  1, -1,  0
	};
	static uint8_t previousAB = 3;
	static int8_t movement = 0;
	static uint8_t buttonStable = 1;
	static uint8_t buttonCandidate = 1;
	static uint8_t buttonTicks = 0;
	uint8_t currentAB;
	uint8_t previous;
	uint8_t buttonNow;
	int8_t step;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET)
	{
		return;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	previous = previousAB;
	currentAB = Encoder_ReadAB();
	step = transitionTable[(previous << 2) | currentAB];
	previousAB = currentAB;

	if (step != 0)
	{
		movement += step;
		if (movement >= ENCODER_STEPS_PER_DETENT)
		{
			++cnt1;
			LEDState = LEFT;
			movement = 0;
		}
		else if (movement <= -ENCODER_STEPS_PER_DETENT)
		{
			--cnt1;
			LEDState = RIGHT;
			movement = 0;
		}
	}
	else if (currentAB != previous)
	{
		/* Both bits changed at once: reject this noisy/invalid jump. */
		movement = 0;
	}

	/* PB12 is pulled high and becomes low while the button is pressed. */
	buttonNow = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_SET) ? 1U : 0U;
	if (buttonNow != buttonCandidate)
	{
		buttonCandidate = buttonNow;
		buttonTicks = 0;
	}
	else if (buttonTicks < BUTTON_DEBOUNCE_MS)
	{
		++buttonTicks;
		if ((buttonTicks == BUTTON_DEBOUNCE_MS) && (buttonStable != buttonCandidate))
		{
			buttonStable = buttonCandidate;
			if (buttonStable == 0U)
			{
				++cnt2;
				LEDState = IDLE;
			}
		}
	}
}

void NVIC_Config(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
