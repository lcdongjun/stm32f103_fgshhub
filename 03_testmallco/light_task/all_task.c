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


uint16_t fan_cont = 0, bat_lev_cont = 5 , temp = 0; //������ת�Ƕȣ�������λ���¶�
static uint32_t timer_seconds = 0;									//��Ҫ���е�ʱ�䣬s
bool Time_Task_Run = 0;											//�Ƿ�����ʱ�ر�
float Real_value; 													//ADC��ȡ��ֵ

uint8_t fan_mode = 0 , set_fan_mode = 0, Set_State = 0;		//��ȡ�ķ��ȵ�λ��Ҫ���õķ��ȵ�λ������ѡ��


void AllInit()
{
	//����ϵͳ�����ʼ��
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
	//���¿�����Ļ��ʾ
	OLED_Init();
	OLED_DisPlay_On();
	OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
}

void AllStop()
{
	//�ر���Ļ��ʾ
	OLED_DisPlay_Off();
	//����ϵͳ���ֹͣ
	HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL);
	HAL_ADC_Stop(&hadc1);
	HAL_UART_DeInit(&huart1);
	HAL_SPI_DeInit(&hspi1);
	HAL_I2C_DeInit(&hi2c1);
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	
}

//��ʾ���ȿ�������ʱ
void ShowTime_Task(void *arg)
{
    uint8_t run_flag = *(uint8_t *)arg;
    uint16_t show_time = 0;
    uint8_t invert = 0;

    if (run_flag == 0) {
        if (Set_State == 1) {
            // ����״̬��δ���У���ʾ��ǰʱ��
            timer_seconds = __HAL_TIM_GET_COUNTER(&htim1);
            show_time = timer_seconds;
            invert = 0;
        }
				else{
					invert = 1;
				}
    } else {
        if (Set_State != 1) {
            // ���������У���ʾʣ�൹��ʱ
            show_time = (timer_seconds > counter) ? (timer_seconds - counter) : 0;
            invert = 1;
        }
    }

    OLED_ShowTimer(10, 40, show_time, 16, invert);
}

//�ɼ�����ʾ��ص���
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

//�ɼ�����ʾ�¶�
void ShowTEMP_Task(void *arg)
{
	temp = (uint16_t)DS18B20_Get_Temp();
  OLED_ShowPicture(42,3,16,16,TEMP_ICO,1);
  OLED_ShowNum(56,2,temp,3,16,1);
  OLED_ShowPicture(72,4,12,12,TEMP_C,1);
}

//���ݱ�������ʾ���ȵ���
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

//���ڲ��Բ�������ʱ
//	//�ṩ����һ������������ʱ��ʾ������
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


int32_t count = 0;
//���л�ȡ������ֵ���л����ȵ�λ
void Run_Fan_Task( uint16_t time_interval)
{
	//��ȡ��������ֵ������12������
	if(Set_State == 0)
	{
			count += encoder1.last_count;
			count = count / 12;
			count %= 4;
			if(set_fan_mode == 1)
			{
				set_fan_mode = 0;
				Set_Fan_State(&fan1,(FanScanState)(count+1));
			}
	}
	
		fan1_display.level = count;
		count?(fan1_display.pause=0):(fan1_display.pause=1);
		//��ʾ���ȵ�λ
		DelayCall(FanFrameUpdate, &fan1_display, time_interval);
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
