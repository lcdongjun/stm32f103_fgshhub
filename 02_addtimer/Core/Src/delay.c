#include "delay.h"


volatile  uint32_t counter = 0;	
/**********************************************
//Delay_us
**********************************************/

void DWT_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void delay_us(unsigned long us) {
    volatile uint32_t* cyccnt = &DWT->CYCCNT;
    uint32_t start = *cyccnt;
    uint32_t cycles = us * SYSClock_MHZ;
    while ((*cyccnt - start) < cycles);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
		{
				counter%=10;
        counter++;
    }
}

