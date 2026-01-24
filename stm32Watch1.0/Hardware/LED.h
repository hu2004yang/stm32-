#ifndef  __LED_H
#define  __LED_H

#include "stm32f10x.h"

//宏定义
#define LED1 GPIO_ODR_ODR15

//初始化
void LED_Init(void);

//控制某个LED的开关
void LED_On(void);
void LED_Off(void);

//翻转LED状态
void LED_Toggle(void);

#endif
