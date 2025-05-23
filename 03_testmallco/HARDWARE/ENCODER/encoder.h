#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

#define SPEED_FILTER_WINDOW 5  // 滑动窗口大小

typedef struct {
    TIM_HandleTypeDef *htim;

    int16_t last_count;
    int16_t current_count;
    int16_t speed; // 单位：ticks/s

    uint32_t last_tick;
    uint16_t ppr;     // 每圈脉冲数
	
	
    int32_t speed_buffer[SPEED_FILTER_WINDOW];
    uint8_t buffer_index;
	
} EncoderWithSpeed;


extern EncoderWithSpeed encoder1;

//获取编码器的计数
int16_t Encoder_Get_Count(void);
//重置编码器的计数
void Encoder_Reset(void);
//获取编码器的方向
uint8_t Encoder_Get_Direction(void);

void EncoderSpeed_Init(EncoderWithSpeed *enc, TIM_HandleTypeDef *htim, uint16_t ppr);

void EncoderSpeed_Update(void *arg);
#endif

