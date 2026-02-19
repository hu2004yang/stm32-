#ifndef __APP_COMMUNICATION_H__
#define __APP_COMMUNICATION_H__ 

#include "Com_Debug.h"
#include "Dri_Ble.h"
#include "Inf_WTN6.h"
#include "Dri_NVS.h"
#include "Inf_BDR6120.h"
#include "App_IO.h"
#include "Dri_Wifi.h" 

void App_Communication_start(void);
void App_Communication_OTA(void);

#endif
