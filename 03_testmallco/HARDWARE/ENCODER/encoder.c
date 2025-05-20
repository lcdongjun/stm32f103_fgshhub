#include "encoder.h"
#include "tim.h"


EncoderWithSpeed encoder1;

int16_t Encoder_Get_Count(void)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
}

void Encoder_Reset(void)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
}

uint8_t Encoder_Get_Direction(void)
{
    return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) ? 0 : 1; // 0=反转, 1=正转
}

void EncoderSpeed_Init(EncoderWithSpeed *enc, TIM_HandleTypeDef *htim, uint16_t ppr)
{
    enc->htim = htim;
    enc->ppr = ppr;
    enc->last_count = __HAL_TIM_GET_COUNTER(htim);
    enc->last_tick = HAL_GetTick();
    enc->speed = 0;

    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

void EncoderSpeed_Update(void *arg)
{
    EncoderWithSpeed *enc = (EncoderWithSpeed *)arg;

    enc->current_count = __HAL_TIM_GET_COUNTER(enc->htim);
    uint32_t now = HAL_GetTick();
    uint32_t dt = now - enc->last_tick;

    if (dt >= 100) // 每100ms计算一次
    {
        int16_t delta = enc->current_count - enc->last_count;

        // 考虑计数器溢出
        if (delta > 32767) delta -= 65536;
        else if (delta < -32768) delta += 65536;

        enc->speed = (delta * 1000) / dt; // 速度：tick/s
        enc->last_count = enc->current_count;
        enc->last_tick = now;
    }
}

