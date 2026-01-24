#include "key.h"

uint8_t Key_Num;

void Key_Init(void)
{
    // 启用GPIOB和GPIOA时钟 (APB2总线)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN;
    
    // 配置PB1为上拉输入
    GPIOB->CRL &= ~(0xF << (1*4));      // 清除原有配置
    GPIOB->CRL |= (0x8 << (1*4));       // 上拉输入模式(CNF=10, MODE=00)
    GPIOB->ODR |= (1 << 1);             // 启用上拉
    
    // 配置PA6和PA4为上拉输入
    GPIOA->CRL &= ~(0xF << (6*4));      // 清除PA6原有配置
    GPIOA->CRL |= (0x8 << (6*4));       // 上拉输入模式
    GPIOA->ODR |= (1 << 6);             // 启用上拉
    GPIOA->CRL &= ~(0xF << (4*4));      // 清除PA4原有配置
    GPIOA->CRL |= (0x8 << (4*4));       // 上拉输入模式
    GPIOA->ODR |= (1 << 4);             // 启用上拉
}

uint8_t Key_GetNum(void)
{
    uint8_t Temp;
    if(Key_Num)
    {
        Temp = Key_Num;
        Key_Num = 0;
        return Temp;
    }
    else
    {
        return 0;
    }
}

uint8_t Key_GetState(void)
{
    if (!(GPIOB->IDR & GPIO_IDR_IDR1))  // 读取PB1状态
    {
        return 1;
    }
    if (!(GPIOA->IDR & GPIO_IDR_IDR6))  // 读取PA6状态
    {
        return 2;
    }
    if (!(GPIOA->IDR & GPIO_IDR_IDR4))  // 读取PA4状态
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

void Key_Tick(void)
{
    static uint8_t Count;
    static uint8_t CurrentState, PreState;
    Count++;
    if(Count >= 100)
    {
        Count = 0;
        PreState = CurrentState;
        CurrentState = Key_GetState();
        if(PreState != 0 && CurrentState == 0)
        {
            Key_Num = PreState;
        }
    }
}
