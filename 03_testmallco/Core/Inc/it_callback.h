#ifndef __IT_CALLBACK_H
#define __IT_CALLBACK_H

#include "main.h"


extern volatile  uint64_t counter;		//����ʱ��������λms
extern __IO uint16_t ADC_value; 			//AD�ɼ�ֵ

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
