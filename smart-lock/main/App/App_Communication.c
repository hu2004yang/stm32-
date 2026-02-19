#include "App_Communication.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

extern char InputBuffer[50];
void App_Communication_start(void)
{
    MY_LOGE("Ble start");
    Dri_Ble_Init();
}

void App_Communication_RecvData(uint8_t *data,uint16_t len)
{
    MY_LOGE("收到手机发来的数据：%.*s", len, data);
    /*
    1：       开锁
    2：123456 设置密码
    3：34567  删除密码
    */
   if(len<2) return;
   if(data[0]=='1')
   {
        Inf_BDR6120_OpenLock();
        sayDoorOpen();
   }
   else if(data[1]=='2')
   {
        /*蓝牙传输的密码copy到inputbuff中*/
        memset(InputBuffer,0,sizeof(InputBuffer));
        memcpy(InputBuffer,data+2,len-2);
        App_IO_ADD_PWD();
   }
   else if(data[1]=='3')
   {
        memset(InputBuffer,0,sizeof(InputBuffer));
        memcpy(InputBuffer,data+2,len-2);
        App_IO_DEL_PWD();
   }
}

char *TAG="ota";
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

void ota(void *args)
{
     esp_http_client_config_t config = {
        .url ="http://192.168.1.54:8080/build/smart-lock.bin" ,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);//开始ota
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();//重启芯片
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    
}


void wifi_succ_cb(void)
{
     MY_LOGE("wifi连接成功");
     xTaskCreate(ota, "ota", 4096, NULL, 5, NULL);
}

void wifi_fail_cb(void)
{
     MY_LOGE("wifi连接失败");
} 

void App_Communication_OTA(void)
{
     uint32_t action=0;
     xTaskNotifyWait(0,0,&action,portMAX_DELAY);
     
     if(action=='0')
     {
          MY_LOGE("开始ota升级");
          /*初始化wifi*/
          Dri_Wifi_Init(wifi_succ_cb,wifi_fail_cb);
     }
     
}