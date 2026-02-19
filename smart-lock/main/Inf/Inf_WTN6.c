#include "Inf_WTN6.h"

#define DATA_PIN GPIO_NUM_9
#define DATA_H gpio_set_level(DATA_PIN, 1)
#define DATA_L gpio_set_level(DATA_PIN, 0)
void Inf_WTN6_Init(void)
{
    gpio_config_t config;
    config.pin_bit_mask = (1 << DATA_PIN);
    config.mode=GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

void Inf_WTN6_SendCmd(uint8_t cmd)
{
    DATA_H;
    vTaskDelay(10);
    DATA_L;
    vTaskDelay(5);

    for(int i=0;i<8;i++)
    {
        if(cmd&0x01)
        {
            DATA_H;
            usleep(600);
            DATA_L;
            usleep(200);
        }
        else{
            DATA_H;
            usleep(200);
            DATA_L; 
            usleep(600);
        }
        cmd>>=1;
    }
    DATA_H;
    vTaskDelay(5);
}