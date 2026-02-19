#ifndef INF_FPM385_h
#define INF_FPM385_h

#include "Com_Debug.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_task.h"
#include "Inf_WTN6.h"
#include "Inf_BDR6120.h"

#define LED_RED_FLICKER Inf_FPM385_LedControl(2, 0x04, 0x04, 0);
#define LED_GREEN_FLICKER Inf_FPM385_LedControl(2, 0x02, 0x02, 0);
#define LED_BLUE_FLICKER Inf_FPM385_LedControl(2, 0x01, 0x01, 0);

#define LED_RED_ON Inf_FPM385_LedControl(3, 0x04, 0x04, 0);
#define LED_GREEN_ON Inf_FPM385_LedControl(3, 0x02, 0x02, 0);
#define LED_BLUE_ON Inf_FPM385_LedControl(3, 0x01, 0x01, 0);
#define LED_COLOR_OFF Inf_FPM385_LedControl(4, 0x07, 0x07, 0);


void Inf_FPM385_OpenLock(void);
void Inf_FPM385_CloseLock(void);
void Inf_FPM385_Init(void);
void Inf_FPM385_ReadID(void);
void Inf_FPM385_LedControl(uint8_t fun, uint8_t startColor, uint8_t endColor, uint8_t cycle);
void PrintRecvData(char *pre, uint16_t len);
void Inf_FPM385_Sleep(void);
void Inf_FPM385_AutoEnroll(void);
void Inf_FPM385_AutoVerify(void);
void Inf_FPM385_Delfinger(void);
#endif
