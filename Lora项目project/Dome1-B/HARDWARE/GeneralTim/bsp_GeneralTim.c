#include "bsp_GeneralTim.h"
#include "bsp_esp8266_test.h"
#include "bsp_led.h"
#include "delay.h"
#include "string.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h" 




void TIM3_Init(u16 psc, u16 arr)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
		/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = arr;	
	 // 时钟预分频数为
  TIM_TimeBaseStructure.TIM_Prescaler= psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数	
	  // 初始化定时器
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		// 清除计数器中断标志位
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		// 开启计数器中断
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
		// 设置中断来源
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;	
		// 设置主优先级为 0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // 设置抢占优先级为3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	// 使能计数器
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  SG90舵机控制函数
  * @param  flag: 0-关闭舵机(0度), 1-打开舵机(90度)
  * @retval 无
  * @note   使用TIM3的CH1通道输出PWM控制SG90舵机
  *         SG90控制脉冲: 0.5ms-0度, 1.5ms-90度, 2.5ms-180度
  */
void sg90(int flag)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	// 使能TIM3和GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// 配置PA6为TIM3的CH1复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// TIM3时基配置 (72MHz / 72 = 1MHz, 1us一个计数)
	// 周期20ms = 20000us
	TIM_TimeBaseStructure.TIM_Period = 19999;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// PWM1模式配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;  // 默认0度
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	// 使能TIM3
	TIM_Cmd(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);

	if(flag == 0)
	{
		// 0.5ms脉冲 (0度)
		TIM_SetCompare1(TIM3, 500);
	}
	else if(flag == 1)
	{
		// 1.5ms脉冲 (90度)
		TIM_SetCompare1(TIM3, 1500);
	}
}


/*********************************************END OF FILE**********************/
