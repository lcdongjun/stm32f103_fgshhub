#include "light_ui.h"
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

