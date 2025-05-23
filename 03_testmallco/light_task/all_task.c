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


#include "light_task.h"
#include "light_ui.h"
#include "ui_comp.h"

#include "oled.h"
#include "bmp.h"

#include "key.h"
#include "fan.h"
#include "encoder.h"


float temp = 0;
uint16_t fan_cont = 0, bat_lev_cont = 5 ; //风扇旋转角度，电量档位，温度
static uint32_t timer_seconds = 0;									//需要运行的时间，s
bool Time_Task_Run = 0;											//是否开启定时关闭
float Real_value; 													//ADC获取的值

uint8_t fan_mode = 0 , set_fan_mode = 0, Set_State = 0;		//读取的风扇档位，要设置的风扇档位，设置选项


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
  MX_RTC_Init();
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

//显示风扇开启倒计时
void ShowTime_Task(void *arg)
{
    uint8_t run_flag = *(uint8_t *)arg;
    uint16_t show_time = 0;
		static uint32_t showtime_task_last_tik = 0;
    if (run_flag == 0)
		{
        if (Set_State == 2)
				{
						fan1.fan_control_mode = FAN_CTRL_MANUAL;

//            timer_seconds = __HAL_TIM_GET_COUNTER(&htim1);
						if(HAL_GetTick()-showtime_task_last_tik>100)
						{
							timer_seconds += encoder1.speed;
							show_time = timer_seconds;
							fantimeset.stop_seconds = show_time;
							fantimeset.show_settime = 1;
						}
        } else
				{
            fantimeset.total_seconds = timer_seconds;
            fantimeset.show_settime = 0;
        }
    }
		else
		{
        fan1.fan_control_mode = FAN_CTRL_TIMER;

        if (Set_State != 2)
				{
            show_time = (timer_seconds > counter) ? (timer_seconds - counter) : 0;

            if (show_time <= 1)
						{
                Time_Task_Run = 0;
                counter = 0;
                HAL_TIM_Base_Stop_IT(&htim2);
                Set_Fan_State(&fan1, FAN_MODE0);

                fan1_display.level = 0;
                fan1_display.pause = 1;
                fan1.fan_control_mode = FAN_CTRL_IDLE;

                printf("Fan Stop! \r\n");
            }
            fantimeset.total_seconds = show_time;
            fantimeset.show_settime = 0;
        }
    }
}

//采集并显示电池电量
void ShowBATLev_Task(void *arg)
{
	HAL_ADC_Start_IT(&hadc1);
	float Battery_voltage = 0.0f;
	static NonBlockingDelay delay;

	if (!delay.active) {
			DelayStart(&delay, 100);
			return;
	}

	if (!DelayIsExpired(&delay)) {
			return;
	}
	Real_value = (float)(ADC_value) / 4095.0f * 3.3f;
	Battery_voltage = Real_value * 2;
	printf("ADC_value: %d, Real voltage: %.2f V, Battery voltage: %.2f V\n", ADC_value, Real_value, Battery_voltage);
  OLED_ShowPicture(96,0,32,16,BAT_LEVELS[bat_lev_cont],1);
}

int32_t count = 0;
//运行获取编码器值来切换风扇档位
void Run_Fan_Task(void)
{
    static uint32_t run_fan_task_last_tik = 0;

    if ((Set_State == 1 || Set_State == 0) && fan1.fan_control_mode != FAN_CTRL_TIMER) // 不在倒计时运行中
    {
        if (HAL_GetTick() - run_fan_task_last_tik >= 100)
        {
            run_fan_task_last_tik = HAL_GetTick();

            count = encoder1.current_count;
            count = count / 12;
            count %= 4;

            if (set_fan_mode == 1)
            {
                set_fan_mode = 0;
                Set_Fan_State(&fan1, (FanScanState)(count + 1));
            }
            fan1_display.level = count;
            fan1_display.pause = (count == 0);
        }
    }
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


void Twinkle(void *arg)
{
	static uint8_t last_Set_State = 0;
			if(last_Set_State != Set_State)
			{
				last_Set_State = Set_State;
					switch (Set_State)
					{
					case 0:
								UI_EnableTwinkle(&temp_element, 0, 0); 
							break;
					case 1:
								UI_EnableTwinkle(&fan1_element, 1, 400); 
							break;
					case 2:
								UI_EnableTwinkle(&fan1_element, 0, 0); 
								UI_EnableTwinkle(&fantimeset_element, 1, 400); 
							break;
					case 3:
								UI_EnableTwinkle(&fantimeset_element, 0, 0);
								UI_EnableTwinkle(&temp_element, 1, 400); 
							break;
					default:
							break;
					}
			}
}
