#include "light_ui.h"
#include "light_task.h"
#include "ui_comp.h"

#include "oled.h"
#include "bmp.h"

key_table table[30]=
{
	//当前页面 下一个页面 上一个页面 确认页面 返回页面 
	//第0层,开机画面
	{0,0,0,1,0,(*hello)},
	
};

void hello()
{

}

static UIElement *ui_elements[MAX_UI_ELEMENTS];
static uint8_t ui_count = 0;

void UI_Register(UIElement *elem)
{
    if (ui_count < MAX_UI_ELEMENTS) {
        ui_elements[ui_count++] = elem;
    }
}

void UI_DrawAll(void *arg)
{
    for (uint8_t i = 0; i < ui_count; i++) {
        if (ui_elements[i]->visible && ui_elements[i]->draw_func) {
            ui_elements[i]->draw_func(ui_elements[i]->data);
        }
    }
    OLED_Refresh();
}

UIElement *UI_Find(const char *name)
{
    for (uint8_t i = 0; i < ui_count; i++) {
        if (ui_elements[i]->name && strcmp(ui_elements[i]->name, name) == 0) {
            return ui_elements[i];
        }
    }
    return 0;
}

void UI_Init(void)
{
    ui_count = 0;
}


FanDisplayData fan1_display = {
    .level = 1,
    .frame_index = 0,
    .x = 10,
    .y = 20
};

UIElement fan1_element;

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

//注册风扇的组件
void DrawFanElement(void *data)
{
    FanDisplayData *fan = (FanDisplayData *)data;
    ShowFanStatus(fan->x, fan->y, fan->level, fan->frame_index);
}

//更新风扇的组件
void FanFrameUpdate(void *arg)
{
    FanDisplayData *fan = (FanDisplayData *)arg;

    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    uint32_t interval = 0;
    switch (fan->level)
    {
    case 0: interval = 500; break;
    case 1: interval = 64; break;
    case 2: interval = 32; break;
    case 3: interval = 16;  break;
    default: interval = 500; break;
    }

    if (now - last_tick >= interval)
    {
        last_tick = now;
        fan->frame_index = (fan->frame_index + 1) % 6;
    }
}

void UI_Setup_All(void)
{
    UI_Init(); // 清空注册
    UI_Register(&fan1_element);
    // 可继续注册其他组件，如时间、温度等
}

//显示风扇并旋转
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx)
{
    // 显示风扇图标（24x24）
    OLED_ShowPicture(x, y, 24, 24, FAN_FRAMES[fan_frame_idx], 1);

    // 根据档位显示不同数量的风格条
    switch (level)
    {
    case 0:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 1:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_AIR, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 2:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_AIR, 1);
      break;
    case 3:
       OLED_ShowPicture(x + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 27 + 30, y + 6, 24, 12, BAR_SOLID, 1);
       OLED_ShowPicture(x + 54 + 30, y + 6, 24, 12, BAR_SOLID, 1);
      break;
    default:
      break;
    }
}

//显示运行时间
void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode)
{

		char time_str[12] = {0};
		OLED_ShowPicture(x,y,24,24,TIMER1,mode);
		if(total_seconds<3600)
		{
			snprintf(time_str, sizeof(time_str), "%02d:%02ds", 
			total_seconds/60,total_seconds%60);
			OLED_ShowString(x+24, y+4, (uint8_t *)time_str, 16, mode);
		}
		else
		{
			snprintf(time_str, sizeof(time_str), "%02d:%02dm", 
			total_seconds/3600,(total_seconds%3600)/60);
			OLED_ShowString(x+24, y+4, (uint8_t *)time_str, 16, mode);
		}
}


void Light_ui_test()
{

	Paint_Show_Arc(64,32,25,120,240,4,1,0,1);
	
}

