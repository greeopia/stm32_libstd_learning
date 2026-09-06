#include <REGX52.H>
void Delay(unsigned int ms);	//@11.0592MHz
void main(){
	unsigned int cnt=0;
	while (1){
		if (P3_1==0){
			Delay(20);
			while (P3_1==0);
			Delay(20);
			
			P2=(0xFF>>cnt++);
			if (cnt==9) cnt=0;
		}
	}
}

void Delay(unsigned int ms){
	unsigned char data i, j;
	while (ms--){
		i = 36;
		j = 217;
		do
		{
			while (--j);
		} while (--i);
	}
}