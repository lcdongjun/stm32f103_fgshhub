#include "it_callback.h"
#include "adc.h"


__IO uint16_t ADC_value; //AD�ɼ�ֵ
volatile  uint64_t counter = 0;	//����ʱ��������λs
// int8_t usart_rxdate[16] = {0};


//ADC�ɼ��жϻص�����
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   ADC_value = HAL_ADC_GetValue(&hadc1);
}

uint8_t tim2_temp_cont = 0;
//TIM��ʱ�������ص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
		{
        tim2_temp_cont++;
				if(tim2_temp_cont>=10)
				{
					tim2_temp_cont = 0;
					counter++;
				}
    }
}
//USART�����ص�����
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  if (huart->Instance == USART1)
//  {
//		HAL_UART_Receive_IT(huart, (uint8_t*)usart_rxdate, sizeof(usart_rxdate));
//  }
//}

