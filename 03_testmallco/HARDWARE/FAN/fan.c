#include "fan.h"

FanHandle fan1 = {
    .set_tick = 100,
    .callback = MyFanEventCallback
};


FanScanState GetFanModeFromGPIO(void)
{
    uint8_t bits = 0;
    bits |= HAL_GPIO_ReadPin(L1_GPIO_Port, L1_Pin) << 0;
    bits |= HAL_GPIO_ReadPin(L2_GPIO_Port, L2_Pin) << 1;
    bits |= HAL_GPIO_ReadPin(L3_GPIO_Port, L3_Pin) << 2;

    switch (bits) {
        case 0x07: return FAN_MODE0;
        case 0x06: return FAN_MODE1;
        case 0x05: return FAN_MODE2;
        case 0x03: return FAN_MODE3;
        default:   return FAN_MODE_INVALID;
    }
}

void Fan_Scan_Task(void *arg)
{
    FanHandle *fan = (FanHandle *)arg;
    fan->read_mode = GetFanModeFromGPIO();

    if (fan->set_err) return; // 错误状态，跳过设置

    if (fan->read_mode == fan->set_mode) {
        fan->set_ok = 1;
        fan->fan_err_count = 0;
    } else {
        fan->set_ok = 0;
        if (fan->callback) {
            fan->callback(fan->read_mode);  // 执行设置修正回调
        }
    }
}

void Set_Fan_State(FanHandle *fan, FanScanState target_mode)
{
    if (target_mode >= FAN_MODE0 && target_mode <= FAN_MODE3) {
        fan->set_mode = target_mode;
        fan->set_ok = 0;
        fan->set_err = 0;
        fan->fan_err_count = 0;
        fan->now_tick = HAL_GetTick();
        fan->key_state = FAN_KEY_IDLE;
    }
}

void FanKeyToggleTask(FanHandle *fan)
{
    switch (fan->key_state) {
        case FAN_KEY_IDLE:
            HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_RESET);
            fan->now_tick = HAL_GetTick();
            fan->key_state = FAN_KEY_PRESSING;
            break;

        case FAN_KEY_PRESSING:
            if (HAL_GetTick() - fan->now_tick >= fan->set_tick) {
                HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_SET);
                fan->now_tick = HAL_GetTick();
                fan->key_state = FAN_KEY_WAIT_RELEASE;
            }
            break;

        case FAN_KEY_WAIT_RELEASE:
            if (HAL_GetTick() - fan->now_tick >= fan->set_tick) {
                fan->key_state = FAN_KEY_IDLE;
            }
            break;
    }
}

// 回调函数：尝试模拟按键设置
void MyFanEventCallback(FanScanState read_state)
{
    if (fan1.fan_err_count < 10) {
        FanKeyToggleTask(&fan1);  // 非阻塞翻转一次
        fan1.fan_err_count++;
    } else {
        fan1.set_err = 1;
        fan1.fan_err_count = 0;
        printf("Failed to set fan mode %d -> still read %d\r\n", fan1.set_mode, fan1.read_mode);
    }
}
