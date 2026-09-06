#include <REGX52.H>
#include <intrins.h>
#include "NixiePrint.h"
void Delay(unsigned int ms);
void Delay500us();

void main(){
	unsigned char string[]="Ciallo~";
	unsigned int time,i=0,j=0,k,num=sizeof(string)-1;
	
	for (j=0;j<num;j++){
		for (time=0;time<100;time++){
			for (i=0;i<20;i++){
				print(j+1,' ');
				Delay(time);
				print(j+1,string[j]);
				Delay(50-time);
			}
		}
		for (time=50;time>=0;time--){
			for (i=0;i<5;i++){
				print(j+1,' ');
				Delay(time);
				print(j+1,string[j]);
				Delay(50-time);
			}
		}
	}
	if (j==num) {
		while (1){
			for (k=0;k<num;k++){
				print(k+1,string[k]);
				Delay500us();
			}
		}
	}
}
void Delay(unsigned int ms)	//@11.0592MHz
{
	while (ms--);
}
void Delay500us(void)	//@11.0592MHz
{
	unsigned char data i;

	_nop_();
	i = 227;
	while (--i);
}