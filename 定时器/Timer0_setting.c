#include <REGX52.H>
#define counter 5000//想要记us数（0~5000）
extern double dt;//。。。MHz下一个机器周期为dt个us
extern unsigned int cnt0, hour, min;
extern unsigned int cycle0;

void Timer0_init(){
	TMOD&=0xF0;
	TMOD|=0x01;
	
	TH0=cycle0/256;
	TL0=cycle0%256;
	TR0=1;
	TF0=0;
	ET0=1;
	EA=1;
}