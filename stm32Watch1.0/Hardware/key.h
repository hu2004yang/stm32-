#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// 按键编号定义
#define KEY1_PRESSED     1
#define KEY2_PRESSED     2
#define KEY3_PRESSED     3
#define NO_KEY_PRESSED   0

// 消抖周期(20ms，假设系统滴答为1ms)
#define KEY_DEBOUNCE_TICKS  20

extern uint8_t Key_Num;

// 函数声明
void Key_Init(void);                // 按键初始化
uint8_t Key_GetNum(void);           // 获取按键编号(带消抖)
uint8_t Key_GetState(void);         // 直接获取当前按键状态
void Key_Tick(void);                // 按键状态检测(需周期性调用)

// 内联函数用于快速检测单个按键状态
static inline uint8_t Is_KEY1_Pressed(void) {
    return !(GPIOB->IDR & (1 << 1));
}

static inline uint8_t Is_KEY2_Pressed(void) {
    return !(GPIOA->IDR & (1 << 6));
}

static inline uint8_t Is_KEY3_Pressed(void) {
    return !(GPIOA->IDR & (1 << 4));
}

#endif /* __KEY_H */
