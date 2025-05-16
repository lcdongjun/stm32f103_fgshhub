#ifndef __LIGHT_TASK_H
#define __LIGHT_TASK_H

#include "main.h"


#define MAX_TASKS 6

typedef struct {
    void (*func)(void *);       // ����ָ�루��������
    void *arg;                  // ���ݸ������Ĳ���
    uint32_t interval_ms;       // ���ü��
    uint32_t last_call_time;    // �ϴε���ʱ��
    uint8_t active;             // �Ƿ񼤻�
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

