#include "usart.h"
#include "stm32f10x.h"                  // Device header
#include "cJSON.h"
#include <string.h>

#define USART1_RX_BUFFER_SIZE 64

volatile Daytime_t Nowtime = {0,0,0,0,0,0};
volatile uint8_t Daytime_Valid = 0;
volatile uint8_t Daytime_DisplayDirty = 0;

static char USART1_RxBuffer[USART1_RX_BUFFER_SIZE];
static volatile uint16_t USART1_RxIndex = 0;
static volatile uint8_t USART1_FrameReady = 0;
static volatile uint8_t USART1_RxOverflow = 0;

void USART1_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(USART_TypeDef* USARTx, uint8_t byte) { // 阻塞式发送
	USART_SendData(USARTx, byte);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

static uint8_t ParseTwoDigits(const char *text, uint8_t *value) {
	if ((text[0] < '0') || (text[0] > '9') ||
		(text[1] < '0') || (text[1] > '9')) {
		return 0;
	}

	*value = (uint8_t)((text[0] - '0') * 10 + (text[1] - '0'));
	return 1;
}

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

static uint8_t ParseDateTimeValue(const char *value, Daytime_t *parsedTime) {
	uint8_t yearHigh;
	uint8_t yearLow;

	/* 解析后的固定格式：2026-09-11<真正的换行>18:30:35 */
	if ((strlen(value) != 19) ||
		(value[4] != '-') || (value[7] != '-') ||
		(value[10] != '\n') ||
		(value[13] != ':') || (value[16] != ':')) {
		return 0;
	}

	if (!ParseTwoDigits(value, &yearHigh) ||
		!ParseTwoDigits(value + 2, &yearLow) ||
		!ParseTwoDigits(value + 5, &parsedTime->month) ||
		!ParseTwoDigits(value + 8, &parsedTime->day) ||
		!ParseTwoDigits(value + 11, &parsedTime->hour) ||
		!ParseTwoDigits(value + 14, &parsedTime->min) ||
		!ParseTwoDigits(value + 17, &parsedTime->s)) {
		return 0;
	}

	parsedTime->year = (uint16_t)yearHigh * 100 + yearLow;

	if ((parsedTime->month < 1) || (parsedTime->month > 12) ||
		(parsedTime->day < 1) ||
		(parsedTime->day > GetMonthDays(parsedTime->year, parsedTime->month)) ||
		(parsedTime->hour > 23) ||
		(parsedTime->min > 59) ||
		(parsedTime->s > 59)) {
		return 0;
	}

	return 1;
}

static void ApplyTimeCalibration(const Daytime_t *calibratedTime) {
	uint32_t primask;

	/*
	 * Nowtime 有多个成员，不能让 TIM1 在赋值到一半时插入。
	 * 临界区只包含成员复制和定时器复位，持续时间很短。
	 */
	primask = __get_PRIMASK();
	__disable_irq();

	Nowtime.year = calibratedTime->year;
	Nowtime.month = calibratedTime->month;
	Nowtime.day = calibratedTime->day;
	Nowtime.hour = calibratedTime->hour;
	Nowtime.min = calibratedTime->min;
	Nowtime.s = calibratedTime->s;

	/* 从校准时刻重新开始完整的一秒，避免刚校准就立刻加 1。 */
	TIM_SetCounter(TIM1, 0);
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	Daytime_Valid = 1;
	Daytime_DisplayDirty = 1;

	if (primask == 0) {
		__enable_irq();
	}
}

uint8_t Daytime_TakeDisplaySnapshot(Daytime_t *snapshot) {
	uint32_t primask;

	primask = __get_PRIMASK();
	__disable_irq();

	if ((Daytime_Valid == 0) || (Daytime_DisplayDirty == 0)) {
		if (primask == 0) {
			__enable_irq();
		}
		return 0;
	}

	snapshot->year = Nowtime.year;
	snapshot->month = Nowtime.month;
	snapshot->day = Nowtime.day;
	snapshot->hour = Nowtime.hour;
	snapshot->min = Nowtime.min;
	snapshot->s = Nowtime.s;
	Daytime_DisplayDirty = 0;

	if (primask == 0) {
		__enable_irq();
	}

	return 1;
}

/* 收到完整且合法的 JSON 时，用它校准 Nowtime。 */
uint8_t USART1_TryCalibrateTime(void) {
	cJSON *root;
	cJSON *datetimeItem;
	Daytime_t calibratedTime;
	uint8_t result = 0;

	if (USART1_FrameReady == 0) {
		return 0;
	}

	root = cJSON_Parse(USART1_RxBuffer);
	if (root != NULL) {
		datetimeItem = cJSON_GetObjectItemCaseSensitive(root, "datetime");

		if (cJSON_IsString(datetimeItem) && (datetimeItem->valuestring != NULL)) {
			if (ParseDateTimeValue(datetimeItem->valuestring, &calibratedTime)) {
				ApplyTimeCalibration(&calibratedTime);
				result = 1;
			}
		}

		cJSON_Delete(root);
	}

	/* 解析完成后再允许中断接收下一帧。 */
	USART1_RxIndex = 0;
	USART1_RxOverflow = 0;
	USART1_FrameReady = 0;

	return result;
}

void USART1_IRQHandler(void) {
	uint8_t data;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		/* 读取 DR 会清除 RXNE，无需手动 ClearITPendingBit。 */
		data = (uint8_t)USART_ReceiveData(USART1);

		if (USART1_FrameReady != 0) {
			return;
		}

		if (data == '\n') {
			if (USART1_RxOverflow != 0) {
				USART1_RxIndex = 0;
				USART1_RxOverflow = 0;
				return;
			}

			/* Python 用 \r\n 作为一帧的结尾，这里去掉末尾的 \r。 */
			if ((USART1_RxIndex > 0) &&
				(USART1_RxBuffer[USART1_RxIndex - 1] == '\r')) {
				USART1_RxIndex--;
			}

			USART1_RxBuffer[USART1_RxIndex] = '\0';
			USART1_FrameReady = 1;
		}
		else if (USART1_RxOverflow == 0) {
			if (USART1_RxIndex < (USART1_RX_BUFFER_SIZE - 1)) {
				USART1_RxBuffer[USART1_RxIndex++] = (char)data;
			}
			else {
				/* 当前帧过长：忽略余下字符，直到收到帧尾 \n。 */
				USART1_RxOverflow = 1;
			}
		}
	}
}
