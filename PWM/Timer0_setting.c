#include <REGX52.H>
#define counter 50000//想要记us数（0~5000）
extern double dt;//。。。MHz下一个机器周期为dt个us
extern unsigned int cnt0,cycle0;

void Timer0_init(){
	TMOD&=0xF0;
	TMOD|=0x01;
	
	TH0=cycle0/256;
	TL0=cycle0%256;
	TR0=1;//开始T0
	TF0=0;//中断（不自动清零）
	ET0=1;
	EA=1;
}