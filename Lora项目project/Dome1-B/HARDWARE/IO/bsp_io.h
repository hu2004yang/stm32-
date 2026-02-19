#ifndef __BSP_IO_H
#define	__BSP_IO_H


#include "stm32f10x.h"


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态


/***********************************蜂鸣器**********************************************/
#define BEEP_GPIO_PORT    	GPIOA		            /* GPIO端口 */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define BEEP_GPIO_PIN		    GPIO_Pin_12			        /* 连接到SCL时钟线的GPIO */
/* 定义控制IO的宏 */
#define BEEP_TOGGLE		 	digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF		   		digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON			digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
void Beep_Init(void);






#endif /* __BSP_IO_H*/
