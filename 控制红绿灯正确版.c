#include <stdio.h>
#include "delay.h"
#include "e1.h"
#include "s2.h"

// 定义一个处理LED亮度变化的函数
void changelight(float acc_x, float acc_y, float acc_z)
{
    int red_brightness = 0;
    int green_brightness = 0;
    int blue_brightness = 0;

    red_brightness = (int)((acc_x + 9.8) * 100 / 19.6);
    green_brightness = (int)((acc_y + 9.8) * 100 / 19.6);
    blue_brightness = (int)((acc_z + 9.8) * 100 / 19.6);

    e1_led_rgb_set(e1_led_info, red_brightness, green_brightness, blue_brightness);
}

int main()
{
    char buf[10] = {0};
    s2_imu_t imu_value;

    /* 数码管初始化 */
    e1_tube_info = e1_tube_init();
    /* 加速度&角速度传感器初始化 */
    s2_imu_info = s2_imu_init();
    /* LED初始化 */
    e1_led_info = e1_led_init();

    while(1)
    {
        /* 读取加速度&角速度传感器测量结果（m/s2、rad/s） */
        imu_value = s2_imu_value_get(s2_imu_info);

        /* 将z轴的加速度值转换为字符串并写入buf */
        sprintf(buf, "%.1f", imu_value.acc_z);

        /* 在数码管上显示buf中的字符串 */
        e1_tube_str_set(e1_tube_info, buf);

        /* 根据加速度值调节LED亮度 */
        changelight(imu_value.acc_x, imu_value.acc_y, imu_value.acc_z);

        /* 延时100ms */
        delay_ms(100);
    }
}
