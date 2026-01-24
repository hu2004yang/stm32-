#include "SetTime.h"

void Show_SetTime_First_UI(void)
{
    OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(0,16,OLED_8X16,"年:%4d",MyRTC_Time[0]);
	OLED_Printf(0,32,OLED_8X16,"月:%2d",MyRTC_Time[1]);
	OLED_Printf(0,48,OLED_8X16,"日:%2d",MyRTC_Time[2]);
}

void Show_SetTime_Second_UI(void)
{
    OLED_Printf(0,0,OLED_8X16,"时:%2d",MyRTC_Time[3]);
	OLED_Printf(0,16,OLED_8X16,"分:%2d",MyRTC_Time[4]);
	OLED_Printf(0,32,OLED_8X16,"秒:%2d",MyRTC_Time[5]);
}

void RTC_Time_change(uint8_t i,uint8_t flag)
{
    if(flag==1)
    {
        MyRTC_Time[i]++;
    }
    else if(flag==2)
    {
        MyRTC_Time[i]--;
    }
    MyRTC_SetTime();
}

int SetYear(void)
{
    while (1)
    {    
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
            if(Time_Num==1)//数值加1
		{
			RTC_Time_change(0,1);
		}
		else if(Time_Num==2)//数值减1
		{
			RTC_Time_change(0,2);
		}

		else if(Time_Num==3)//确认，保存并退出
		{
			return 1;
		}           
        }
        
        Show_SetTime_First_UI();
        OLED_ReverseArea(24,16,32,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }   
} 

int SetMonth(void)
{
    while(1)
    {
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
            if(Time_Num==1)
        {
            RTC_Time_change(1,1);
            if(MyRTC_Time[1]>=13){MyRTC_Time[1]=1;MyRTC_SetTime();}
        }
        else if(Time_Num==2)
        {
            RTC_Time_change(1,2);
            if(MyRTC_Time[1]<=0){MyRTC_Time[1]=12;MyRTC_SetTime();}
        }
        else if(Time_Num==3)
        {
            return 1;
        }           
        }
        
        Show_SetTime_First_UI();
        OLED_ReverseArea(24,32,16,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }   
} 

int SetDay(void)
{
    while (1)
    {
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
            if(Time_Num==1)
        {
            RTC_Time_change(2,1);
            if(MyRTC_Time[2]>=32){MyRTC_Time[2]=1;MyRTC_SetTime();}
        }
        else if(Time_Num==2)
        {
            RTC_Time_change(2,2);
            if(MyRTC_Time[2]<=0){MyRTC_Time[2]=31;MyRTC_SetTime();}
        }
        else if(Time_Num==3)
        {
            return 1;
        }            
        }
        
        Show_SetTime_First_UI();
        OLED_ReverseArea(24,48,16,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }    
} 

int SetHour(void)
{
    while (1)
    {
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
            if(Time_Num==1)
        {
            RTC_Time_change(3,1);
            if(MyRTC_Time[3]>=24){MyRTC_Time[3]=0;MyRTC_SetTime();}
        }
        else if(Time_Num==2)
        {
            RTC_Time_change(3,2);
            if(MyRTC_Time[3]<0){MyRTC_Time[3]=23;MyRTC_SetTime();}
        }
        else if(Time_Num==3)
        {
            return 1;
        }            
        }
        
        Show_SetTime_Second_UI();
        OLED_ReverseArea(24,0,16,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }   
} 

int SetMin(void)
{
    while (1)
    {
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
            if(Time_Num==1)
        {
            RTC_Time_change(4,1);
            if(MyRTC_Time[4]>=60){MyRTC_Time[4]=0;MyRTC_SetTime();}
        }
        else if(Time_Num==2)
        {
            RTC_Time_change(4,2);
            if(MyRTC_Time[4]<0){MyRTC_Time[4]=59;MyRTC_SetTime();}
        }
        else if(Time_Num==3)
        {
            return 1;
        }            
        }
        
        Show_SetTime_Second_UI();
        OLED_ReverseArea(24,16,16,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }       
} 

int SetSec(void)
{
   while(1)
   {
        uint8_t Time_Num=Key_GetNum();
        if(Time_Num!=0)
        {
           if(Time_Num==1)
        {
            RTC_Time_change(5,1);
            if(MyRTC_Time[5]>=60){MyRTC_Time[5]=0;MyRTC_SetTime();}
        }
        else if(Time_Num==2)
        {
            RTC_Time_change(5,2);
            if(MyRTC_Time[5]<0){MyRTC_Time[5]=59;MyRTC_SetTime();}
        }
        else if(Time_Num==3)
        {
            return 1;
        }            
        }
        
        Show_SetTime_Second_UI();
        OLED_ReverseArea(24,32,16,16);
        OLED_Update();
        for(volatile int i = 0; i < 10000; i++); 
    }
}

int Set_Time(void)
{	
	static int Set_Time_Flag=1;
	while (1)
	{
		uint8_t currentkey=Key_GetNum();
		if(currentkey!=0)
		{
			uint8_t Set_Time_Flag_temp=0;
			if(currentkey==1)
			{
				Set_Time_Flag--;
				if(Set_Time_Flag<=0) Set_Time_Flag=7;
			}
			else if(currentkey==2)
			{
				Set_Time_Flag++;
				if(Set_Time_Flag>7) Set_Time_Flag=1;
			}
			else if(currentkey==3)
			{
				OLED_Clear();
				OLED_Update();
				Set_Time_Flag_temp=Set_Time_Flag;
			}
			
				if(Set_Time_Flag_temp==1) return 0;
				else if(Set_Time_Flag_temp==2)
				{
					SetYear();
				}
                else if(Set_Time_Flag_temp==3)
				{
					SetMonth();
				}
                else if(Set_Time_Flag_temp==4)
				{
                    SetDay();
				}
                else if(Set_Time_Flag_temp==5)
				{
                    SetHour();
				}
                else if(Set_Time_Flag_temp==6)
				{
                    SetMin();
				}
                else if(Set_Time_Flag_temp==7)
				{
					SetSec();
				}			
		}
		
		switch(Set_Time_Flag)
		{
			case 1:
				{
                    OLED_Clear();
					Show_SetTime_First_UI();
					OLED_ReverseArea(0,0,16,16);
					OLED_Update();
				}
				break;

			case 2:
				{
                    OLED_Clear();
					Show_SetTime_First_UI();
					OLED_ReverseArea(0,16,16,16);
					OLED_Update();
				}
				break;

            case 3:
				{
                    OLED_Clear();
					Show_SetTime_First_UI();
					OLED_ReverseArea(0,32,16,16);
					OLED_Update();
				}
				break;

            case 4:
				{
                    OLED_Clear();
					Show_SetTime_First_UI();
					OLED_ReverseArea(0,48,16,16);
					OLED_Update();
				}
				break;

            case 5:
				{
                    OLED_Clear();
					Show_SetTime_Second_UI();
					OLED_ReverseArea(0,0,16,16);
					OLED_Update();
				}
				break;

            case 6:
				{
                    OLED_Clear();
					Show_SetTime_Second_UI();
					OLED_ReverseArea(0,16,16,16);
					OLED_Update();
				}
				break;

            case 7:
				{
                    OLED_Clear();
					Show_SetTime_Second_UI();
					OLED_ReverseArea(0,32,16,16);
					OLED_Update();
				}
				break;            
		}			
	}	
}
