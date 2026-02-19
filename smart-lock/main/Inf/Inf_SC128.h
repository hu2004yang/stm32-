#ifndef INF_SC128_H
#define INF_SC128_H

#include "Com_Debug.h"
#include "Inf_LED.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_task.h"

void Inf_SC128_Init(void);
Touch_Key_t Inf_SC128_ReadTouchkey(void);

#endif
