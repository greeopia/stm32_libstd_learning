#include <REGX52.H>
#include <stdlib.h>
#include <intrins.h>
#include "Timer0_setting.h"
#include "NixiePrint.h"

void Timer0_ISR();
void print_PWM(unsigned char location,character,speed);//渐亮效果
void Timer1_ISR();//计时1s

#define cnt0 50000//一个周期多少us. for Timer0.
//此时200次1s

double dt=12.0/11.0592;//***核心！！！
unsigned int cycle0,cycle1;
unsigned char string[]="Ciallo~",num = sizeof(string)-1,s=1;
unsigned int counter0=0,counter1=0;
volatile unsigned char flag=0;//0表示正常，1表示需要移位。

void main(){
	cycle0=65536-(unsigned int)(cnt0/dt);
	cycle1=256-(unsigned int)(200/dt);//200us一个T1中断
	Timer0_init();
	TMOD&=0x0F;
	TMOD|=0x20;//T1 
	TR1=1;
	TF1=0;
	
	unsigned int i,j;
	while (1){
		
	}
}

void print_PWM(unsigned char location,character,speed){
	unsigned char i,j;
	for (i=0;i<num;i++){
		for (j=0;j<speed;j++){
			print(i,string[i]);
			//延时递增
		}
		for (j=speed;j>0;j--){
			print(i,string[i]);
			//延时递减
		}
		if (flag==1) {
			flag=0;
		}
		
	}	
}

void Timer0_ISR() interrupt 1{
	TH0=cycle0/256;
	TL0=cycle0%256;
	TF0=0;
	
	counter0++;
	
	if (counter0>=200) {
		counter0=0;
		flag=1;
	}
}

void Timer1_ISR() interrupt 3{//计时 + 提供数码管移位
	TH1=cycle1;//存初值
	TL1=cycle1;//写入初值
	counter1++;
	if (counter1>=1000000/200){
		s++;//初始s=1
		counter1=0;
	}
	
	TF1=0;
}
