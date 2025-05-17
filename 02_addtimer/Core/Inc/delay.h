#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern volatile  uint32_t counter;	

void DWT_Init(void);
void delay_us(unsigned long  us);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


#ifdef __cplusplus
}
#endif



#endif /* __DELAY_H */
