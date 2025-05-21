#ifndef __ALL_TASK_H
#define __ALL_TASK_H

#include "main.h"

typedef enum {
    FAN_IDLE,
    FAN_SETTING
} FanSetState;

extern bool Time_Task_Run;											//�Ƿ�����ʾ����ʱ��
extern uint8_t fan_mode, set_fan_mode ,Set_State;		//��ȡ�ķ��ȵ�λ��Ҫ���õķ��ȵ�λ

void ShowBATLev_Task(void *arg);
void ShowTEMP_Task(void *arg);
void ShowFAN_Task(void *arg);
void ShowTime_Task(void *arg);

void Run_Fan_Task(void);


#endif

