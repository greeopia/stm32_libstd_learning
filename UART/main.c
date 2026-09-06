#include <REGX52.H>
#include <stdio.h>
#include <intrins.h>
#include "Timer1_setting_UART.h"
#include "Timer0_setting.h"
#include "LCD1602.h"

#define Baud 14400//Timer0的单位时间
#define counter 5000//Timer1的单位时间us                                                                                          
double dt=12.0/11.0592;
unsigned int cycle0 = 65536 - (unsigned int)(5000.0 * 11.0592 / 12.0);  // Timer0初值：5ms定时
unsigned int cnt0,s;//给Timer0定时
unsigned int UARTnum=11059200/(32*12*Baud);//Timer1的TH1,TL1
unsigned int Column=0;
unsigned char flag=0;//定时器转换检测位：0为Timer0。1为Timer1。
int Byte=1;
void UART_ISR();
void SendByte(int Byte);

/*
Timer0:
	TMOD&=0xF0;
	TMOD|=0x01;
	标志位TF0

Timer1(UART):
	TMOD&=0x0F;
	TMOD|=0x20;
	标志位TF1
*/

void Delay1000ms(void)	//@11.0592MHz
{
	unsigned char data i, j, k;

	_nop_();
	i = 8;
	j = 1;
	k = 243;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void main(){
int flag1=0;
	Timer0_init();//先计时
	LCD_Init();
	while (1) 
		{
	flag1=Column-2;
	LCD_ShowString(2,flag1,"  ");
	LCD_ShowString(1,1,"RogerBytes:");
	LCD_ShowSignedNum(2,Column,Byte,3);	

Delay1000ms();
	Byte++;
	SendByte(Byte);

		
			
			//    if (flag==0) Timer0_init();
		}
	
	



}

void SendByte(int Byte){
	TR1=1;
	SBUF=Byte++;
	
	if (Byte==100) Byte=0;
	while (TI==0);
	TI=0;
	TR1=0;
	Timer0_init();
}

/*
void UART_ISR(void) interrupt 4{//***1，提供精准的Baud

	
	
	TH1=256-UARTnum;
	TL1=256-UARTnum;
	TF1=0;


	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=cycle0/256;
	TL0=cycle0%256;
	
	TF0=0;
	TR0=1;

}
*/

/*
void Timer0_ISR(void) interrupt 1{//***0,提供时间
	cnt0++;
	if (cnt0>=1000000/counter){
		cnt0=0;
		s++;
		flag=1;
		
	}
	TH0=cycle0/256;
	TL0=cycle0%256;

}
*/

void Timer0_ISR(void) interrupt 1 {
    // 先重装初值（保证定时准确）
    TH0 = cycle0 / 256;
    TL0 = cycle0 % 256;
    
    cnt0++;
    
    // 修正计算方式：避免整数除法截断
    // counter = 5000us = 5ms
    // 每200次中断为1秒：1000ms / 5ms = 200
    if (cnt0 >= (1000 * 1000) / counter)
		{  // 更精确的计算
        cnt0 = 0;
        s++;
	Column+=1;
	if (Column==17) Column=0;
		
    }
}
