#include "ui_comp.h"
#include "math.h"

#include "rtc.h"

#include "oled.h"
#include "bmp.h"

void Paint_Show_Arc(u16 x_center, u16 y_center, u16 radius, u16 start_angle, u16 end_angle, u8 thickness, u16 color, u8 filled, u8 invPix)
{

    // 将角度转换为弧度
    float radian_start = start_angle * 3.14159 / 180.0;
    float radian_end = end_angle * 3.14159 / 180.0;

    // 设置步进大小，控制绘制的平滑度
    float angle_step = 0.002; // 每次递增的弧度，值越小越平滑

    // 初始位置
    u16 x_prev = x_center + (int)(radius * cos(radian_start));
    u16 y_prev = y_center + (int)(radius * sin(radian_start));

    // 绘制弧线
    for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
        // 使用三角函数计算圆上点的坐标
        u16 x = x_center + (int)(radius * cos(angle));
        u16 y = y_center + (int)(radius * sin(angle));

        // 使用画线函数连接相邻的点，形成弧线
        OLED_DrawLine(x_prev, y_prev, x, y, color);

        // 更新前一个点的坐标
        x_prev = x;
        y_prev = y;
    }

    // 如果是填充的弧线（扇形），绘制从圆心到弧线的线段
    if (filled) {
        for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
            u16 x = x_center + (int)(radius * cos(angle));
            u16 y = y_center + (int)(radius * sin(angle));
            OLED_DrawLine(x_center, y_center, x, y, color);
        }
    }

    // 如果需要更宽的弧线（厚度），可以重复绘制不同半径的弧线（向内绘制）
    if (thickness > 1) {
        for (u8 i = 1; i < thickness; i++) {
            u16 offset_radius = radius - i; // 使用减法来确保厚度向内扩展
            x_prev = x_center + (int)(offset_radius * cos(radian_start));
            y_prev = y_center + (int)(offset_radius * sin(radian_start));

            // 绘制具有不同半径的弧线（将弧线向内绘制）
            for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                u16 x = x_center + (int)(offset_radius * cos(angle));
                u16 y = y_center + (int)(offset_radius * sin(angle));
                OLED_DrawLine(x_prev, y_prev, x, y, color);
                x_prev = x;
                y_prev = y;
            }

            // 如果是填充的弧线，绘制扇形
            if (filled) {
                for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                    u16 x = x_center + (int)(offset_radius * cos(angle));
                    u16 y = y_center + (int)(offset_radius * sin(angle));
                    OLED_DrawLine(x_center, y_center, x, y, color);
                }
            }
        }
    }
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

//显示设置风扇运行时间
void OLED_FanTimeSet(u8 x, u8 y, u32 total_seconds, u32 stop_seconds, u8 show_settime)
{
		u32 seconds = 0;
		u8 size = 16, mode = 1;
		char time_str[12] = {0};
		OLED_ShowPicture(x,y,24,24,TIMER1,mode);
		show_settime?(seconds = stop_seconds):(seconds = total_seconds);
		if(seconds<3600)
		{
			snprintf(time_str, sizeof(time_str), "%02d:%02ds", 
			seconds/60,seconds%60);
			OLED_ShowString(x+24, y+4, (uint8_t *)time_str, 16, mode);
		}
		else
		{
			snprintf(time_str, sizeof(time_str), "%02d:%02dm", 
			seconds/3600,(seconds%3600)/60);
			OLED_ShowString(x+24+48, y+4, (uint8_t *)time_str, 16, mode);
		}
		
}

//显示轮询一次消耗的时间，单位是us
static uint32_t loop_count = 0;
static uint64_t total_elapsed_cycles = 0;
static uint32_t last_tick = 0;
static	uint32_t start_time = 0;

void ShowRunTime(u8 x, u8 y)
{

	uint32_t elapsed_cycles = (DWT->CYCCNT - start_time) & 0xFFFFFFFF;
	total_elapsed_cycles += elapsed_cycles;
	loop_count++;
	uint32_t current_tick = HAL_GetTick();
	if (current_tick - last_tick >= 1000)
	{
			last_tick = current_tick;
			uint32_t avg_us = (total_elapsed_cycles / loop_count)/SYSClock_MHZ;
		if(avg_us>10000)
		{
			OLED_ShowString(x, y, "T:", 8, 1);
			OLED_ShowNum(x+12, y, avg_us/1000, 4, 8, 1);
			OLED_ShowString(x+36, y, "m", 8, 1);
		}
		else
		{
			OLED_ShowString(x, y, "T:", 8, 1);
			OLED_ShowNum(x+12, y, avg_us, 4, 8, 1);
			OLED_ShowString(x+36, y, "u", 8, 1);
		}

			total_elapsed_cycles = 0;
			loop_count = 0;
	}
	start_time = DWT->CYCCNT;
	
}

//显示当前系统运行时间
void Show_SysTime(u8 x, u8 y)
{
		RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    static uint32_t last_seconds = 0;
    char time_str[12] = {0};
		//获取RTC时间
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		
		//计算运行时间
		uint32_t total_seconds = sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds;
		
		//只有当时间发生改变才刷新显示
		if(total_seconds != last_seconds)
		{
				last_seconds = total_seconds;
				if(sTime.Hours<10)
				{
					snprintf(time_str, sizeof(time_str), "%01d:%02d:%02d", 
					sTime.Hours, sTime.Minutes, sTime.Seconds);
					OLED_ShowString(x, y, (uint8_t *)time_str, 12, 1);
				}
				else
				{
					snprintf(time_str, sizeof(time_str), "%02d:%02dm", 
					sTime.Hours, sTime.Minutes);
					OLED_ShowString(x, y, (uint8_t *)time_str, 12, 1);
				}
		}
}
