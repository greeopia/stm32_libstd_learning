#include <REGX52.H>
extern unsigned char UARTnum;//baud在main.c里

void Timer1_init_UART(){
	SCON=0x50;
	TMOD&=0x0F;
	TMOD|=0x20;//0010 0000
	SM0=0;
	TH1=256-UARTnum;//存初值
	TL1=256-UARTnum;//写入初值
	TF1=0;//中断标志位
	ET1=0;
	
	EA=1;
	ES=1;
	TR1=1;
}
/*

void Uart1_Init(void)	//14400bps@11.0592MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xFE;			//设置定时初始值
	TH1 = 0xFE;			//设置定时重载值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
}
*/

/*
interrupt 3:定时器1
interrupt 4:UART串口Baud发生器
*/