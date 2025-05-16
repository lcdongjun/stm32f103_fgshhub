#include "light_task.h"

DelayTask tasks[MAX_TASKS] = {0};

//����һ������
void DelayCall(void (*func)(void *), void *arg, uint32_t interval_ms)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].active && tasks[i].func == func && tasks[i].arg == arg) {
            // ��ע�ᣬ����Ƿ�ʱ��
            if (HAL_GetTick() - tasks[i].last_call_time >= tasks[i].interval_ms) {
                tasks[i].last_call_time = HAL_GetTick();
                tasks[i].func(tasks[i].arg);
            }
            return;
        } else if (!tasks[i].active) {
            // ������ע��
            tasks[i].func = func;
            tasks[i].arg = arg;
            tasks[i].interval_ms = interval_ms;
            tasks[i].last_call_time = HAL_GetTick();
            tasks[i].active = 1;
            return;
        }
    }
}

//ɾ������
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

//������������ʱ
void DelayStart(NonBlockingDelay *delay, uint32_t delay_ms)
{
    delay->start_time = HAL_GetTick();
    delay->delay_ms = delay_ms;
    delay->active = 1;
}

//������ʱ
uint8_t DelayIsExpired(NonBlockingDelay *delay)
{
    if (!delay->active) return 1;
    if (HAL_GetTick() - delay->start_time >= delay->delay_ms) {
        delay->active = 0;
        return 1;
    }
    return 0;
}

