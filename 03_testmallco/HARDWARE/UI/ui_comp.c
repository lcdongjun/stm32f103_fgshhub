#include "ui_comp.h"
#include "oled.h"
#include "bmp.h"
#include "math.h"

void Paint_Show_Arc(u16 x_center, u16 y_center, u16 radius, u16 start_angle, u16 end_angle, u8 thickness, u16 color, u8 filled, u8 invPix)
{

    // ���Ƕ�ת��Ϊ����
    float radian_start = start_angle * 3.14159 / 180.0;
    float radian_end = end_angle * 3.14159 / 180.0;

    // ���ò�����С�����ƻ��Ƶ�ƽ����
    float angle_step = 0.002; // ÿ�ε����Ļ��ȣ�ֵԽСԽƽ��

    // ��ʼλ��
    u16 x_prev = x_center + (int)(radius * cos(radian_start));
    u16 y_prev = y_center + (int)(radius * sin(radian_start));

    // ���ƻ���
    for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
        // ʹ�����Ǻ�������Բ�ϵ������
        u16 x = x_center + (int)(radius * cos(angle));
        u16 y = y_center + (int)(radius * sin(angle));

        // ʹ�û��ߺ����������ڵĵ㣬�γɻ���
        OLED_DrawLine(x_prev, y_prev, x, y, color);

        // ����ǰһ���������
        x_prev = x;
        y_prev = y;
    }

    // ��������Ļ��ߣ����Σ������ƴ�Բ�ĵ����ߵ��߶�
    if (filled) {
        for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
            u16 x = x_center + (int)(radius * cos(angle));
            u16 y = y_center + (int)(radius * sin(angle));
            OLED_DrawLine(x_center, y_center, x, y, color);
        }
    }

    // �����Ҫ����Ļ��ߣ���ȣ��������ظ����Ʋ�ͬ�뾶�Ļ��ߣ����ڻ��ƣ�
    if (thickness > 1) {
        for (u8 i = 1; i < thickness; i++) {
            u16 offset_radius = radius - i; // ʹ�ü�����ȷ�����������չ
            x_prev = x_center + (int)(offset_radius * cos(radian_start));
            y_prev = y_center + (int)(offset_radius * sin(radian_start));

            // ���ƾ��в�ͬ�뾶�Ļ��ߣ����������ڻ��ƣ�
            for (float angle = radian_start + angle_step; angle <= radian_end; angle += angle_step) {
                u16 x = x_center + (int)(offset_radius * cos(angle));
                u16 y = y_center + (int)(offset_radius * sin(angle));
                OLED_DrawLine(x_prev, y_prev, x, y, color);
                x_prev = x;
                y_prev = y;
            }

            // ��������Ļ��ߣ���������
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
