#include <REGX52.H>
#include <stdio.h>
#include "LCD1602.h"
#define counter 5000//想要记us数（0~5000）
void Timer0_ISR(void);
double dt=12*(1.0/11.0592);//11.0592MHz下一个机器周期为dt个us
volatile unsigned int cnt1=0,hour=0,min=0,s=0;
static unsigned int day=0;
void main(){
	unsigned int cycle=65536 - (unsigned int)(counter/(dt));
	Timer0_init();
	LCD_Init();
	LCD_ShowString(1,1,"Clock:");
	while(1){
		LCD_ShowString(2,9,":");
		LCD_ShowString(2,12,":");
		
		TF0=0;
		TH0=cycle/256;
		TL0=cycle%256;
		
		LCD_ShowNum(2,1,day,2);
		LCD_ShowNum(2,7,hour,2);
		LCD_ShowNum(2,10,min,2);
		LCD_ShowNum(2,13,s,2);
		if (day<2) LCD_ShowString(2,3,"Day");
		else LCD_ShowString(2,3,"Days");
	}
}
void Timer0_ISR(void) interrupt 1{
	cnt1++;
	if (cnt1>=200){
		cnt1=0;
		s++;
		if(s>=60){
			s=0;
			min++;
			if (min>=60){
				min=0;
				hour++;
				if (hour>=24){
					hour=0;
					day++;
					if (day==100)
						day=0;
					}
				}
			}
		}
	}