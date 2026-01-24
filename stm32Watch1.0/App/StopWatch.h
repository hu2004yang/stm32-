#ifndef __STOPWATCH_H
#define __STOPWATCH_H

#include "OLED.h"
#include "key.h"
#include "Delay.h"
#include "MyRTC.h"

extern uint8_t start_timing_flag;
extern uint8_t hour, min, sec;

void StopWatch_Tick(void);
int StopWatch_Page(void);

#endif
