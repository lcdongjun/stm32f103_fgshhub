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
#include "key.h"

#include "light_task.h"
#include "light_ui.h"
#include "ui_comp.h"

#include "oled.h"
#include "bmp.h"

uint16_t fan_cont = 0, bat_lev_cont = 5 , temp = 0; //风扇旋转角度，电量档位，温度
static uint32_t timer_seconds = 0;									//需要运行的时间，s
bool Time_Task_Run = 0;											//是否开启定时关闭
float Real_value; 													//ADC获取的值

uint8_t fan_mode = 0 , set_fan_mode = 0, Set_State = 0;		//读取的风扇档位，要设置的风扇档位，设置选项
static FanSetState fan_set_state = FAN_IDLE;//风扇设置档位的状态
static uint32_t fan_timer = 0;							//开始设置风扇档位的时间
static uint8_t fan_target_mode = 0;					//要设置的档位，使用二进制 000	001	010	100			
static uint8_t fan_retry_count = 0;					//重复尝试设置的次数


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
    uint8_t invert = 0;

    if (run_flag == 0) {
        if (Set_State == 1) {
            // 设置状态下未运行，显示当前时间
            timer_seconds = __HAL_TIM_GET_COUNTER(&htim1);
            show_time = timer_seconds;
            invert = 0;
        }
				else{
					invert = 1;
				}
    } else {
        if (Set_State != 1) {
            // 正常运行中，显示剩余倒计时
            show_time = (timer_seconds > counter) ? (timer_seconds - counter) : 0;
            invert = 1;
        }
    }

    OLED_ShowTimer(10, 40, show_time, 16, invert);
}

//采集并显示电池电量
void ShowBATLev_Task(void *arg)
{
	HAL_ADC_Start_IT(&hadc1);
	
	static NonBlockingDelay delay;

	if (!delay.active) {
			DelayStart(&delay, 100);
			return;
	}

	if (!DelayIsExpired(&delay)) {
			return;
	}
	Real_value=(float)(ADC_value)/2048*3.3;
	printf("The ADC_value is %d",ADC_value);
	printf("The Real_value is %f\n\n",Real_value); 
  OLED_ShowPicture(96,0,32,16,BAT_LEVELS[bat_lev_cont],1);
}

//采集并显示温度
void ShowTEMP_Task(void *arg)
{
	temp = (uint16_t)DS18B20_Get_Temp();
  OLED_ShowPicture(42,3,16,16,TEMP_ICO,1);
  OLED_ShowNum(56,2,temp,3,16,1);
  OLED_ShowPicture(72,4,12,12,TEMP_C,1);
}

//根据编码器显示风扇档数
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

//用于测试不阻塞延时
//	//提供创建一个不阻塞的延时的示范程序
//		static NonBlockingDelay delay;
//		static uint16_t delay_cont = 0;
//    if (!delay.active) {
//        DelayStart(&delay, 1000);
//        return;
//    }

//    if (!DelayIsExpired(&delay)) {
//        return;
//    }
//		
//		delay_cont++;
//		delay_cont%=99;
//		OLED_ShowNum(24,1, delay_cont, 2, 12, 1);
//    OLED_ShowString(1,1, "T:", 12, 1);

//显示当前系统运行时间
void Show_SysTime_Task(void *arg)
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
					OLED_ShowString(0, 0, (uint8_t *)time_str, 12, 1);
				}
				else
				{
					snprintf(time_str, sizeof(time_str), "%02d:%02dm", 
					sTime.Hours, sTime.Minutes);
					OLED_ShowString(0, 0, (uint8_t *)time_str, 12, 1);
				}
		}
}

int32_t count = 0;
//运行获取编码器值来切换风扇档位
void Run_Fan_Task( uint16_t time_interval)
{
	//获取编码器的值，除以12来消抖
	if(Set_State == 0)
	{
	    count = __HAL_TIM_GET_COUNTER(&htim1);
			count = count / 12;
			if(count >=4)
			{
				count = 0;
				__HAL_TIM_SET_COUNTER(&htim1,0);
			}
			set_fan_mode = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
			if(set_fan_mode ==1)
				Set_Fan_State_NonBlocking(count);
			//开始无阻塞的设置风扇档位
			Process_Fan_Set();
	}
		//显示风扇档位
    DelayCall(ShowFAN_Task, (void *)&count, time_interval);
		
//   int32_t count = __HAL_TIM_GET_COUNTER(&htim1);
//    count = count;
//		if(count >=365)
//		{
//			count = 0;
//			__HAL_TIM_SET_COUNTER(&htim1,0);
//		}
//		Paint_Show_Arc(64,32,25,0,count,1,1,0,1);
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

//获取风扇的档位
void Get_GPIO_State()
{
	fan_mode = 0;
	fan_mode |= HAL_GPIO_ReadPin(L1_GPIO_Port,L1_Pin);
	fan_mode |= HAL_GPIO_ReadPin(L2_GPIO_Port,L2_Pin)<<1;
	fan_mode |= HAL_GPIO_ReadPin(L3_GPIO_Port,L3_Pin)<<2;
	printf("fan_mode : 0X%X \r\n",fan_mode);
}

//设置风扇的档位
void Set_Fan_State_NonBlocking(uint8_t count)
{
    switch(count)
		{
        case 0: fan_target_mode = 0x07; break;
        case 1: fan_target_mode = 0x06; break;
        case 2: fan_target_mode = 0x05; break;
        case 3: fan_target_mode = 0x03; break;
        default: return; // 无效输入
    }

    fan_set_state = FAN_SETTING;
    fan_timer = HAL_GetTick();  // 记录当前时间
    fan_retry_count = 0;
}

void Process_Fan_Set(void)
{
    if (fan_set_state == FAN_SETTING)
    {
        if ((HAL_GetTick() - fan_timer) >= 40)  // 每40ms尝试一次
        {
            Get_GPIO_State();
            if (fan_mode == fan_target_mode) {
                printf("Set Fan Mode %d OK\r\n", fan_target_mode);
                fan_set_state = FAN_IDLE;
                set_fan_mode = 0;
            } else {
                HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_RESET);
                HAL_Delay(60);
                HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_SET);
                fan_timer = HAL_GetTick();
                fan_retry_count++;
                if (fan_retry_count > 10) {
                    printf("Failed to set fan mode %d\r\n", fan_target_mode);
                    fan_set_state = FAN_IDLE;
                }
            }
        }
    }
}
