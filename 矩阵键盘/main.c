#include <REGX52.H>
#include <intrins.h>
#include "NixiePrint.h"
int MatrixInput();
void Delay(unsigned int ms);

void main(){
	unsigned int cnt=1,keynum=0;
	if (P3_0==0){
		Delay(10);
		while (P3_0==0);
		Delay(10);
		P0=0x00;
		
		while (1){
			MatrixInput();
			keynum=MatrixInput();
			
			if (cnt<=8) {
				print(cnt++,keynum);
			}
			if (cnt==9) {P0=0x00; while(1);}
		}
	}
}

int MatrixInput(){
	unsigned char num=0;
	P1=0xFF;
	P1_3=0;
	if (P1_7==0){Delay(10); while (P1_7==0); Delay(10); num=1;}
	if (P1_6==0){Delay(10); while (P1_6==0); Delay(10); num=5;}
	if (P1_5==0){Delay(10); while (P1_5==0); Delay(10); num=9;}
	
	P1_2=0;
	if (P1_6==0){Delay(10); while (P1_6==0); Delay(10); num=6;}
	if (P1_7==0){Delay(10); while (P1_7==0); Delay(10); num=2;}
	if (P1_5==0){Delay(10); while (P1_5==0); Delay(10); num=0;}
	
	P1_1=0;
	if (P1_6==0){Delay(10); while (P1_6==0); Delay(10); num=7;}
	if (P1_7==0){Delay(10); while (P1_7==0); Delay(10); num=3;}
	
	P1_0=0;
	if (P1_6==0){Delay(10); while (P1_6==0); Delay(10); num=8;}
	if (P1_7==0){Delay(10); while (P1_7==0); Delay(10); num=4;}
	
	return num;
}

void Delay(ms){//@11.0592MHz
	while (ms--){
		unsigned char data i, j;

		_nop_();
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}