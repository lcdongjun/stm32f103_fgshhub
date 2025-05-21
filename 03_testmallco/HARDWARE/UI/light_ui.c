#include "light_ui.h"
#include "light_task.h"
#include "ui_comp.h"

#include "oled.h"
#include "bmp.h"

static UIElement *ui_elements[MAX_UI_ELEMENTS];
static uint8_t ui_count = 0;

void UI_Register(UIElement *elem)
{
    if (ui_count < MAX_UI_ELEMENTS){
        ui_elements[ui_count++] = elem;
    }
}

void UI_DrawAll(void *arg)
{
    for (uint8_t i = 0; i < ui_count; i++){
        if (ui_elements[i]->visible && ui_elements[i]->draw_func && !ui_elements[i]->stop){
            ui_elements[i]->draw_func(ui_elements[i]->data);
        }
    }
    OLED_Refresh();
}

UIElement *UI_Find(const char *name)
{
    for (uint8_t i = 0; i < ui_count; i++){
        if (ui_elements[i]->name && strcmp(ui_elements[i]->name, name) == 0){
            return ui_elements[i];
        }
    }
    return 0;
}

void UI_Init(void)
{
    ui_count = 0;
}

//�����������
DisplayData fan1_display = {
    .level = 1,
    .frame_index = 0,
    .x = 10,
    .y = 20,
		.pause = 0
};

UIElement fan1_element = {
    .name = "fan1",
    .x = 11,
    .y = 18,
    .width = 102,
    .height = 24,
    .font_size = 0,
    .visible = 1,
    .draw_func = DrawFanElement,
    .data = &fan1_display
};

//������ʾ����ѭ����ʱ�����
DisplayData runtime = {
    .level = 1,
    .frame_index = 0,
    .x = 86,
    .y = 54,
		.pause = 0
};

UIElement runtime_element = {
    .name = "runtime",
    .x = 98,
    .y = 54,
    .width = 26,
    .height = 12,
    .font_size = 0,
    .visible = 1,
    .draw_func = ShowRunTimeElement,
    .data = &runtime
};

//������ʾϵͳ����ʱ������
DisplayData systime = {
    .level = 1,
    .frame_index = 0,
    .x = 0,
    .y = 0,
		.pause = 0
};

UIElement systime_element = {
    .name = "systime",
    .x = 0,
    .y = 0,
    .width = 52,
    .height = 12,
    .font_size = 12,
    .visible = 1,
    .draw_func = ShowSysTimeElement,
    .data = &systime
};


//ע��ѭ����ʱ��ʾ���
void ShowRunTimeElement(void *date)
{
	DisplayData *runtime = (DisplayData *)date;
	ShowRunTime(runtime->x,runtime->y);
}

//ע��ϵͳ����ʱ����ʾ���
void ShowSysTimeElement(void *date)
{
	DisplayData *systime = (DisplayData *)date;
	Show_SysTime(systime->x,systime->y);
}

//ע����ȵ����
void DrawFanElement(void *data)
{
    DisplayData *fan = (DisplayData *)data;
    ShowFanStatus(fan->x, fan->y, fan->level, fan->frame_index);
}

//���·��ȵ����
void FanFrameUpdate(void *arg)
{
    DisplayData *fan = (DisplayData *)arg;

    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    uint32_t interval = 0;
    switch (fan->level)
    {
    case 0: interval = 500; break;
    case 1: interval = 48; break;
    case 2: interval = 32; break;
    case 3: interval = 16;  break;
    default: interval = 500; break;
    }

    if (now - last_tick >= interval)
    {
        last_tick = now;
				if(fan->pause ==0)
					fan->frame_index = (fan->frame_index + 1) % 6;
    }
}


void UI_Setup_All(void)
{
    UI_Init(); // ���ע��
    UI_Register(&fan1_element);
		UI_Register(&runtime_element);
		UI_Register(&systime_element);
}

