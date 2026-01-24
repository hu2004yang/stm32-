#ifndef __MENU_H
#define __MENU_H

#include "key.h"
#include "Delay.h"
#include "oled.h"
#include "MyRTC.h"
#include "adc.h"
#include "SetTime.h"
#include "StopWatch.h"
#include "MPU6050.h"
#include "FlashLight.h"
#include "game.h"
#include "Gradienter.h"

extern int16_t MyRTC_Time[];

void menu_Init(void);
void Show_Clock_UI(void);
int First_Page_Clock(void);
int Setting_Page(void);
int Menu_Page(void);

#endif
