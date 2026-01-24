#include "menu.h"
void menu_Init(void)
{
	OLED_Init();
	OLED_Clear();
	MyRTC_Init();
	Key_Init();
	LED_Init();
	MPU6050_Init();
	ADC1_Init();
	ADC1_StartConvert();
}

uint16_t AD_value;
int Battery_Capacity;
//电池UI
void Show_Battery_UI(void)
{
	int sum;
	for(int i=0;i<3000;i++)
	{
		AD_value=ADC1_ReadV();
		sum+=AD_value;
		
	}
	AD_value=sum/3000;
	Battery_Capacity=(AD_value-3276)*100/819;
	if(Battery_Capacity<0)Battery_Capacity=0;
	OLED_ShowNum(85,4,Battery_Capacity,3,OLED_6X8);
	OLED_ShowChar(103,4,'%',OLED_6X8);

	if(Battery_Capacity==100)OLED_ShowImage(110,0,16,16,Battery);
	else if(Battery_Capacity>=10&&Battery_Capacity<100)
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea((112+Battery_Capacity/10),5,(10-Battery_Capacity/10),6);
		OLED_ClearArea(85,4,6,8);
	}
	
	else
	{
		OLED_ShowImage(110,0,16,16,Battery);
		OLED_ClearArea(112,5,10,6);
		OLED_ClearArea(85,4,12,8);
	}
}

//时钟UI
void Show_Clock_UI(void)
{
	OLED_Clear();
	MyRTC_ReadTime();
	Show_Battery_UI();
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);
}


int First_Page_Clock(void)
{
	static int ClkFlag=1; 
    while(1)
    {
        uint8_t currentKey = Key_GetNum(); // 获取按键值
        
        // 只有在有按键按下时才处理
        if(currentKey != 0)
        {
            if(currentKey==1)
            {
                ClkFlag--;
                if(ClkFlag<=0) ClkFlag=2;
            }
            else if(currentKey==2)
            {
                ClkFlag++;
                if(ClkFlag>2) ClkFlag=1;
            }
            else if(currentKey==3)
            {
                OLED_Clear();
                OLED_Update();
                return ClkFlag;
            }         
           
        }        
             
        switch (ClkFlag)
        {
            case 1:
                {
                    Show_Clock_UI();
                    OLED_ReverseArea(0,48,32,16);
                    OLED_Update();
                }
            break;
            case 2:
                {
                    Show_Clock_UI();
                    OLED_ReverseArea(96,48,32,16);
                    OLED_Update();
                }
            break;
            default:
                break;
        }          
                
        // 添加小延时，避免CPU占用过高
        for(volatile int i=0; i<1000; i++);
    }
}

//设置UI
void Show_Setting_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"日期时间设置",OLED_8X16);
}


int Setting_Page(void)
{	
	static int SetFlag=1;
	while (1)
	{
		uint8_t currentkey=Key_GetNum();
		if(currentkey!=0)
		{
			uint8_t SetFlag_temp=0;
			if(currentkey==1)
			{
				SetFlag--;
				if(SetFlag<=0) SetFlag=2;
			}
			else if(currentkey==2)
			{
				SetFlag++;
				if(SetFlag>2) SetFlag=1;
			}
			else if(currentkey==3)
			{
				OLED_Clear();
				OLED_Update();
				SetFlag_temp=SetFlag;
			}

			if(SetFlag_temp!=0)
			{
				if(SetFlag_temp==1) return 0;
				else if(SetFlag_temp==2)
				{
					Set_Time();
				}
			}
		}
		
		switch(SetFlag)
		{
			case 1:
				{
					Show_Setting_UI();
					OLED_ReverseArea(0,0,16,16);
					OLED_Update();
				}
					break;

			case 2:
				{
					Show_Setting_UI();
					OLED_ReverseArea(0,16,96,16);
					OLED_Update();
				}
					break;

				default:
					break;
		}		
	}	
}

//菜单UI
typedef enum {
    MENU_IDLE,
    MENU_ANIMATING
} MenuState;

