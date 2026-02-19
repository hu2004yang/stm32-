#include "Inf_FPM385.h"

#define TXD_PIN 21
#define RXD_PIN 20   
#define TOUCH_OUT 10
void fpm385_cb(void *args);

void Inf_FPM385_Init(void)
{
    /*串口驱动*/
    const uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, 1024* 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    /*中断引脚驱动*/
    //中断引脚开启
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1<<TOUCH_OUT;
    //disable pull-down mode
    io_conf.pull_down_en = 1;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //install gpio isr service 只需安装一次
    //gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(TOUCH_OUT,fpm385_cb, (void*) TOUCH_OUT);

    /*
    模组一启动默认没有进入休眠
    1.先禁用中断
    2.让模组进入休眠
    休眠后启用中断
    */
   gpio_intr_disable(TOUCH_OUT);
   Inf_FPM385_Sleep();
   Inf_FPM385_ReadID();
}

extern TaskHandle_t fingerscanTaskHandle;
void fpm385_cb(void *args)
{
    if((int)args==TOUCH_OUT)
    {
        xTaskNotifyFromISR(fingerscanTaskHandle,(uint32_t)'2',eSetValueWithOverwrite,NULL);
        /*禁用中断*/
        
        gpio_intr_disable(TOUCH_OUT);
    }
}

esp_err_t Inf_FPM385_SendCmd(uint8_t *buff,uint16_t len)
{
    /*通过串口把数据发送出去*/
    int bytesLen=uart_write_bytes(UART_NUM_1, buff, len);
    return bytesLen==-1?ESP_FAIL:ESP_OK;
}

/*读数据函数*/
static uint8_t rxBuff[256];
esp_err_t Inf_FPM385_RecvData(uint16_t len,uint32_t timeout)
{
    int rxBytes =uart_read_bytes(UART_NUM_1,rxBuff, len, timeout / portTICK_PERIOD_MS);
    return rxBytes==len?ESP_OK:ESP_FAIL;
}

void Inf_FPM385_ReadID(void)
{
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x04,
        0x34,//指令码：1个字节
        0x00,//参数：0~N个字节
        0x00,//校验和：2个字节
        0x39
    };

    /*发送指令*/
    Inf_FPM385_SendCmd(cmd,sizeof(cmd));
    if(Inf_FPM385_RecvData(44,5000)==ESP_OK)
    {
        PrintRecvData("ID",44);
        printf("%.*s\n",32,&rxBuff[10]);
    }
    else{
        esp_rom_printf("读ID失败\r\n");
    }
}



/*让模块进入休眠*/
void Inf_FPM385_Sleep(void)
{
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x03,
        0x33,//指令码：1个字节        
        0x00,//校验和：2个字节
        0x37
    };
    esp_err_t err=ESP_FAIL;
    do{
        Inf_FPM385_SendCmd(cmd,sizeof(cmd));
        err=Inf_FPM385_RecvData(12,5000);
        PrintRecvData("Sleep",12);
    }while (!(err==ESP_OK&&rxBuff[9]==0x00)); 
    
    gpio_intr_enable(TOUCH_OUT);
}

void Inf_FPM385_CheckSum(uint8_t cmd[],uint16_t len)
{
    uint16_t sum=0;
    for(size_t i=6;i<len-2;i++)
    {
        sum+=cmd[i];
    }
    cmd[len-2]=sum>>8;
    cmd[len-1]=sum;
}
/*
取消自动注册指令和自动验证指令
*/
void Inf_FPM385_CancelAuto(void)
{
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x03,
        0x30,//指令码：1个字节        
        '\0',//校验和：2个字节
        '\0'
    };
    Inf_FPM385_CheckSum(cmd,sizeof(cmd));
    do{
        Inf_FPM385_SendCmd(cmd,sizeof(cmd));
        Inf_FPM385_RecvData(12,5000);
        PrintRecvData("CancelAuto ",12);

        //收到数据 并且是成功
        if(rxBuff[6]!=0x00&&rxBuff[9]==0x00) break;;
    }while (1); 
}

int16_t Inf_FPM385_GetMINID(void)
{
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x04,
        0x1f,//指令码：1个字节    
        0x00,//页码：0    
        '\0',//校验和：2个字节
        '\0'
    };
    Inf_FPM385_CheckSum(cmd,sizeof(cmd));

    Inf_FPM385_SendCmd(cmd,sizeof(cmd));
    Inf_FPM385_RecvData(44,5000);
    
    PrintRecvData("GetMINID",44);

    int16_t id=0;
    //遍历索引信息的32个字节的每一位，找到第一个0
    for(size_t i=10;i<42;i++)
    {
        uint8_t b=rxBuff[i];
        for(size_t j=0;j<8;j++) //从最低位开始遍历
        {
            if((b&&0x01)==0)
            {
                //计算id
                id=j+(i-10)*8;
                return id;
            }
            b >>= 1;
        }       
    }
    return -1;
}

