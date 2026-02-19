#include "Inf_SC128.h"

#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_IO 2
#define I2C_MASTER_SCL_IO 1
#define I2C_MASTER_FREQ_HZ 100000

#define INT_PORT 0
#define ADDR (0x42)

void sc12b_touch_cb(void *arg);
void Inf_SC128_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t write_buf[2] = {reg, data};

    i2c_master_write_to_device(I2C_MASTER_NUM, ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);
}

uint8_t Inf_SC128_ReadReg(uint8_t reg)
{
    uint8_t data=0;
    i2c_master_write_read_device(I2C_MASTER_NUM, ADDR, &reg, 1, &data, 1, 1000 / portTICK_PERIOD_MS);
    return data;
}
void Inf_SC128_Init(void)
{
    //i2c初始化
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);

    //中断引脚开启
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1<<INT_PORT;
    //disable pull-down mode
    io_conf.pull_down_en = 1;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //install gpio isr service 只需一次
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(INT_PORT, sc12b_touch_cb, (void*) INT_PORT);

    //降低灵敏度：防止干扰
    Inf_SC128_WriteReg(0x00,0x25);
    Inf_SC128_WriteReg(0x01,0x25);
}

extern TaskHandle_t keyscanTaskHandle;

//中断回调函数
void sc12b_touch_cb(void *arg)
{
    if((int)arg==INT_PORT)
    {
        //esp_rom_printf("1\r\n");
        xTaskNotifyFromISR(keyscanTaskHandle,1,eSetValueWithOverwrite,NULL);         
    }
}

Touch_Key_t Inf_SC128_ReadTouchkey(void)
{
    if(xTaskNotifyWait(0,0,NULL,5000)!=pdTRUE)
    {
        return key_no;
    }

    uint8_t data1 =Inf_SC128_ReadReg(0x08);
    uint8_t data2 =Inf_SC128_ReadReg(0x09);
    uint16_t data =(data1<<4)|(data2>>4);

    Touch_Key_t key=key_no;
    
    switch (data)
    {
    case 1<<11: //通道0
        {
            key=key_0;
            break;
        }

    case 1<<10: //通道1
        {
            key=key_1;
            break;
        }

    case 1<<9: //通道2
        {
            key=key_2;
            break;
        }  

    case 1<<8: //通道3
        {
            key=key_3;
            break;
        }

    case 1<<7: //通道4
        {
            key=key_7;
            break;
        }  

    case 1<<6: //通道5
        {
            key=key_5;
            break;
        }

    case 1<<5: //通道6
        {
            key=key_6;
            break;
        }

    case 1<<4: //通道7
        {
            key=key_4;
            break;
        }

    case 1<<3: //通道8
        {
            key=key_M;
            break;
        }

    case 1<<2: //通道9
        {
            key=key_8;
            break;
        } 

    case 1<<1: //通道10
        {
            key=key_9;
            break;
        }

    case 1<<0: //通道11
        {
            key=key_sharp;
            break;
        }                          

    default:
        break;
    }
    

    return key;
}
