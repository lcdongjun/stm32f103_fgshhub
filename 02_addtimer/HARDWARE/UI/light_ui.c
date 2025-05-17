#include "light_ui.h"
#include "ui_comp.h"

#include "oled.h"
#include "bmp.h"

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
    u32 minutes = total_seconds / 60;
    u32 seconds = total_seconds % 60;
		OLED_ShowPicture(x,y,24,24,TIMER1,1);
    OLED_ShowNum(x+28,y+4, minutes, 2, size, mode);
    OLED_ShowString(x + size * 2 * 1-16+28, y-1+4, ":", size, mode);
    OLED_ShowNum(x + size * 2 * 1-8+28, y+4, seconds, 2, size, mode);
}

void Light_ui_test()
{

	Paint_Show_Arc(64,32,25,120,240,4,1,0,1);
	
}
