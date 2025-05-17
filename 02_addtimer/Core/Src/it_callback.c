#include "it_callback.h"
#include "adc.h"


__IO uint16_t ADC_value; //AD采集值
volatile  uint64_t counter = 0;	//倒计时计数，单位ms


//ADC采集中断回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   ADC_value = HAL_ADC_GetValue(&hadc1);
}

//TIM定时器触发回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
		{
        counter++;
			
    }
}


