#include <REGX52.H>
#include "Timer0_init_PWM.h"
#include "NixiePrint.h"

unsigned char DisplayChar[8];     // 8 位实际显示内容
unsigned char Brightness[8];      // 每位 PWM 亮度 0~50

unsigned char PWMCount = 0;       // 0~50
unsigned char ScanPos = 0;        // 当前扫描位

volatile unsigned char SysTick = 0;  // 1ms 时基标志，由定时器置 1

// 正弦亮度表 0~50，64 点一个完整波浪周期（存 ROM，省 RAM）
unsigned char code SIN_TABLE[64] =
{
    25,27,30,32,35,37,39,41,
    43,44,46,47,48,49,50,50,
    50,50,50,49,48,47,46,44,
    43,41,39,37,35,32,30,27,
    25,23,20,18,15,13,11, 9,
     7, 6, 4, 3, 2, 1, 0, 0,
     0, 0, 0, 1, 2, 3, 4, 6,
     7, 9,11,13,15,18,20,23
};

unsigned int  WavePhase = 0;     // 波浪相位 0~63
unsigned int  ScrollTimer = 0;   // 文字移位计时

#define WAVE_STEP_MS  2          // 相位步进间隔(ms)：2ms×64 = 128ms 一个波浪
#define SCROLL_MS     200        // 文字每 200ms 左移一位

char String[] = "Ciallo~";
unsigned char StringIndex = 0;    // 当前取的字符
unsigned char StringLen = sizeof(String)-1;

//==================== Timer0 = 每 500us 扫描一位 ====================
void Timer0_ISR() interrupt 1
{
    static unsigned char halfMs = 0;

    // 500us 重载值 (11.0592 MHz)
    TH0 = 0xFE;
    TL0 = 0x0B;

    //----------------------------------
    // ★ 消影：关全部位选 + 清段码
    //----------------------------------
    P2_4 = 1;
    P2_3 = 1;
    P2_2 = 1;
    P0 = 0x00;

    // PWM 计数
    PWMCount++;
    if(PWMCount > 50) PWMCount = 0;

    //----------------------------------
    // ★ PWM 控制显示
    //----------------------------------
    if(PWMCount < Brightness[ScanPos])
    {
        // 亮
        print(ScanPos+1, DisplayChar[ScanPos]);
    }
    else
    {
        // 灭（段与位已经在最前面清掉）
    }

    // 下一位
    ScanPos++;
    if(ScanPos >= 8) ScanPos = 0;

    // 1ms 时基：每 2 次 500us 中断置一次标志
    if(++halfMs >= 2)
    {
        halfMs = 0;
        SysTick = 1;
    }
}

//==================== 浪潮亮度 + 文字移位 ====================
void WaveUpdate(void)
{
    static unsigned int stepTick = 0;
    unsigned char i;

    // 波浪相位前进（每 WAVE_STEP_MS 走一步）
    if(++stepTick >= WAVE_STEP_MS)
    {
        stepTick = 0;
        WavePhase++;
        if(WavePhase >= 64) WavePhase = 0;
    }

    // 8 位亮度 = 正弦波在 8 位上的采样，随相位整体平移 → 浪潮般的明暗流动
    for(i=0;i<8;i++)
    {
        Brightness[i] = SIN_TABLE[(WavePhase + i*8) & 0x3F];
    }

    // 文字左移流水
    if(++ScrollTimer >= SCROLL_MS)
    {
        ScrollTimer = 0;
        for(i=0;i<7;i++)
            DisplayChar[i] = DisplayChar[i+1];
        DisplayChar[7] = String[StringIndex];
        StringIndex++;
        if(StringIndex >= StringLen) StringIndex = 0;
    }
}


//======================== main =========================================================

void main()
{
    unsigned char i;

    // 初始填满字符串，亮度从 0 开始由波浪带起
    for(i=0;i<8;i++)
    {
        DisplayChar[i] = String[i % StringLen];
        Brightness[i] = 0;
    }
    StringIndex = 0;

    Timer0_Init();

    while(1)
    {
        if(SysTick)
        {
            SysTick = 0;
            WaveUpdate();
        }
    }
}