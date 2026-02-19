#ifndef APP_IO_H
#define APP_IO_H
#include "Com_Debug.h"
#include "Inf_LED.h"
#include "Inf_SC128.h"
#include "Inf_WTN6.h"
#include "Dri_NVS.h"
#include "Inf_BDR6120.h"
#include "Inf_FPM385.h"

typedef enum
{
    FREE=0,
    INPUT
}Input_state_t;

typedef enum
{
    ADD_PWD=0,
    DEL_PWD,
    CHECK_PWD
}Pwd_handler_state_t;

typedef enum
{
    FINGER_ADD=0,
    FINGER_DEL,
    FINGER_CHECK
}Finger_state_t;

void App_IO_Start(void);
void App_IO_Touchkey_Scan(void);
void App_IO_TaskHandler(void);

void App_IO_ADD_PWD(void);
void App_IO_DEL_PWD(void);
void App_IO_CHECK_PWD(void);
void App_IO_Finger_Scan(void);

void App_IO_nvs_Test(void);

#endif
