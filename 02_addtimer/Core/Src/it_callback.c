#include "it_callback.h"
#include "adc.h"


__IO uint16_t ADC_value; //AD�ɼ�ֵ
volatile  uint64_t counter = 0;	//����ʱ��������λms


//ADC�ɼ��жϻص�����
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   ADC_value = HAL_ADC_GetValue(&hadc1);
}

//TIM��ʱ�������ص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
		{
        counter++;
			
    }
}


