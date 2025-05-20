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

typedef struct {
    const char *name;     // �������

    uint16_t x;            // X����
    uint16_t y;            // Y����
    uint16_t width;       // ���
    uint16_t height;      // �߶�

    uint8_t font_size;    // �����С
    uint8_t visible;      // �Ƿ���ʾ

    void (*draw_func)(void *);   // ���ƺ���ָ��
    void *data;           // ָ�����ʵ����ʾ���ݵ�ָ��
} UIElement;


typedef struct {
    uint8_t level;         // ��λ
    uint8_t frame_index;   // ����֡���
    uint16_t x, y;          // ����
} FanDisplayData;


extern FanDisplayData fan1_display;//�����ķ�������
extern UIElement fan1_element;		//ע��ķ������
	
extern key_table table[30];

extern void hello(void);



// ע�� UI Ԫ�أ�����������޸ģ�
#define MAX_UI_ELEMENTS 10

// ע��һ�� UI Ԫ��
void UI_Register(UIElement *elem);

// �������пɼ�Ԫ��
void UI_DrawAll(void *arg);

// �����Ʋ���Ԫ�أ���ѡ��
UIElement *UI_Find(const char *name);

// ��ʼ�� UI ϵͳ
void UI_Init(void);

//��ʼ����ע�����
void UI_Setup_All(void);


//ע����ȵ����
void DrawFanElement(void *data);
//���·��ȵ����
void FanFrameUpdate(void *arg);
//��ʾ���Ȳ���ת
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx);
//��ʾ����ʱ��
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode);

void Light_ui_test(void);

#endif // __LIGHT_UI_H

