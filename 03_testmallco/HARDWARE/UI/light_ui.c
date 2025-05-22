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
    for (uint8_t i = 0; i < ui_count; i++) {
        UIElement *elem = ui_elements[i];

        if (!elem->visible || elem->stop)
            continue;

        if (elem->twinkle && elem->twinkle_hidden) {
            OLED_Clear_Area(elem->x, elem->y, elem->width, elem->height);
        } else if (elem->draw_func) {
            elem->draw_func(elem->data);
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
    .width = 114,
    .height = 24,
    .font_size = 0,
    .visible = 1,
    .draw_func = DrawFanElement,
		.update_func = FanFrameUpdate,
    .data = &fan1_display,

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
		.update_func = ShowRunTimeFrameUpdate,
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
    .data = &systime,
};

//������������ʱ������
FanTimeSetData fantimeset = {
		.x = 10,
		.y = 42,
    .width = 72,
    .height = 24,
};
UIElement fantimeset_element = {
    .name = "fantimeset",
    .x = 10,
    .y = 42,
    .width = 72,
    .height = 24,
    .font_size = 16,
    .visible = 1,
    .draw_func = FanTimeSetElement,
		.update_func = FanTimeSetFrameUpdate,
    .data = &fantimeset
};

//������ʾ�¶����
DisplayData temp_display = {
    .x = 42,
    .y = 3,
		.pause = 0
};

UIElement temp_element = {
    .name = "temp",
    .x = 42,
    .y = 3,
    .width = 42,
    .height = 16,
    .font_size = 0,
    .visible = 1,
    .draw_func = TempDisplayAndSetElement,
		.update_func = TempDisplayAndSetFrameUpdate,
    .data = &temp_display,

};


//ע����ʾ�������¶����
void TempDisplayAndSetElement(void *date)
{
	DisplayData *temp_display = (DisplayData *)date;
	ShowTemp(temp_display->x,temp_display->y,temp_display->temp);
}

//������ʾ�������¶����
void TempDisplayAndSetFrameUpdate(void *arg)
{
		DisplayData *temp_display = (DisplayData *)arg;
		ShowTemp(temp_display->x,temp_display->y,temp_display->temp);
}

//ע�����÷�������ʱ�����ʾ���
void FanTimeSetElement(void *date)
{
	FanTimeSetData *fantimeset = (FanTimeSetData *)date;
	OLED_FanTimeSet(fantimeset->x,fantimeset->y,fantimeset->total_seconds,fantimeset->stop_seconds,fantimeset->show_settime);
}
//�������÷�������ʱ������
void FanTimeSetFrameUpdate(void *arg)
{
	
}


//ע��ѭ����ʱ��ʾ���
void ShowRunTimeElement(void *date)
{
	DisplayData *runtime = (DisplayData *)date;
	ShowRunTime(runtime->x,runtime->y);
}

//����ѭ����ʱ��ʾ���
void ShowRunTimeFrameUpdate(void *arg)
{
    DisplayData *runtime = (DisplayData *)arg;
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
    uint32_t now = HAL_GetTick();

    uint32_t interval = 0;
    switch (fan->level)
    {
    case 0: interval = 500; break;
    case 1: interval = 48; break;
    case 2: interval = 32; break;
    case 3: interval = 16; break;
    default: interval = 500; break;
    }

    if (now - fan->last_tick >= interval) {
        fan->last_tick = now;
        if (fan->pause == 0)
            fan->frame_index = (fan->frame_index + 1) % 6;
    }
}


void UI_Setup_All(void)
{
    UI_Init(); // ���ע��
    UI_Register(&fan1_element);
		UI_Register(&runtime_element);
		UI_Register(&systime_element);
		UI_Register(&fantimeset_element);
		UI_Register(&temp_element);
}

void UI_UpdateAll(void *arg)
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < ui_count; i++) {
        UIElement *elem = ui_elements[i];

        //ͨ����˸�߼�
        if (elem->twinkle) {
            if (now - elem->twinkle_tick >= elem->twinkle_interval) {
                elem->twinkle_tick = now;
                elem->twinkle_hidden = !elem->twinkle_hidden;
            }
        } else {
            elem->twinkle_hidden = 0; // ����˸��ʼ����ʾ
        }

        // �����Զ�����º���
        if (elem->visible && elem->update_func) {
            elem->update_func(elem->data);
        }
    }
}

void UI_EnableTwinkle(UIElement *elem, uint8_t enable, uint32_t interval)
{
    if (enable) {
        if (!elem->twinkle || elem->twinkle_interval != interval) {
            elem->twinkle = 1;
            elem->twinkle_interval = interval;
            elem->twinkle_tick = HAL_GetTick();
        }
    } else {
        if (elem->twinkle) {
            elem->twinkle = 0;
            elem->twinkle_hidden = 0;
        }
    }
}
