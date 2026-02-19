#ifndef __DRI_NVS_H__
#define __DRI_NVS_H__

#include "Com_Debug.h"
#include "nvs_flash.h"
#include "nvs.h"

void Dri_NVS_Init(void);
esp_err_t Dri_NVS_WriteI8(char *key, int8_t value);
esp_err_t Dri_NVS_Delkey(char *key);
esp_err_t Dri_NVS_IskeyMatch(char *key);

#endif