/*
一站式指纹注册
*/
void Inf_FPM385_AutoEnroll(void)
{  
    Inf_FPM385_CancelAuto();
    Inf_FPM385_CancelAuto();
    Inf_FPM385_CancelAuto();
    Inf_FPM385_CancelAuto();

    uint16_t id=Inf_FPM385_GetMINID();
    MY_LOGE("id:%d",id);
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x08,       
        0x31,//指令码：1个字节     
        id>>8,//id:2个字节
        id,
        0x02,//录入次数
        '\0',//参数
        '\0',    
        '\0',//校验和：2个字节
        '\0'
    };
    /*
        0b  11 1011=>0x3B
        bit0: 1 获取图像后成功后灯灭
        bit1: 1 打开预处理
        bit2: 0 返回关键步骤
        bit3: 1 id不允许覆盖
        bit4: 1 不允许重复注册
        bit5: 1 采集过程中，手指不可以离开 
    */
   cmd[13]=0x3b>>8;
   cmd[14]=0x3b;
   Inf_FPM385_CheckSum(cmd,sizeof(cmd));
   Inf_FPM385_SendCmd(cmd,sizeof(cmd));

   while (1)
   {
    Inf_FPM385_RecvData(14,5000);
    PrintRecvData("AutoEnroll",14);  
    
    uint8_t packId=rxBuff[6];
    uint8_t code=rxBuff[9];
    uint8_t p1=rxBuff[10];
    uint8_t p2=rxBuff[11];

    if(packId==0x00) continue;;
        if(code!=0x00){
            //录入失败
            MY_LOGD("指纹录入失败：%x",code);
            sayFingerprintAddFail();
            break;
        }
        else if(p1==0x06&&p2==0xf2)
        {
            MY_LOGD("指纹录入成功");
            sayFingerprintAddSucc();
            break;
        }       
   }
   Inf_FPM385_CancelAuto();
   Inf_FPM385_CancelAuto();
   Inf_FPM385_CancelAuto();
   Inf_FPM385_CancelAuto();   
}

/*
一站式验证
*/
void Inf_FPM385_AutoVerify(void)
{
    Inf_FPM385_CancelAuto();
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x08,       
        0x32,//指令码：1个字节    
        0x03,//分数等级 
        0xff,//id:2个字节
        0xff,
        0x00,//参数
        0x03,    
        '\0',//校验和：2个字节
        '\0'
    };
    /*
        011
        bit0: 1 获取图像后成功后灯灭
        bit1: 1 打开预处理
        bit2: 0 返回关键步骤        
    */
    Inf_FPM385_CheckSum(cmd,sizeof(cmd));
    Inf_FPM385_SendCmd(cmd,sizeof(cmd));
    while(1)
    {
        Inf_FPM385_RecvData(17,5000);
        PrintRecvData("AutoVerify",17);

        if(rxBuff[6]==0x00) continue;

        uint8_t code=rxBuff[9];
        uint8_t p=rxBuff[10];
        uint16_t id=rxBuff[12]|(rxBuff[11]<<8);

        if(code!=0x00)
        {
            //验证失败
            MY_LOGD("指纹验证失败：%x",code);
            sayFingerprintVerifyFail(); 
            break;           
        }
        else if(p==0x05)
        {
            MY_LOGD("指纹验证成功");
            sayFingerprintVerifySucc();  
            vTaskDelay(1500/portTICK_PERIOD_MS);            
            Inf_BDR6120_OpenLock();   
            sayDoorOpen();
            break;      
        }               
    }
    Inf_FPM385_CancelAuto();
}  


