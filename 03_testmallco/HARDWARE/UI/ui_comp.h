#ifndef __UI_COMP_H
#define __UI_COMP_H

#include "main.h"
void Paint_Show_Arc(u16 x_center, u16 y_center, u16 radius, u16 start_angle, u16 end_angle, u8 thickness, u16 color, u8 filled, u8 invPix);
//��ʾ���Ȳ���ת
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
//��ʾ���÷�������ʱ��
void OLED_FanTimeSet(u8 x, u8 y, u32 total_seconds, u32 stop_seconds, u8 twinkle);
//��ʾһ��ѭ����ʱ��
void ShowRunTime(u8 x, u8 y);
//��ʾ��ǰϵͳ����ʱ��
void Show_SysTime(u8 x, u8 y);
//��ʾ�¶�
void ShowTemp(u8 x,u8 y,u8 temp);



#endif // __UI_COMP_H

