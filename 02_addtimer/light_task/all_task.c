#include "all_task.h"

#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "delay.h"
#include "ds18b20.h"
#include "oled.h"
#include "bmp.h"
#include "light_task.h"

uint16_t fan_cont = 0, bat_lev_cont = 0 , temp = 0;
uint32_t timer_seconds = 0;
bool Time_Task_Run = 0;



void AllInit()
{
	//所有系统组件初始化
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
//  MX_RTC_Init();
	DWT_Init();
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	//重新开启屏幕显示
	OLED_Init();
	OLED_DisPlay_On();
	OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
}

void AllStop()
{
	//关闭屏幕显示
	OLED_DisPlay_Off();
	//所有系统组件停止
	HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
	HAL_ADC_Stop(&hadc1);
	HAL_UART_DeInit(&huart1);
	HAL_SPI_DeInit(&hspi1);
	HAL_I2C_DeInit(&hi2c1);
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	
}

void ShowTime_Task(void *arg)
{
	uint8_t run_flag = *(uint8_t *)arg;
	printf("run_flag : %d\r\n",run_flag);
	if(run_flag == 1)
	{
		timer_seconds = counter/10;
    if (timer_seconds >= 5999)
        timer_seconds = 0;
    OLED_ShowTimer(10, 40, timer_seconds, 16, 1);
	}
	else
	{
		OLED_ShowTimer(10, 40, 0, 16, 1);
	}
}

void ShowBATLev_Task(void *arg)
{
  bat_lev_cont++;
  bat_lev_cont%=7;
  OLED_ShowPicture(96,0,32,16,BAT_LEVELS[bat_lev_cont],1);
}

void ShowTEMP_Task(void *arg)
{
	temp = (uint16_t)DS18B20_Get_Temp();
  OLED_ShowPicture(42,3,16,16,TEMP_ICO,1);
  OLED_ShowNum(56,2,temp,3,16,1);
  OLED_ShowPicture(72,4,12,12,TEMP_C,1);
}

void ShowFAN_Task(void *arg)
{
		
    uint8_t level = *(uint8_t *)arg;
    static uint8_t frame_skip_counter = 0;
    uint8_t speed_factor = 4 - level;
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
	//提供创建一个不阻塞的延时的示范程序
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

void Run_Fan_Task( uint16_t time_interval)
{
	//获取编码器的值，除以12来消抖
    int32_t count = __HAL_TIM_GET_COUNTER(&htim1);
    count = count / 12;
		if(count >=4)
		{
			count = 0;
			__HAL_TIM_SET_COUNTER(&htim1,0);
		}
    DelayCall(ShowFAN_Task, (void *)&count, time_interval);
}

void Run_ShowTime_Task(uint16_t time_interval)
{
	//检测编码器按键是否按下，按下就切换显示时间状态
	static uint32_t last_press_time = 0;
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1)
		{
				if (HAL_GetTick() - last_press_time > 50)
				{
            Time_Task_Run = !Time_Task_Run;
            counter = 0;
            (Time_Task_Run) ? HAL_TIM_Base_Start_IT(&htim2) : HAL_TIM_Base_Stop_IT(&htim2);
        }
				last_press_time = HAL_GetTick();
		}
		DelayCall(ShowTime_Task, (void *)&Time_Task_Run, time_interval);
}

void Run_Standby_Stop_Task(uint16_t time_interval)
{
			AllStop();
//			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);
//			AllInit();
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
			HAL_PWR_EnterSTANDBYMode();

}
