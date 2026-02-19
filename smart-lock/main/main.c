#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_flash_mmap.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "App_IO.h"
#include "App_Communication.h"

TaskHandle_t keyscanTaskHandle;
void keyScanTask(void *args);
TaskHandle_t fingerscanTaskHandle;
void fingerScanTask(void *args);
TaskHandle_t otaTaskHandle;
void otaTask(void *args);
void app_main(void)
{
    //printf("智能门锁v2.0\n");

    //ESP_LOGE(tag,"智能门锁 v1.0");//error
    ESP_LOGW(tag,"智能门锁 v1.0");//warning
    ESP_LOGI(tag,"智能门锁 v1.0");//info
    ESP_LOGD(tag,"智能门锁 v1.0");//debug
    ESP_LOGV(tag,"智能门锁 v1.0");//verbose 
    
    MY_LOGE("智能门锁 v2.0");    
    App_IO_Start();
    App_Communication_start();    

    //创建按键扫描任务
    xTaskCreate(keyScanTask, "keyScanTask", 4096, NULL, 5, &keyscanTaskHandle);   
    
    //创建指纹扫描任务
    xTaskCreate(fingerScanTask, "fingerScanTask", 4096, NULL, 5, &fingerscanTaskHandle); 

    //创建一个ota任务
    xTaskCreate(otaTask, "otaTask", 4096, NULL, 5, &otaTaskHandle); 


    return;
}

void keyScanTask(void *args)
{
    MY_LOGE("按键扫描任务开始调度");
    while(1)
    {
        App_IO_Touchkey_Scan();
    }
}

void fingerScanTask(void *args)
{
    MY_LOGE("指纹扫描任务开始调度");
    while(1)
    {
        App_IO_Finger_Scan();
    }
}

void otaTask(void *args)
{
    MY_LOGE("OTA任务开始调度");
    while(1)
    {
        App_Communication_OTA();
    }
}