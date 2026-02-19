#include "Inf_BDR6120.h"

#define BDR_A_PIN 4
#define BDR_B_PIN 5
void Inf_BDR6120_Init(void)
{
    gpio_config_t config;
    config.mode=GPIO_MODE_OUTPUT;
    config.pin_bit_mask=(1<<BDR_A_PIN)|(1<<BDR_B_PIN);
    config.intr_type=GPIO_INTR_DISABLE;
    config.pull_down_en=GPIO_PULLDOWN_DISABLE;
    config.pull_up_en=GPIO_PULLUP_DISABLE;

    gpio_config(&config);

    gpio_set_level(BDR_A_PIN,0);
    gpio_set_level(BDR_B_PIN,0);
}

void Inf_BDR6120_Forward(void)
{
    gpio_set_level(BDR_A_PIN,1);
    gpio_set_level(BDR_B_PIN,0);
}

void Inf_BDR6120_Backward(void)
{
    gpio_set_level(BDR_A_PIN,0);
    gpio_set_level(BDR_B_PIN,1);
}

void Inf_BDR6120_Brake(void)
{
    gpio_set_level(BDR_A_PIN,1);
    gpio_set_level(BDR_B_PIN,1);
}

void Inf_BDR6120_Ready(void)
{
    gpio_set_level(BDR_A_PIN,0);
    gpio_set_level(BDR_B_PIN,0);
}

void Inf_BDR6120_OpenLock(void)
{
    /*
    正转1s
    反转1s
    待命1s
    */
   Inf_BDR6120_Forward();
   vTaskDelay(1000/portTICK_PERIOD_MS);
   Inf_BDR6120_Backward();
   vTaskDelay(1000/portTICK_PERIOD_MS);
   Inf_BDR6120_Ready();
}

void Inf_BDR6120_CloseLock(void)
{
    /*
    反转1s
    正转1s
    刹车1s
    */
   Inf_BDR6120_Backward();
   vTaskDelay(1000/portTICK_PERIOD_MS);
   Inf_BDR6120_Forward();
   vTaskDelay(1000/portTICK_PERIOD_MS);
   Inf_BDR6120_Brake();
}