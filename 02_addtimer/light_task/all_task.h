#ifndef __ALL_TASK_H
#define __ALL_TASK_H

#include "main.h"

typedef enum {
    FAN_IDLE,
    FAN_SETTING
} FanSetState;

extern bool Time_Task_Run;											//是否开启显示运行时间
extern uint8_t fan_mode, set_fan_mode ,Set_State;		//读取的风扇档位，要设置的风扇档位

void ShowTime_Task(void *arg);
void ShowBATLev_Task(void *arg);
void ShowTEMP_Task(void *arg);
void ShowFAN_Task(void *arg);
void Show_SysTime_Task(void *arg);
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);


void Run_Fan_Task( uint16_t time_interval);
void Run_ShowTime_Task(uint16_t time_interval);

//获取风扇的档位
void Get_GPIO_State(void);
//设置风扇的档位
void Set_Fan_State_NonBlocking(uint8_t count);
void Process_Fan_Set(void);


#endif

