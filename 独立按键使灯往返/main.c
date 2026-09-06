#include <REGX52.H>
#include <intrins.h>
void Delay(unsigned int ms);
void main(){
	unsigned char cnt=1;
	while (1){
		if (P3_1==0){
			Delay(10);
			if (P3_1==0){
				while (P3_1==0);
				Delay(10);
			}
			switch (cnt) {
				case 1 :{
					unsigned char i;
					for (i=0;i<8;i++){
						P2=~(0x01<<i);
						Delay(150);
					}
					cnt=8;
					break;
				}
				case 8 :{
					unsigned char i;
					for (i=7;i>=0;i--){
						P2=~(0x01<<i);
						Delay(150);
					}
					cnt=1;
					break;
				}
			}
		}
	}
}
void Delay(unsigned int ms)	//@11.0592MHz
{
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