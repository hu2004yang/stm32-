/*
 * @Author: wushengran
 * @Date: 2024-06-11 15:02:52
 * @Description: 
 * 
 * Copyright (c) 2024 by atguigu, All Rights Reserved. 
 */
#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"


void ADC1_Init(void);


void ADC1_StartConvert(void);


double ADC1_ReadV(void);

#endif
