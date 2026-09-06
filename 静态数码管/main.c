#include <REGX52.H>
#include <intrins.h>
#include "NixiePrint.h"
void Delay (unsigned int ms);
void Delay500us();
void main(){
	unsigned char cnt=0,string[]="Ciallo~",num = sizeof(string)-1;
	unsigned int i=0,j=0;
	
	if (!P3_1){
		Delay(10);
		while (!P3_1);
		Delay (10);
		P0=0xFF;
		
		for (i=0;string[i]!='\0';i++){
			print (i+1,string[i]);
			if (i+1<num) Delay(200);
			else{
				Delay (200);
				P0=0x00;
				Delay (300);
				break;
			}	
		}
		while (1){
			for (j=0;j<num;j++){
				print (j+1,string[j]);
				Delay500us();
			}
		}
	}
}
void Delay(unsigned int ms) {	//@11.0592MHz	
	while (ms--){
		unsigned char data i, j;

		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}
void Delay500us(void)	//@11.0592MHz
{
	unsigned char data i, j;

	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);
}