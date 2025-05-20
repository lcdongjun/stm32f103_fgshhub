#include "key.h"
#include "tim.h"
#include "light_task.h"
#include "all_task.h"


	KeyHandle key1 = {
		.port = GPIOA,
		.pin = GPIO_PIN_0,
		.state = KEY_IDLE,
		.callback = MyKeyEventCallback
};
void Key_Scan_Task(void *arg)
{
    KeyHandle *key = (KeyHandle *)arg;
    uint32_t now = HAL_GetTick();

    uint8_t pin_state = HAL_GPIO_ReadPin(key->port, key->pin);

    switch (key->state) {
        case KEY_IDLE:
            if (pin_state == GPIO_PIN_SET) {
                key->state = KEY_PRESS_DEBOUNCE;
                key->last_tick = now;
            }
            break;

        case KEY_PRESS_DEBOUNCE:
            if ((now - key->last_tick) >= DEBOUNCE_TIME) {
                if (pin_state == GPIO_PIN_SET) {
                    key->state = KEY_PRESSED;
                    key->last_tick = now;
                    key->pressed = 1;
                    key->long_press_reported = 0;
                } else {
                    key->state = KEY_IDLE;
                }
            }
            break;

        case KEY_PRESSED:
            if (pin_state == GPIO_PIN_RESET) {
                key->state = KEY_RELEASE_DEBOUNCE;
                key->last_tick = now;
            } else if ((now - key->last_tick) >= LONG_PRESS_TIME && !key->long_press_reported) {
                key->long_press_reported = 1;
                key->callback(KEY_EVENT_LONG);
                key->click_count = 0;
            }
            break;

        case KEY_RELEASE_DEBOUNCE:
            if ((now - key->last_tick) >= DEBOUNCE_TIME) {
                if (pin_state == GPIO_PIN_RESET) {
                    key->pressed = 0;
                    key->state = KEY_IDLE;
                    if (!key->long_press_reported) {
                        key->click_count++;
                        key->last_tick = now;
                    }
                } else {
                    key->state = KEY_PRESSED;
                }
            }
            break;
    }

    // 判断多击
    if (!key->pressed && key->click_count > 0 && (now - key->last_tick) > CLICK_INTERVAL) {
        switch (key->click_count) {
            case 1: key->callback(KEY_EVENT_SINGLE); break;
            case 2: key->callback(KEY_EVENT_DOUBLE); break;
            case 3: key->callback(KEY_EVENT_TRIPLE); break;
        }
        key->click_count = 0;
    }

    DelayCall(Key_Scan_Task, arg, 10);  // 每10ms运行一次
}

void MyKeyEventCallback(KeyEvent event)
{
    switch (event) {
        case KEY_EVENT_SINGLE:
//						set_fan_mode = 1;
            printf("短按\n");
            break;
        case KEY_EVENT_DOUBLE:
						Set_State++;
						Set_State%=4;
						printf("Set_State : %d\n",Set_State);
            printf("双击\n");
            break;
        case KEY_EVENT_TRIPLE:
            printf("三击\n");
            break;
        case KEY_EVENT_LONG:
						Time_Task_Run = !Time_Task_Run;
						counter = 0;
            (Time_Task_Run) ? HAL_TIM_Base_Start_IT(&htim2) : HAL_TIM_Base_Stop_IT(&htim2);
						printf("长按\n");
            break;
        default:
            break;
    }
}

