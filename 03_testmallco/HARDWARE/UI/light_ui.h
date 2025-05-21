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
		void (*update_func)(void *);// ���º���
    void *data;           // ָ�����ʵ����ʾ���ݵ�ָ��
	
		uint8_t twinkle;               // �Ƿ�������˸
    uint8_t twinkle_hidden;        // ��ǰ�Ƿ����أ��ڲ��߼�ʹ�ã�
    uint32_t twinkle_interval;     // ��˸��������룩
    uint32_t twinkle_tick;         // �ϴ��л�ʱ��
	
} UIElement;


typedef struct {
    uint8_t level;         // ��λ
    uint8_t frame_index;   // ����֡���
    uint16_t x, y;          // ����
		uint8_t pause;					//�Ƿ���ͣ����
		uint32_t last_tick;
} DisplayData;

typedef struct {
    uint16_t x, y;          // ����
		u32 total_seconds;			//����ʱ
		u32 stop_seconds;				//���õ�ʱ��
    uint8_t stop;         // �����Ƿ���������
	  u32 width;
    u32 height;
		u32 show_settime;			//�Ƿ���ʾ���õ�ʱ��
} FanTimeSetData;

//������������ʱ������
extern FanTimeSetData fantimeset;
extern UIElement fantimeset_element;

extern DisplayData systime;
extern UIElement systime_element;

extern DisplayData runtime;//�����ĺ�ʱ������
extern UIElement runtime_element;//ע�����ʾ��ʱ�����

extern DisplayData fan1_display;//�����ķ�������
extern UIElement fan1_element;		//ע��ķ������

void UI_UpdateAll(void);
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
//�������÷�������ʱ������
void FanTimeSetFrameUpdate(void *arg);
//ע����ȵ����
void DrawFanElement(void *data);
//���·��ȵ����
void FanFrameUpdate(void *arg);
//ע�����÷�������ʱ�����ʾ���
void FanTimeSetElement(void *date);
//������˸
void UI_EnableTwinkle(UIElement *elem, uint8_t enable, uint32_t interval);

#endif // __LIGHT_UI_H