uint16_t Inf_FPM385_GetFingerId(void)
{
    Inf_FPM385_CancelAuto();
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x08,       
        0x32,//指令码：1个字节    
        0x03,//分数等级 
        0xff,//id:2个字节
        0xff,
        0x00,//参数
        0x03,    
        '\0',//校验和：2个字节
        '\0'
    };
    /*
        011
        bit0: 1 获取图像后成功后灯灭
        bit1: 1 打开预处理
        bit2: 0 返回关键步骤        
    */
    Inf_FPM385_CheckSum(cmd,sizeof(cmd));
    Inf_FPM385_SendCmd(cmd,sizeof(cmd));
    uint16_t id=0xffff;
    while(1)
    {
        Inf_FPM385_RecvData(17,5000);
        PrintRecvData("AutoVerify",17);

        if(rxBuff[6]==0x00) continue;

        uint8_t code=rxBuff[9];
        uint8_t p=rxBuff[10];      
        
        if(code!=0x00)
        {
            break;
        }
        else if(p==0x05)
        {
            id=rxBuff[12]|(rxBuff[11]<<8);
            break;
        }
                    
    }
    Inf_FPM385_CancelAuto();
    return id;
}
/*
删除指纹
*/
void Inf_FPM385_Delfinger(void)
{
    /*获取id*/
    uint16_t id=Inf_FPM385_GetFingerId();
    MY_LOGE("要删除的指纹Id:%d",id);
    if(id==0xffff)
    {
        MY_LOGE("没有指纹");
        MY_LOGD("删除失败");
        sayDelUserFingerprint();
        vTaskDelay(1300/portTICK_PERIOD_MS);
        sayDelFail();
    }    

    /*从指定的Id删除一个指纹*/
    uint8_t cmd[]=
    {
        0xef,//包头:2个字节
        0x01,
        0xff,//设备地址：4个字节
        0xff,
        0xff,
        0xff,
        0x01,//包标识：1个字节
        0x00,//包长度：2个字节
        0x07,       
        0x0c,//指令码：1个字节            
        id>>8,//页码:2个字节 要删除的指纹Id
        id,
        0x00,//删除个数
        0x01,    
        '\0',//校验和：2个字节
        '\0'
    };
    Inf_FPM385_CheckSum(cmd,sizeof(cmd));
    
    while(1)
    {
        Inf_FPM385_SendCmd(cmd,sizeof(cmd));
        Inf_FPM385_RecvData(12,5000);
        PrintRecvData("Delfinger",12);
        if(rxBuff[9]==0x00)
        {
            MY_LOGD("删除成功");
            sayDelUserFingerprint();
            vTaskDelay(1300/portTICK_PERIOD_MS);
            sayDelSucc();
            break;
        }
        else
        {
            MY_LOGD("删除失败");
            sayDelUserFingerprint();
            vTaskDelay(1300/portTICK_PERIOD_MS);
            sayDelFail();
            break;
        }
    }
    
}

/**
 * @description: 呼吸灯控制
 * @param {uint8_t} fun
 *      1-普通呼吸灯，2-闪烁灯，3-常开灯，4-常闭灯，5-渐开灯，6-渐闭灯
 * @param {uint8_t} startColor
 *      设置为普通呼吸灯时，由灭到亮的颜色，只限于普通呼吸灯（功能码 01）功能，其他功能时，与结束颜色保持一致。
 *      其中，bit0 是蓝灯控制位；bit1 是绿灯控制位；bit2 是红灯控制位。
 *      置 1 灯亮，置 0 灯灭。
 *      例如 0x01_蓝灯亮，
 *           0x02_绿灯亮，
 *           0x04_红灯亮，
 *           0x06_红绿灯亮，
 *           0x05_红蓝灯亮，
 *           0x03_绿蓝灯亮，
 *           0x07_红绿蓝灯亮，
 *           0x00_全灭；
 * @param {uint8_t} endColor
 * @param {uint8_t} cycle
 *              表示呼吸或者闪烁灯的次数。
 *                  当设为 0 时，表示无限循环，
 *                  当设为其他值时，表示呼吸有限次数。
 *              循环次数适用于呼吸、闪烁功能，其他功能中无效，
 *                  例如在常开、常闭、开和渐闭中是无效的；
 * @return {*}
 */
void Inf_FPM385_LedControl(uint8_t fun, uint8_t startColor, uint8_t endColor, uint8_t cycle)
{
    uint8_t cmd[] = {
        0xEF,
        0x01,   // 包头
        0xFF,
        0xFF,
        0xFF,
        0xFF,   // 默认地址
        0x01,   // 包标识
        0x00,
        0x07,   // 包长度
        0x3C,   // 指令码
        '\0',   // 功能码占位符
        '\0',   // 起始颜色占位符
        '\0',   // 结束颜色占位符
        '\0',   // 循环次数占位符
        '\0',
        '\0'   // 校验和占位符
    };

    cmd[10] = fun;
    cmd[11] = startColor;
    cmd[12] = endColor;
    cmd[13] = cycle;

    Inf_FPM385_CheckSum(cmd, sizeof(cmd));

    Inf_FPM385_SendCmd(cmd, sizeof(cmd));
    Inf_FPM385_RecvData (12, portMAX_DELAY);
}


void PrintRecvData(char *pre,uint16_t len)
{
    printf("%s:",pre);
    for(size_t i=0;i<len;i++)
    {
        printf("%02x ",rxBuff[i]);
    }
    printf("\n");
}