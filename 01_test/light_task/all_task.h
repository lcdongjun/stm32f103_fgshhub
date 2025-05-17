#ifndef __ALL_TASK_H
#define __ALL_TASK_H

#include "main.h"

void ShowTime_Task(void *arg);
void ShowBATLev_Task(void *arg);
void ShowTEMP_Task(void *arg);
void ShowFAN_Task(void *arg);
void Test_Delay_Task(void *arg);
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);

#endif

