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

uint8_t fan_mode = 0 , set_fan_mode = 0;
uint16_t fan_cont = 4, bat_lev_cont = 0 , temp = 0;
uint32_t timer_seconds = 0;
bool Time_Task_Run = 0;
float Real_value; //实际电压值



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

//显示运行时间
void ShowTime_Task(void *arg)
{
	uint8_t run_flag = *(uint8_t *)arg;
	if(run_flag == 1)
	{
		timer_seconds = counter;
    if (timer_seconds >= 59990)
        timer_seconds = 0;
    OLED_ShowTimer(10, 40, timer_seconds/10, 16, 1);
	}
	else
	{
		OLED_ShowTimer(10, 40, 0, 16, 1);
	}
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

//运行获取编码器值来切换风扇档位
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
		set_fan_mode = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		if(set_fan_mode ==1)
			Set_Fan_State(count);
		
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

//运行切换显示运行时间函数
void Run_ShowTime_Task(uint16_t time_interval)
{
	//检测编码器按键是否按下，按下就切换显示时间状态
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1)
		{
			Time_Task_Run = !Time_Task_Run;
			counter = 0;
			(Time_Task_Run) ? HAL_TIM_Base_Start_IT(&htim2) : HAL_TIM_Base_Stop_IT(&htim2);
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

//获取风扇的档位
void Get_GPIO_State()
{
	fan_mode = 0;
	fan_mode |= HAL_GPIO_ReadPin(L1_GPIO_Port,L1_Pin);
	fan_mode |= HAL_GPIO_ReadPin(L2_GPIO_Port,L2_Pin)<<2;
	fan_mode |= HAL_GPIO_ReadPin(L3_GPIO_Port,L3_Pin)<<3;
}

//设置风扇的档位
void Set_Fan_State(uint8_t count)
{
		printf("set fan mode\r\n");
		switch(count)
		{
			case 0:
				while(fan_mode != 0x00)
				{
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_SET);
						HAL_Delay(20);
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_RESET);
						HAL_Delay(20);
						Get_GPIO_State();
						printf("fan_mode: %d",fan_mode);
				}
				set_fan_mode = 0;
				printf("Set Fan Mode 0 OK \r\n");
				break;
			case 1:
				while(fan_mode != 0x01)
				{
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_SET);
						HAL_Delay(20);
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_RESET);
						HAL_Delay(20);
						Get_GPIO_State();
						printf("fan_mode: %d",fan_mode);
				}
				set_fan_mode = 0;
				printf("Set Fan Mode 1 OK \r\n");
				break;
			case 2:
				while(fan_mode != 0x04)
				{
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_SET);
						HAL_Delay(20);
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_RESET);
						HAL_Delay(20);
						Get_GPIO_State();
						printf("fan_mode: %d",fan_mode);
				}
				set_fan_mode = 0;
				printf("Set Fan Mode 2 OK \r\n");
				break;
			case 3:
				while(fan_mode != 0x08)
				{
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_SET);
						HAL_Delay(20);
						HAL_GPIO_WritePin(KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_RESET);
						HAL_Delay(20);
						Get_GPIO_State();
						printf("fan_mode: %d",fan_mode);
				}
				set_fan_mode = 0;
				printf("Set Fan Mode 3 OK \r\n");
				break;
				
			default:
				break;
		}    
}
