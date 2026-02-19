#ifndef INF_LED_H
#define INF_LED_H
#include "Com_Debug.h"

extern uint8_t black[3];
extern uint8_t white[3];
extern uint8_t red[3];
extern uint8_t green[3];
extern uint8_t blue[3];
extern uint8_t cyan[3];   /* 青色 */
extern uint8_t purple[3]; /* 紫色 */

typedef enum
{
    key_0,
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,
    key_sharp,
    key_M,
    key_no    
}Touch_Key_t;


void Inf_LED_Init(void);
void Inf_LED_LightLeds(void);
void Inf_LED_Lightkeyled(Touch_Key_t key,uint8_t color[]);
void Inf_LED_LightAll(uint8_t color[]);

#endif 