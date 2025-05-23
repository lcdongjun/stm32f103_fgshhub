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
    return (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) ? 0 : 1; // 0=��ת, 1=��ת
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

    if (dt >= 100) // ÿ100ms����һ��
    {
        int16_t delta = enc->current_count - enc->last_count;

        // ���Ǽ��������
        if (delta > 32767) delta -= 65536;
        else if (delta < -32768) delta += 65536;

        int32_t raw_speed = (delta * 1000) / dt;

        // ���뻺����
        enc->speed_buffer[enc->buffer_index] = raw_speed;
        enc->buffer_index = (enc->buffer_index + 1) % SPEED_FILTER_WINDOW;

        // ���㻬��ƽ��
        int64_t sum = 0;
        for (int i = 0; i < SPEED_FILTER_WINDOW; i++) {
            sum += enc->speed_buffer[i];
        }
        enc->speed = sum / SPEED_FILTER_WINDOW;

        enc->last_count = enc->current_count;
        enc->last_tick = now;
    }
}

