#include "App_IO.h"

extern TaskHandle_t fingerscanTaskHandle;
extern TaskHandle_t otaTaskHandle;

/*存储数字对应的字符1=>'1*/
char InputBuffer[50]={0};
uint8_t InputBufferLen=0;
static Pwd_handler_state_t pwdState=CHECK_PWD;
void App_IO_Start(void)
{
    MY_LOGI("1.初始化LED");
    Inf_LED_Init();
    
    MY_LOGI("2.初始化触摸按键");
    Inf_SC128_Init();

    MY_LOGI("3.初始化语言");
    Inf_WTN6_Init();

    MY_LOGI("4.初始化NVS存储");
    Dri_NVS_Init();
    
    MY_LOGI("5.电机初始化");
    Inf_BDR6120_Init();

    MY_LOGI("6.初始化指纹模块");
    Inf_FPM385_Init();
}

void App_IO_Touchkey_Scan(void)
{
    /*
    定义输入状态机
    free：默认状态，有按键按下，激活进入input
    input:读取按键，sharp表示输入结束，进入complete,M表示非法，回到free
    complete：对输入进行处理，结束进入free    
    */
   Touch_Key_t key=Inf_SC128_ReadTouchkey();
   static Input_state_t state=FREE;

   if(key==key_no)
   {
    state=FREE;
    Inf_LED_LightAll(black);

    pwdState=CHECK_PWD;
    return; 
   }

   switch (state)
   {
   case FREE:/*激活键盘*/
   {
    Inf_LED_LightAll(white);
    memset(InputBuffer,0,sizeof(InputBuffer));
    InputBufferLen=0;
    state=INPUT;
    break;
   }    
    
   case INPUT:
   {
    sayWaterDrop();
    Inf_LED_Lightkeyled(key,blue);
    if(key==key_M)
    {
        sayIllegalOperation();
        state=FREE;
    }
    else if(key==key_sharp)
    {        
        //对输入进行处理
        App_IO_TaskHandler();
        if(pwdState==ADD_PWD||pwdState==DEL_PWD)
        {
            /*后面的输入是要添加或删除的密码*/
            memset(InputBuffer,0,sizeof(InputBuffer));
            InputBufferLen=0;
        }
        else
        {
            state=FREE;
            vTaskDelay(500);
            Inf_LED_LightAll(black);
        }
        
    }
    else 
    {
        InputBuffer[InputBufferLen++]=key+'0';
    }
    break;
   }   

   default:
    break;
   }
}

/*对输入做处理
命令 2个数字表示命令
     密码相关
     00 添加密码
     01 删除密码
     
     指纹相关
     10 添加指纹
     11 删除指纹

     ota相关
     20 升级固件

密码
     >=6 输入的是密码
 
错误
    <=1 || =3 || =4 || =5     

*/
void App_IO_TaskHandler(void)
{
    MY_LOGE("输入：%s",InputBuffer);    

    if(InputBufferLen<=1||InputBufferLen==3||InputBufferLen==4||InputBufferLen==5)
    {
        sayIllegalOperation();
    }
    else if(InputBufferLen==2)
    {
        if(InputBuffer[0]=='0')
        {
            if(InputBuffer[1]=='0')
            {
                sayAddUser();       
                vTaskDelay(1000);
                sayInputUserPassword();  
                
                pwdState=ADD_PWD;
            }
            else if(InputBuffer[1]=='1')
            {
                sayDelUser();
                vTaskDelay(1000);
                sayInputUserPassword(); 

                pwdState=DEL_PWD;
            }
            else
            {
                sayIllegalOperation();
            }
        }
        else if(InputBuffer[0]=='1')
        {
           xTaskNotify(fingerscanTaskHandle,(uint32_t)InputBuffer[1],eSetValueWithOverwrite);
        }
        else if(InputBuffer[0]=='2')
        {
            xTaskNotify(otaTaskHandle,(uint32_t)InputBuffer[1],eSetValueWithOverwrite);  
        }
        else
        {
            sayIllegalOperation();
        }
    }
    else
    {
        /*正常密码*/        
        if(pwdState==ADD_PWD)
        {
            /*添加密码*/
            MY_LOGE("添加密码：%s",InputBuffer);
            App_IO_ADD_PWD();
            pwdState=CHECK_PWD;
        }
        else if(pwdState==DEL_PWD)
        {
            /*删除密码*/
            MY_LOGE("删除密码：%s",InputBuffer);
            App_IO_DEL_PWD();
            pwdState=CHECK_PWD;
        }
        else if(pwdState==CHECK_PWD)
        {
            /*验证密码：开锁*/
            MY_LOGE("验证密码：%s",InputBuffer);
            App_IO_CHECK_PWD();
        }        
    }
}


