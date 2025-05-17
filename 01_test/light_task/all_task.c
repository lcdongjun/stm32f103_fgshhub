#include "all_task.h"
#include "rtc.h"

#include "ds18b20.h"
#include "oled.h"
#include "bmp.h"
#include "light_task.h"

uint16_t fan_cont = 0, bat_lev_cont = 0 , temp = 0;
uint32_t elapsed_time = 0, timer_seconds = 0;

void ShowTime_Task(void *arg)
{
	uint8_t run_flag = *(uint8_t *)arg;
	printf("run_flag : %d\r\n",run_flag);
	if(run_flag == 1)
	{
	  RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    static uint8_t last_seconds = 0;
    uint8_t current_seconds = sTime.Seconds;
    uint8_t diff = (current_seconds - last_seconds) % 60;

    if (diff > 0)
    {
        last_seconds = current_seconds;
        timer_seconds += diff;
    }
    if (timer_seconds >= 5999)
        timer_seconds = 0;
    OLED_ShowTimer(10, 40, timer_seconds, 16, 1);
	}
	else
	{
		OLED_ShowTimer(10, 40, 0, 16, 1);
	}
}

void ShowBATLev_Task(void *arg){
  bat_lev_cont++;
  bat_lev_cont%=7;
  OLED_ShowPicture(96,0,32,16,BAT_LEVELS[bat_lev_cont],1);
}

void ShowTEMP_Task(void *arg){
	temp = (uint16_t)DS18B20_Get_Temp();
  OLED_ShowPicture(42,3,16,16,TEMP_ICO,1);
  OLED_ShowNum(56,2,temp,3,16,1);
  OLED_ShowPicture(72,4,12,12,TEMP_C,1);
}

void ShowFAN_Task(void *arg){
    uint8_t level = *(uint8_t *)arg;
    static uint8_t frame_skip_counter = 0;
    uint8_t speed_factor = 4 - level;
    if (speed_factor == 0) speed_factor = 1;
    frame_skip_counter++;
    if (frame_skip_counter >= speed_factor && level != 0) {
        frame_skip_counter = 0;
        fan_cont++;
        fan_cont %= 6;
    }
    ShowFanStatus(11, 18, level, fan_cont);
}
  
void Test_Delay_Task(void *arg)
{
		static NonBlockingDelay delay;
		static uint16_t delay_cont = 0;
	
    if (!delay.active) {
        DelayStart(&delay, 1000);
        return;
    }

    if (!DelayIsExpired(&delay)) {
        return;
    }
		delay_cont++;
		delay_cont%=99;
		OLED_ShowNum(24,1, delay_cont, 2, 12, 1);
    OLED_ShowString(1,1, "T:", 12, 1);
}

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

void OLED_ShowTimer(u8 x, u8 y, u32 total_seconds, u8 size, u8 mode)
{
    u32 minutes = total_seconds / 60;
    u32 seconds = total_seconds % 60;
		OLED_ShowPicture(x,y,24,24,TIMER1,1);
    OLED_ShowNum(x+28,y+4, minutes, 2, size, mode);
    OLED_ShowString(x + size * 2 * 1-16+28, y-1+4, ":", size, mode);
    OLED_ShowNum(x + size * 2 * 1-8+28, y+4, seconds, 2, size, mode);
}





