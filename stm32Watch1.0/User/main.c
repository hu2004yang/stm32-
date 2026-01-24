#include "menu.h"
#include "Timer.h"
int main(void)
{
	menu_Init();
	Timer_Init();	
	
	int clkflag=1;
	while(1)
	{
		clkflag=First_Page_Clock();
		if(clkflag==1)
		{
			Menu_Page();
		}
		else if(clkflag==2)
		{
			Setting_Page();
		}
	}
}
