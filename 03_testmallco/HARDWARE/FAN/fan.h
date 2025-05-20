#ifndef __FAN_H
#define __FAN_H

#include "main.h"


typedef enum {
    FAN_MODE0 = 1,
    FAN_MODE1,
    FAN_MODE2,
    FAN_MODE3,
    FAN_MODE_INVALID = 0xFF
} FanScanState;

typedef enum {
    FAN_KEY_IDLE,
    FAN_KEY_PRESSING,
    FAN_KEY_WAIT_RELEASE
} FanKeyState;

typedef struct {
    FanScanState read_mode;
    FanScanState set_mode;

    uint8_t set_ok;
    uint8_t set_err;
    uint8_t fan_err_count;

    uint32_t now_tick;
    uint32_t last_tick;
    uint8_t set_tick;

    FanKeyState key_state;

    void (*callback)(FanScanState);
} FanHandle;

extern FanHandle fan1;

// GPIOģʽӳ�亯��
FanScanState GetFanModeFromGPIO(void);

// ����ɨ������������ѯ��
void Fan_Scan_Task(void *arg);

// ����Ŀ�����ģʽ
void Set_Fan_State(FanHandle *fan, FanScanState target_mode);

// ������ת���񣨷�������
void FanKeyToggleTask(FanHandle *fan);

//�û��Զ���Ļص�����
void MyFanEventCallback(FanScanState read_state);
#endif

