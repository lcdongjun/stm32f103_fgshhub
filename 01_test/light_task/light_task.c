#include "light_task.h"

DelayTask tasks[MAX_TASKS] = {0};

//创建一个任务
void DelayCall(void (*func)(void *), void *arg, uint32_t interval_ms)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].active && tasks[i].func == func && tasks[i].arg == arg) {
            // 已注册，检查是否到时间
            if (HAL_GetTick() - tasks[i].last_call_time >= tasks[i].interval_ms) {
                tasks[i].last_call_time = HAL_GetTick();
                tasks[i].func(tasks[i].arg);
            }
            return;
        } else if (!tasks[i].active) {
            // 新任务注册
            tasks[i].func = func;
            tasks[i].arg = arg;
            tasks[i].interval_ms = interval_ms;
            tasks[i].last_call_time = HAL_GetTick();
            tasks[i].active = 1;
            return;
        }
    }
}

//删除任务
void DelayCallRemove(void (*func)(void *), void *arg)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].active && tasks[i].func == func && tasks[i].arg == arg) {
            tasks[i].active = 0;
            tasks[i].func = NULL;
            tasks[i].arg = NULL;
        }
    }
}

//开启不阻塞延时
void DelayStart(NonBlockingDelay *delay, uint32_t delay_ms)
{
    delay->start_time = HAL_GetTick();
    delay->delay_ms = delay_ms;
    delay->active = 1;
}

//进行延时
uint8_t DelayIsExpired(NonBlockingDelay *delay)
{
    if (!delay->active) return 1;
    if (HAL_GetTick() - delay->start_time >= delay->delay_ms) {
        delay->active = 0;
        return 1;
    }
    return 0;
}

