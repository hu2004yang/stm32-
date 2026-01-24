#include "Gradienter.h"

void Show_Gradienter_UI(void)
{
	MPU6050_Calculation();
	OLED_DrawCircle(64,32,30,0);
	OLED_DrawCircle(64-Roll,32+Pitch,4,1);
}

int Gradienter_Page(void)
{
    while (1)
    {
        uint8_t currentKey=Key_GetNum();
        if(currentKey==3)
        {
            OLED_Clear();
            OLED_Update();
            return 0;
        }
        OLED_Clear();
        Show_Gradienter_UI();
        OLED_Update();
    }    
}
