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
		void (*update_func)(void *);// 更新函数
    void *data;           // 指向组件实际显示数据的指针
	
		uint8_t twinkle;               // 是否启用闪烁
    uint8_t twinkle_hidden;        // 当前是否隐藏（内部逻辑使用）
    uint32_t twinkle_interval;     // 闪烁间隔（毫秒）
    uint32_t twinkle_tick;         // 上次切换时间
	
} UIElement;


typedef struct {
    uint8_t level;         // 档位
    uint8_t frame_index;   // 动画帧序号
    uint16_t x, y;          // 坐标
		uint8_t pause;					//是否暂停运行
		uint32_t last_tick;
} DisplayData;

typedef struct {
    uint16_t x, y;          // 坐标
		u32 total_seconds;			//倒计时
		u32 stop_seconds;				//设置的时间
    uint8_t stop;         // 控制是否跳过绘制
	  u32 width;
    u32 height;
		u32 show_settime;			//是否显示设置的时间
} FanTimeSetData;

//创建设置运行时间的组件
extern FanTimeSetData fantimeset;
extern UIElement fantimeset_element;

extern DisplayData systime;
extern UIElement systime_element;

extern DisplayData runtime;//建立的耗时的数据
extern UIElement runtime_element;//注册的显示耗时的组件

extern DisplayData fan1_display;//建立的风扇数据
extern UIElement fan1_element;		//注册的风扇组件

void UI_UpdateAll(void);
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
//更新设置风扇运行时间的组件
void FanTimeSetFrameUpdate(void *arg);
//注册风扇的组件
void DrawFanElement(void *data);
//更新风扇的组件
void FanFrameUpdate(void *arg);
//注册设置风扇运行时间的显示组件
void FanTimeSetElement(void *date);
//设置闪烁
void UI_EnableTwinkle(UIElement *elem, uint8_t enable, uint32_t interval);

#endif // __LIGHT_UI_H

