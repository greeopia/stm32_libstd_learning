#include <REGX52.H>
#include <stdio.h>
void Delay20ms(void)	//@11.0592MHz
{
	unsigned char data i, j;
	i = 36;
	j = 217;
	do
	{
		while (--j);
	} while (--i);
}
void main(){
	while (1){
		if (P3_1==0){
			Delay20ms();
			while (P3_1==0);
			Delay20ms();
			
			P2_0=~P2_0;
		}
	}
} 