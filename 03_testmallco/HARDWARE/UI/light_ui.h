#ifndef __LIGHT_UI_H
#define __LIGHT_UI_H

#include "main.h"

// 注册 UI 元素
#define MAX_UI_ELEMENTS 10


typedef struct
{
	uint8_t current;
	uint8_t up;//向上翻索引号
	uint8_t down;//向下翻索引号
	uint8_t enter;//确认索引号
	uint8_t back;//返回
	void (*current_operation)();
} key_table;

typedef struct {
    const char *name;     // 组件名称

    uint16_t x;            // X坐标
    uint16_t y;            // Y坐标
    uint16_t width;       // 宽度
    uint16_t height;      // 高度

    uint8_t font_size;    // 字体大小
    uint8_t visible;      // 是否显示

		uint8_t stop;
    void (*draw_func)(void *);   // 绘制函数指针
    void *data;           // 指向组件实际显示数据的指针
} UIElement;


typedef struct {
    uint8_t level;         // 档位
    uint8_t frame_index;   // 动画帧序号
    uint16_t x, y;          // 坐标
		uint8_t pause;					//是否暂停运行
} DisplayData;




extern DisplayData runtime;//建立的耗时的数据
extern UIElement runtime_element;//注册的显示耗时的组件

extern DisplayData fan1_display;//建立的风扇数据
extern UIElement fan1_element;		//注册的风扇组件


// 注册一个 UI 元素
void UI_Register(UIElement *elem);
// 绘制所有可见元素
void UI_DrawAll(void *arg);
// 按名称查找元素（可选）
UIElement *UI_Find(const char *name);
// 初始化 UI 系统
void UI_Init(void);
//初始化并注册组件
void UI_Setup_All(void);
//创建循环耗时显示组件
void ShowRunTimeElement(void *date);
//注册系统运行时间显示组件
void ShowSysTimeElement(void *date);
//注册风扇的组件
void DrawFanElement(void *data);
//更新风扇的组件
void FanFrameUpdate(void *arg);

#endif // __LIGHT_UI_H