/*密码存储
1.存储地
 非易失性存储设备
 nvs esp32芯片内部flash提供的一块空间
2.存储格式
*/
void App_IO_ADD_PWD(void)
{
    esp_err_t err=Dri_NVS_WriteI8(InputBuffer,0);
    if(err==ESP_OK)
    {
        MY_LOGE("密码:%s 添加成功",InputBuffer);
        sayPasswordAddSucc();
    }
    else{
        MY_LOGE("密码:%s 添加失败",InputBuffer);
        sayPasswordAddFail();
    }
}

void App_IO_DEL_PWD(void)
{
    esp_err_t err=Dri_NVS_Delkey(InputBuffer);
    if(err==ESP_OK)
    {
        MY_LOGE("密码:%s 删除成功",InputBuffer);
        sayPassword();
        sayDelSucc();
    }
    else{
        MY_LOGE("密码:%s 删除失败",InputBuffer);
        sayPassword();
        sayDelFail();
    }
}

void App_IO_CHECK_PWD(void)
{
    if(Dri_NVS_IskeyMatch(InputBuffer)==ESP_OK)
    {
        MY_LOGE("密码：%s 验证成功",InputBuffer);
        sayPasswordVerifySucc();
        Inf_BDR6120_OpenLock();
        sayDoorOpen();
    }
    else{
        MY_LOGE("密码：%s 验证失败",InputBuffer);
        sayPasswordVerifyFail();
    }
}

void App_IO_Finger_Scan(void)
{
    static uint8_t fingerState=FINGER_CHECK;
    uint32_t action=0;
    xTaskNotifyWait(0,0,&action,portMAX_DELAY);

    if(action=='0')
    {
        /*绿灯闪烁*/
        LED_GREEN_FLICKER;

        sayAddUserFingerprint();
        vTaskDelay(1500/portTICK_PERIOD_MS);
        sayPlaceFinger();
        fingerState=FINGER_ADD;
    }
    else if(action=='1')
    {
        /*红灯闪烁*/
        LED_RED_FLICKER;

        sayDelUserFingerprint();
        vTaskDelay(1500/portTICK_PERIOD_MS);
        sayPlaceFinger();
        fingerState=FINGER_DEL;
    }
    else if(action=='2')
    {
       switch (fingerState)
       {
       case FINGER_ADD:
       {
        MY_LOGE("录入指纹");     
        Inf_FPM385_AutoEnroll();
        LED_COLOR_OFF;
        break;
       }
       case FINGER_DEL:
       {
        MY_LOGE("删除指纹");
        Inf_FPM385_Delfinger();
        LED_COLOR_OFF;
        break;
       }
       case FINGER_CHECK:
       {
        LED_BLUE_ON;
        MY_LOGE("验证指纹");
        Inf_FPM385_AutoVerify();
        LED_COLOR_OFF;
        break;
       }
       default:
        break;
       }
       Inf_FPM385_Sleep();
       fingerState=FINGER_CHECK;
    }
}

void App_IO_nvs_Test(void)
{
    if(Dri_NVS_IskeyMatch("1234")==ESP_OK)
    {
        MY_LOGE("1234查找成功");
    }
    else{
        MY_LOGE("1234查找失败");
    }

    if(Dri_NVS_IskeyMatch("123456")==ESP_OK)
    {
        MY_LOGE("123456查找成功");
    }
    else{
        MY_LOGE("123456查找失败");
    }

    if(Dri_NVS_IskeyMatch("2212345633")==ESP_OK)
    {
        MY_LOGE("2212345633查找成功");
    }
    else{
        MY_LOGE("2212345633查找失败");
    }

    if(Dri_NVS_Delkey("123456")==ESP_OK)
    {
        MY_LOGE("123456删除成功");
    }
    else{
        MY_LOGE("123456删除失败");
    }

    if(Dri_NVS_IskeyMatch("123456")==ESP_OK)
    {
        MY_LOGE("123456查找成功");
    }
    else{
        MY_LOGE("123456查找失败");
    }
}