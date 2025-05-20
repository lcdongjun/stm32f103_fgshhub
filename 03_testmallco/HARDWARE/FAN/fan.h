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

// GPIO模式映射函数
FanScanState GetFanModeFromGPIO(void);

// 风扇扫描任务（周期轮询）
void Fan_Scan_Task(void *arg);

// 设置目标风扇模式
void Set_Fan_State(FanHandle *fan, FanScanState target_mode);

// 按键翻转任务（非阻塞）
void FanKeyToggleTask(FanHandle *fan);

//用户自定义的回调函数
void MyFanEventCallback(FanScanState read_state);
#endif

