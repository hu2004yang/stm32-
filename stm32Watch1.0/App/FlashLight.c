#include "FlashLight.h"

void Show_FlashLight_UI(void)
{
    OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(20,20,"OFF",OLED_12X24);
	OLED_ShowString(72,20,"ON",OLED_12X24);
}

int FlashLight_Page(void)
{
    int FlashFlag=1; 
    int lastFlashFlag = -1;  // 添加变量跟踪上一次的FlashFlag
    while(1)
    {
        uint8_t FlashKey = Key_GetNum(); // 获取按键值
        uint8_t FlashFlag_temp=0;  // 初始化为0
        // 只有在有按键按下时才处理
        if(FlashKey != 0)
        {
            if(FlashKey==1)
            {
                FlashFlag--;
                if(FlashFlag<=0) FlashFlag=3;
            }
            else if(FlashKey==2)
            {
                FlashFlag++;
                if(FlashFlag>3) FlashFlag=1;
            }
            else if(FlashKey==3)
            {
                OLED_Clear();
                OLED_Update();
                FlashFlag_temp=FlashFlag;                
            }  
            
            if(FlashFlag_temp!=0)
            {
                if(FlashFlag_temp==1) 
                {
                    //OLED_Clear();
                    //OLED_Update();
                    return 0;
                }
                else if(FlashFlag_temp==2)
                {
                    LED_Off();
                    Delay_ms(10);
                    lastFlashFlag = -1;  // 强制下次循环显示UI
                }
                else if(FlashFlag_temp==3)
                {
                    LED_On();
                    Delay_ms(10);
                    lastFlashFlag = -1;  // 强制下次循环显示UI
                }
            }               
        }     
        
        // 只有当FlashFlag改变时才更新显示
        if (FlashFlag != lastFlashFlag)
        {
            switch (FlashFlag)
            {
            case 1:
                {
                    OLED_Clear();
                    Show_FlashLight_UI();
				    OLED_ReverseArea(0,0,16,16);
				    OLED_Update();
                }
            break;
            case 2:
                {
                    OLED_Clear();
                    Show_FlashLight_UI();				    
				    OLED_ReverseArea(20,20,36,24);
				    OLED_Update();
                }
            break;
            case 3:
                {
                    OLED_Clear();
                    Show_FlashLight_UI();
				    OLED_ReverseArea(72,20,24,24);
				    OLED_Update();
                }    
            break;
            default:
                break;
            }                
            lastFlashFlag = FlashFlag;  // 更新lastFlashFlag
        }
                         
        // 添加小延时，避免CPU占用过高
        Delay_ms(10);
    }
}
