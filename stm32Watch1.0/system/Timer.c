#include "Timer.h"                // Device header
#include "StopWatch.h"
#include "game.h"

/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;			//开启TIM2的时钟
	
	/*配置时钟源*/
	//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/	
	TIM2->CR1=0;
	TIM2->CR1 |=TIM_CR1_CKD_0;//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM2->CR1 &=~TIM_CR1_DIR;	//计数器模式，选择向上计数
	TIM2->ARR = 100 - 1;				//计数周期，即ARR的值
	TIM2->PSC = 720 - 1;				//预分频器，即PSC的值
	//重复计数器，高级定时器才会用到	
	
	/*中断输出配置*/
	TIM2->SR &=~TIM_SR_UIF;					//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM2->DIER |=TIM_DIER_UIE;					//开启TIM2的更新中断
	
	/*NVIC中断分组*/
	SCB->AIRCR = (SCB->AIRCR & ~SCB_AIRCR_PRIGROUP_Msk) | (0x5 << SCB_AIRCR_PRIGROUP_Pos);  // 配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC->IP[28] = (2 << 4) | 1;  // 设置优先级：抢占优先级为2，响应优先级为1
	NVIC->ISER[28 / 32] |= (1 << (28 % 32));  // 使能TIM2中断
	
	/*TIM使能*/
	TIM2->CR1 |=TIM_CR1_CEN;			// 启动计数器
}

// 定时器中断函数，可以复制到使用它的地方
void TIM2_IRQHandler(void)
{
	if(TIM2->SR&TIM_SR_UIF)		//检查指定的中断发生与否：TIM 中断源
	{
		TIM2->SR &= ~TIM_SR_UIF;    //清除中断标志位
		
		//在此处添加定时器中断处理代码		
		Key_Tick();	
		StopWatch_Tick();
		Dino_Tick();
	}
}