MenuState menu_state = MENU_IDLE;
uint8_t pre_selection;//上次选择的选项
uint8_t target_selection;//目标选项
uint8_t x_pre=48;//上次选项的x坐标
uint8_t Speed=4;//速度
void MenuToFunction(void)
{
	for(uint8_t i=0;i<=6;i++)
	{
		OLED_Clear();
			if(pre_selection>=1)
		{
			OLED_ShowImage(x_pre-48,16+8*i,32,32,Menu_Graph[pre_selection-1]);
		}
		
		
		OLED_ShowImage(x_pre,16+8*i,32,32,Menu_Graph[pre_selection]);
		OLED_ShowImage(x_pre+48,16+8*i,32,32,Menu_Graph[pre_selection+1]);
		
		OLED_Update();
	}
	
}
void Menu_UI(void)
{	
	OLED_Clear();
	OLED_ShowImage(42,10,44,44,Frame);
	if(pre_selection<target_selection)
	{
		x_pre-=Speed;
		if(x_pre==0)
		{
			pre_selection++;
			x_pre=48;			
		}
	}
	else if(pre_selection>target_selection)
	{
		x_pre+=Speed;
		if(x_pre==96)
		{
			pre_selection--;
			x_pre=48;
		}
	}

	if(pre_selection>=1 && pre_selection-1 < 7)
	{
		OLED_ShowImage(x_pre-48,16,32,32,Menu_Graph[pre_selection-1]);
	}
	
	if(pre_selection>=2 && pre_selection-2 < 7)
	{
		OLED_ShowImage(x_pre-96,16,32,32,Menu_Graph[pre_selection-2]);
	}
	
	if(pre_selection < 7)
	{
		OLED_ShowImage(x_pre,16,32,32,Menu_Graph[pre_selection]);
	}
	if(pre_selection+1 < 7)
	{
		OLED_ShowImage(x_pre+48,16,32,32,Menu_Graph[pre_selection+1]);
	}
	if(pre_selection+2 < 7)
	{
		OLED_ShowImage(x_pre+96,16,32,32,Menu_Graph[pre_selection+2]);
	}

	OLED_Update();

	if(pre_selection == target_selection) {
        menu_state = MENU_IDLE;
    }
}

void Set_Selection(uint8_t Pre_Selection,uint8_t Target_Selection)
{
	if(menu_state==MENU_IDLE)
	{
		if(Pre_Selection < 7 && Target_Selection < 7) {
			pre_selection = Pre_Selection;
			target_selection = Target_Selection;
			menu_state=MENU_ANIMATING;
			Menu_UI();
		}
	}
}

int Menu_Page(void)
{
	static int MenuFlag=1;	
	static uint8_t initialized = 0;
	static uint8_t lastKey = 0;
	if (!initialized) {
		pre_selection = MenuFlag - 1;
		target_selection = MenuFlag - 1;
		menu_state = MENU_IDLE;
		initialized = 1;
	}
	while (1)
	{
		uint8_t currentkey=Key_GetNum();
		if(currentkey != lastKey && currentkey != 0)
		{
			uint8_t MenuFlag_temp=0;
			int old_selection = MenuFlag - 1;
			if(currentkey==1)
			{
				MenuFlag--;
				if(MenuFlag<=0) MenuFlag=7;
			}
			else if(currentkey==2)
			{
				MenuFlag++;
				if(MenuFlag>7) MenuFlag=1;
			}
			else if(currentkey==3)
			{
				OLED_Clear();
				OLED_Update();
				MenuFlag_temp=MenuFlag;
			}

			int new_selection = MenuFlag - 1;
			Set_Selection(old_selection, new_selection);

			if(MenuFlag_temp!=0)
			{
				if(MenuFlag_temp==1) return 0;
				else if(MenuFlag_temp==2)
				{
					MenuToFunction();	
					StopWatch_Page();									
				}
				else if(MenuFlag_temp==3)
				{		
					MenuToFunction();
					FlashLight_Page();			
				}
				else if(MenuFlag_temp==4)
				{	
					MenuToFunction();	
					MPU6050_Page();		
				}
				else if(MenuFlag_temp==5)
				{		
					MenuToFunction();
					Game_Page();			
				}
				else if(MenuFlag_temp==6)
				{		
					MenuToFunction();			
				}
				else if(MenuFlag_temp==7)
				{		
					MenuToFunction();	
					Gradienter_Page();	
				}
			}
		}
		lastKey = currentkey;
		
		Menu_UI();
		
		// 添加小延时，避免CPU占用过高
		for(volatile int i=0; i<1000; i++);
	}	
}
