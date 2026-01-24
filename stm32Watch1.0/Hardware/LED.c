#include "LED.h"

//初始化
void LED_Init(void)
{
    //1.时钟配置		
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN; 
	
	//2.GPIO工作模式配置
    GPIOB->CRH &= ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15);
	GPIOB->CRH &=~GPIO_CRH_CNF15;	
	GPIOB->CRH |=GPIO_CRH_MODE15;	
	
	//3.PB15输出高电平,默认关灯
    LED_Off();
}

//控制某个LED的开关
void LED_On(void)
{
    GPIOB->ODR &=~GPIO_ODR_ODR15;
}

void LED_Off(void)
{
    GPIOB->ODR |=GPIO_ODR_ODR15;
}

//翻转LED状态
void LED_Toggle(void)
{
    //需要先判断当前LED状态，读取IDR对应位
    if((GPIOB->IDR &GPIO_ODR_ODR15)==0)
    {
        LED_Off();
    }
    else{
        LED_On();
    }
}   
