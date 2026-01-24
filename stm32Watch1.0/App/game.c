#include "game.h"

void Show_Game_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"\xE8\xB0\xB7\xE6\xAD\x8C\xE5\xB0\x8F\xE6\x81\x90\xE9\xBE\x99",OLED_8X16);
}

int Game_Page(void)
{
	static int GameFlag=1;
	while (1)
	{
		uint8_t currentkey=Key_GetNum();
		if(currentkey!=0)
		{
			uint8_t GameFlag_temp=0;
			if(currentkey==1)
			{
				GameFlag--;
				if(GameFlag<=0) GameFlag=2;
			}
			else if(currentkey==2)
			{
				GameFlag++;
				if(GameFlag>2) GameFlag=1;
			}
			else if(currentkey==3)
			{
				OLED_Clear();
				OLED_Update();
				GameFlag_temp=GameFlag;
			}

			if(GameFlag_temp!=0)
			{
				if(GameFlag_temp==1) return 0;
				else if(GameFlag_temp==2)
				{					
					Dino_Game();				
				}
			}
		}
		
		switch(GameFlag)
		{
			case 1:
				{
					Show_Game_UI();
					OLED_ReverseArea(0,0,16,16);
					OLED_Update();
				}
					break;

			case 2:
				{
					Show_Game_UI();
					OLED_ReverseArea(0,16,80,16);
					OLED_Update();
				}
					break;

				default:
					break;
		}		
	}	
}
