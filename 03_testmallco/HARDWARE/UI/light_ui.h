#ifndef __LIGHT_UI_H
#define __LIGHT_UI_H

#include "main.h"

typedef struct
{
	uint8_t current;
	uint8_t up;//���Ϸ�������
	uint8_t down;//���·�������
	uint8_t enter;//ȷ��������
	uint8_t back;//����
	void (*current_operation)();
} key_table;

extern key_table table[30];

extern void hello(void);

//��ʾ���Ȳ���ת
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
//��ʾ����ʱ��
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);

void Light_ui_test(void);

#endif // __LIGHT_UI_H

