#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

#define SPEED_FILTER_WINDOW 5  // �������ڴ�С

typedef struct {
    TIM_HandleTypeDef *htim;

    int16_t last_count;
    int16_t current_count;
    int16_t speed; // ��λ��ticks/s

    uint32_t last_tick;
    uint16_t ppr;     // ÿȦ������
	
	
    int32_t speed_buffer[SPEED_FILTER_WINDOW];
    uint8_t buffer_index;
	
} EncoderWithSpeed;


extern EncoderWithSpeed encoder1;

//��ȡ�������ļ���
int16_t Encoder_Get_Count(void);
//���ñ������ļ���
void Encoder_Reset(void);
//��ȡ�������ķ���
uint8_t Encoder_Get_Direction(void);

void EncoderSpeed_Init(EncoderWithSpeed *enc, TIM_HandleTypeDef *htim, uint16_t ppr);

void EncoderSpeed_Update(void *arg);
#endif

