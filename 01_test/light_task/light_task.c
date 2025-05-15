#include "light_task.h"


DelayTask tasks[MAX_TASKS] = {0};

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
