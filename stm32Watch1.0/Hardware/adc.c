#include "adc.h"


void ADC1_Init(void)
{
    
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_0;

    
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    ADC1->CR1 &= ~ADC_CR1_SCAN;

    ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 &= ~ADC_CR2_ALIGN;

 
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
    ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;

    
    ADC1->SQR1 &= ~ADC_SQR1_L;

    
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= 10 << 0;

    
}


void ADC1_StartConvert(void)
{
    
    ADC1->CR2 |= ADC_CR2_ADON;

   
    ADC1->CR2 |= ADC_CR2_CAL;
  
    while (ADC1->CR2 & ADC_CR2_CAL)
    {}

    
    ADC1->CR2 |= ADC_CR2_ADON;

    while ((ADC1->SR & ADC_SR_EOC) == 0)
    {}
}


double ADC1_ReadV(void)
{
    return ADC1->DR * 3.3 / 4095;
}
