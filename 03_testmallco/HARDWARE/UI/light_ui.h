#ifndef __LIGHT_UI_H
#define __LIGHT_UI_H

#include "main.h"

// ע�� UI Ԫ��
#define MAX_UI_ELEMENTS 10


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

		uint8_t stop;
    void (*draw_func)(void *);   // ���ƺ���ָ��
    void *data;           // ָ�����ʵ����ʾ���ݵ�ָ��
} UIElement;


typedef struct {
    uint8_t level;         // ��λ
    uint8_t frame_index;   // ����֡���
    uint16_t x, y;          // ����
		uint8_t pause;					//�Ƿ���ͣ����
} DisplayData;




extern DisplayData runtime;//�����ĺ�ʱ������
extern UIElement runtime_element;//ע�����ʾ��ʱ�����

extern DisplayData fan1_display;//�����ķ�������
extern UIElement fan1_element;		//ע��ķ������


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
//����ѭ����ʱ��ʾ���
void ShowRunTimeElement(void *date);
//ע��ϵͳ����ʱ����ʾ���
void ShowSysTimeElement(void *date);
//ע����ȵ����
void DrawFanElement(void *data);
//���·��ȵ����
void FanFrameUpdate(void *arg);

#endif // __LIGHT_UI_H

