#ifndef __LIGHT_UI_H
#define __LIGHT_UI_H

#include "main.h"

//显示风扇并旋转
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
//显示运行时间
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);

void Light_ui_test(void);

#endif // __LIGHT_UI_H

