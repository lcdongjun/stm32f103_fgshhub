#ifndef __LIGHT_TASK_H
#define __LIGHT_TASK_H

#include "main.h"


#define MAX_TASKS 6

typedef struct {
    void (*func)(void *);       // 函数指针（带参数）
    void *arg;                  // 传递给函数的参数
    uint32_t interval_ms;       // 调用间隔
    uint32_t last_call_time;    // 上次调用时间
    uint8_t active;             // 是否激活
} DelayTask;

typedef struct {
    uint32_t start_time;
    uint32_t delay_ms;
    uint8_t active;
} NonBlockingDelay;


extern DelayTask tasks[MAX_TASKS];


void DelayCall(void (*func)(void *), void *arg, uint32_t interval_ms);
void DelayStart(NonBlockingDelay *delay, uint32_t delay_ms);
uint8_t DelayIsExpired(NonBlockingDelay *delay);


#endif

