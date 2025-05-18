#ifndef __KEY_H
#define __KEY_H

#include "main.h"


#define DEBOUNCE_TIME     20     // ms
#define LONG_PRESS_TIME   1000   // ms
#define CLICK_INTERVAL    250    // ms


typedef enum {
    KEY_IDLE,
    KEY_PRESS_DEBOUNCE,
    KEY_PRESSED,
    KEY_RELEASE_DEBOUNCE
} KeyScanState;

typedef enum {
    KEY_EVENT_NONE,
    KEY_EVENT_SINGLE,
    KEY_EVENT_DOUBLE,
    KEY_EVENT_TRIPLE,
    KEY_EVENT_LONG
} KeyEvent;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    KeyScanState state;
    uint32_t last_tick;
    uint8_t click_count;
    uint8_t pressed;
    uint8_t long_press_reported;
    void (*callback)(KeyEvent);
} KeyHandle;

extern KeyHandle key1;

void Key_Scan_Task(void *arg);
void MyKeyEventCallback(KeyEvent event);

#endif

