#include <stdio.h>
#include "delay.h"
#include "e1.h"
#include "s1.h"
#include "e2.h"
#include "e3.h"

// 定义窗帘初始位置和亮度
unsigned char pos = 0, r = 0, g = 0, b = 0,speed=0;
char buf[10] = {0};
int step = 10;

// 函数声明
void adjust_pos();
void adjust_led(char key);
void adjust_speed(char key);
void display(unsigned char val);

int main()
{
    char key = 0;

    // 初始化数码管、按键和窗帘设备
    e1_tube_info = e1_tube_init();
    s1_key_info = s1_key_init();
    e2_fan_info = e2_fan_init();
    e3_curtain_info = e3_curtain_init();
    e1_led_info = e1_led_init();

    // 设置初始窗帘位置
    e3_curtain_position_set(e3_curtain_info, pos);

    // 数码管显示默认值 "HEL0"
    e1_tube_str_set(e1_tube_info, "HEL0");

    while(1)
    {
        // 读取按键键值
        key = s1_key_value_get(s1_key_info);

        // 调整窗帘位置或LED亮度
        if(key == '*') adjust_pos();

        else if(key=='#') adjust_speed(key);

        else if(key >= '1' && key <= '9') adjust_led(key);

        // 延时100ms
        delay_ms(500);
    }
}

void adjust_pos()
{
    // 获取当前窗帘位置
    unsigned char cur_pos = e3_curtain_position_get(e3_curtain_info);

    // 调整窗帘位置
    cur_pos += step;

    // 确保位置在0到100之间
    if (cur_pos >= 100)
    {
        cur_pos = 100;
        step = -10;  // 改变方向
    }
    else if (cur_pos <= 0)
    {
        cur_pos = 0;
        step = 10;   // 改变方向
    }

    // 更新窗帘位置
    e3_curtain_position_set(e3_curtain_info, cur_pos);

    // 显示窗帘位置
    sprintf(buf, "%03d", cur_pos);
    e1_tube_str_set(e1_tube_info, buf);

    // 更新全局位置变量
    pos = cur_pos;
}

void adjust_led(char key)
{
    switch(key)
    {
        case '1':
            if (r < 100) r+=10;
            e1_led_rgb_set(e1_led_info, r, 0, 0);
            display(r);
            break;
        case '2':
            if (g < 100) g+=10;
            e1_led_rgb_set(e1_led_info, 0, g, 0);
            display(g);
            break;
        case '3':
            if (b < 100) b+=10;
            e1_led_rgb_set(e1_led_info, 0, 0, b);
            display(b);
            break;
        case '4':
            if (r > 0) r-=10;
            e1_led_rgb_set(e1_led_info, r, 0, 0);
            display(r);
            break;
        case '5':
            if (g > 0) g-=10;
            e1_led_rgb_set(e1_led_info, 0, g, 0);
            display(g);
            break;
        case '6':
            if (b > 0) b-=10;
            e1_led_rgb_set(e1_led_info, 0, 0, b);
            display(b);
            break;
        case '7':
            for(unsigned char i = 0; i < 100; i+=10)
            {
                e1_led_rgb_set(e1_led_info, i, 0, 0);
                delay_ms(10);
            }
            for(unsigned char i = 100; i > 0; i-=10)
            {
                e1_led_rgb_set(e1_led_info, i, 0, 0);
                delay_ms(10);
            }
            display(r);
            break;
        case '8':
            for(unsigned char i = 0; i < 100; i+=10)
            {
                e1_led_rgb_set(e1_led_info, 0, i, 0);
                delay_ms(10);
            }
            for(unsigned char i = 100; i > 0; i-=10)
            {
                e1_led_rgb_set(e1_led_info, 0, i, 0);
                delay_ms(10);
            }
            display(g);
            break;
        case '9':
            for(unsigned char i = 0; i < 100; i+=10)
            {
                e1_led_rgb_set(e1_led_info, 0, 0, i);
                delay_ms(10);
            }
            for(unsigned char i = 100; i > 0; i-=10)
            {
                e1_led_rgb_set(e1_led_info, 0, 0, i);
                delay_ms(10);
            }
            display(b);
            break;
    }
}

void display(unsigned char val)
{
    sprintf(buf, "%03d", val);
    e1_tube_str_set(e1_tube_info, buf);
}

void adjust_speed(char key)
{
    // 调整风扇转速
    speed += step;

    // 确保速度在0到100之间
    if (speed >= 100)
    {
        speed = 100;
        step = -10;  // 改变方向
    }
    else if (speed <= 0)
    {
        speed = 0;
        step = 10;   // 改变方向
    }

    // 更新风扇转速
    e2_fan_speed_set(e2_fan_info, speed);

    // 显示风扇转速
    sprintf(buf, "S%02d", speed);
    e1_tube_str_set(e1_tube_info, buf);
}


