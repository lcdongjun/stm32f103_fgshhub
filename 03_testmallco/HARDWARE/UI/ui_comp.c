#include "ui_comp.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"

void Paint_Show_Arc(u16 x_center, u16 y_center, u16 radius, u16 start_angle, u16 end_angle, u8 thickness, u16 color, u8 filled, u8 invPix)
{

    // 将角度转换为弧度
    float radian_start = start_angle * 3.14159 / 180.0;
    float radian_end = end_angle * 3.14159 / 180.0;

    // 设置步进大小，控制绘制的平滑度
    float angle_step = 0.002; // 每次递增的弧度，值越小越平滑

    // 初始位置
    u16 x_prev = x_center + (int)(radius * cos(radian_start));
    u16 y_prev = y_center + (int)(radius * sin(radian_start));

    // 绘制弧线
    for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
        // 使用三角函数计算圆上点的坐标
        u16 x = x_center + (int)(radius * cos(angle));
        u16 y = y_center + (int)(radius * sin(angle));

        // 使用画线函数连接相邻的点，形成弧线
        OLED_DrawLine(x_prev, y_prev, x, y, color);

        // 更新前一个点的坐标
        x_prev = x;
        y_prev = y;
    }

    // 如果是填充的弧线（扇形），绘制从圆心到弧线的线段
    if (filled) {
        for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
            u16 x = x_center + (int)(radius * cos(angle));
            u16 y = y_center + (int)(radius * sin(angle));
            OLED_DrawLine(x_center, y_center, x, y, color);
        }
    }

    // 如果需要更宽的弧线（厚度），可以重复绘制不同半径的弧线（向内绘制）
    if (thickness > 1) {
        for (u8 i = 1; i < thickness; i++) {
            u16 offset_radius = radius - i; // 使用减法来确保厚度向内扩展
            x_prev = x_center + (int)(offset_radius * cos(radian_start));
            y_prev = y_center + (int)(offset_radius * sin(radian_start));

            // 绘制具有不同半径的弧线（将弧线向内绘制）
            for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                u16 x = x_center + (int)(offset_radius * cos(angle));
                u16 y = y_center + (int)(offset_radius * sin(angle));
                OLED_DrawLine(x_prev, y_prev, x, y, color);
                x_prev = x;
                y_prev = y;
            }

            // 如果是填充的弧线，绘制扇形
            if (filled) {
                for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                    u16 x = x_center + (int)(offset_radius * cos(angle));
                    u16 y = y_center + (int)(offset_radius * sin(angle));
                    OLED_DrawLine(x_center, y_center, x, y, color);
                }
            }
        }
    }
}
