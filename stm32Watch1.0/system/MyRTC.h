#ifndef __MYRTC_H
#define __MYRTC_H

#include "stm32f10x.h"
#include <time.h>

extern int16_t MyRTC_Time[];

void MyRTC_Init(void);
void MyRTC_ReadTime(void);
void MyRTC_SetTime(void);

#endif
