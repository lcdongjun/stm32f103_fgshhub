#ifndef __BMP_H
#define __BMP_H


#include "main.h"

//电池 32*16
extern const unsigned char BAT_C[];
extern const unsigned char BAT_0[];
extern const unsigned char BAT_20[];
extern const unsigned char BAT_40[];
extern const unsigned char BAT_60[];
extern const unsigned char BAT_80[];
extern const unsigned char BAT_100[];
//空白进度 24*12
extern const unsigned char BAR_AIR[];
//实心进度	 24*12
extern const unsigned char BAR_SOLID[];

//风扇0 24*24
extern const unsigned char FAN0[];
//风扇15 24*24
extern const unsigned char FAN15[];
//风扇30 24*24
extern const unsigned char FAN30[];
//风扇45 24*24
extern const unsigned char FAN45[];
//风扇60 24*24
extern const unsigned char FAN60[];
//风扇75 24*24
extern const unsigned char FAN75[];

//计时器 24*24
extern const unsigned char TIMER1[];
//温度单位℃ 16*16
extern const unsigned char	TEMP_C[];
//温度计 16*16
extern const unsigned char	TEMP_ICO[];
extern const unsigned char* BAT_LEVELS[];

extern const unsigned char* FAN_FRAMES[];


#endif


