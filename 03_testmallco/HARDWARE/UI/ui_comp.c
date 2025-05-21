#include "ui_comp.h"
#include "math.h"

#include "rtc.h"

#include "oled.h"
#include "bmp.h"

void Paint_Show_Arc(u16 x_center, u16 y_center, u16 radius, u16 start_angle, u16 end_angle, u8 thickness, u16 color, u8 filled, u8 invPix)
{

    // ���Ƕ�ת��Ϊ����
    float radian_start = start_angle * 3.14159 / 180.0;
    float radian_end = end_angle * 3.14159 / 180.0;

    // ���ò�����С�����ƻ��Ƶ�ƽ����
    float angle_step = 0.002; // ÿ�ε����Ļ��ȣ�ֵԽСԽƽ��

    // ��ʼλ��
    u16 x_prev = x_center + (int)(radius * cos(radian_start));
    u16 y_prev = y_center + (int)(radius * sin(radian_start));

    // ���ƻ���
    for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
        // ʹ�����Ǻ�������Բ�ϵ������
        u16 x = x_center + (int)(radius * cos(angle));
        u16 y = y_center + (int)(radius * sin(angle));

        // ʹ�û��ߺ����������ڵĵ㣬�γɻ���
        OLED_DrawLine(x_prev, y_prev, x, y, color);

        // ����ǰһ���������
        x_prev = x;
        y_prev = y;
    }

    // ��������Ļ��ߣ����Σ������ƴ�Բ�ĵ����ߵ��߶�
    if (filled) {
        for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
            u16 x = x_center + (int)(radius * cos(angle));
            u16 y = y_center + (int)(radius * sin(angle));
            OLED_DrawLine(x_center, y_center, x, y, color);
        }
    }

    // �����Ҫ����Ļ��ߣ���ȣ��������ظ����Ʋ�ͬ�뾶�Ļ��ߣ����ڻ��ƣ�
    if (thickness > 1) {
        for (u8 i = 1; i < thickness; i++) {
            u16 offset_radius = radius - i; // ʹ�ü�����ȷ�����������չ
            x_prev = x_center + (int)(offset_radius * cos(radian_start));
            y_prev = y_center + (int)(offset_radius * sin(radian_start));

            // ���ƾ��в�ͬ�뾶�Ļ��ߣ����������ڻ��ƣ�
            for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                u16 x = x_center + (int)(offset_radius * cos(angle));
                u16 y = y_center + (int)(offset_radius * sin(angle));
                OLED_DrawLine(x_prev, y_prev, x, y, color);
                x_prev = x;
                y_prev = y;
            }

            // ��������Ļ��ߣ���������
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

//��ʾ���Ȳ���ת
void ShowFanStatus(uint8_t x, uint8_t y, uint8_t level, uint8_t fan_frame_idx)
{
    // ��ʾ����ͼ�꣨24x24��
    OLED_ShowPicture(x, y, 24, 24, FAN_FRAMES[fan_frame_idx], 1);

    // ���ݵ�λ��ʾ��ͬ�����ķ����
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

//��ʾ���÷�������ʱ��
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

//��ʾ��ѯһ�����ĵ�ʱ�䣬��λ��us
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

//��ʾ��ǰϵͳ����ʱ��
void Show_SysTime(u8 x, u8 y)
{
		RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    static uint32_t last_seconds = 0;
    char time_str[12] = {0};
		//��ȡRTCʱ��
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		
		//��������ʱ��
		uint32_t total_seconds = sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds;
		
		//ֻ�е�ʱ�䷢���ı��ˢ����ʾ
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
