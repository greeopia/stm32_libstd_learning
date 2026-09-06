#include <REGX52.H>
#include "NixiePrint.h"

//extern unsigned char PWMCount;
//extern unsigned char ScanPos;
//extern unsigned char Brightness[8],DisplayChar[8];
/*void Timer0Init()
{
    TMOD &= 0xF0;
    TMOD |= 0x01;     //模式1

    TL0 = 0x18;
    TH0 = 0xFC;       //1ms

    TF0 = 0;
    TR0 = 1;

    ET0 = 1;
    EA = 1;
}*/

void Timer0_Init()
{
    TMOD &= 0xF0;
    TMOD |= 0x01;

    TH0 = 0xFE;
    TL0 = 0x0B;

    ET0 = 1;
    EA  = 1;
    TR0 = 1;
}



/*
void Timer0_ISR() interrupt 1
{
    PWMCount++;
    if(PWMCount > 50) PWMCount = 0;

    // 关所有位
    P2_4 = P2_3 = P2_2 = 1;

    // PWM 输出
    if(PWMCount < Brightness[ScanPos])
        print(ScanPos+1, DisplayChar[ScanPos]);
    else
        print(ScanPos+1, ' ');

    // 下一位
    ScanPos++;
    if(ScanPos >= 8) ScanPos = 0;
}
*/