#include <stdio.h>
#include "u1.h"
#include "delay.h"
#include "e1.h"
#include "e2.h"
#include "s1.h"
#include "s2.h"
#include "s5.h"

// 定义状态
enum status_t
{
    INIT,
    OUT,
    AUTH,
    IN,
    FAIL
};

enum status_t Status = INIT;
char buf[10] = {0};
unsigned char speed = 0;
s2_ths_t ths_value;
s2_imu_t imu_value;
unsigned int illuminance = 0;
unsigned char CardType[2] = {0};
unsigned char CardID[4] = {0};

// 函数声明
void s_init();
void s_out();
void s_auth();
void s_in();
void s_fail();
void changelight(float acc_z);
void change_speed(float t);
void update_led_by_illuminance(unsigned int illuminance);

int main()
{
    // 初始化设备
    e1_tube_info = e1_tube_init();
    s1_key_info = s1_key_init();
    s2_ths_info = s2_ths_init();
    s2_imu_info = s2_imu_init();
    s2_illuminance_info = s2_illuminance_init();
    e2_fan_info = e2_fan_init();
    s5_nfc_info = s5_nfc_init();
    e1_led_info = e1_led_init();

    while(1)
    {
        switch (Status)
        {
        case INIT:
            s_init();
            break;
        case OUT:
            s_out();
            break;
        case AUTH:
            s_auth();
            break;
        case IN:
            s_in();
            break;
        case FAIL:
            s_fail();
            break;
        default:
            Status = INIT;
            break;
        }
    }
}

void s_init()
{
    e1_tube_str_set(e1_tube_info, "INIT");
    delay_ms(1000);
    Status = OUT;
}

void s_out()
{
    e1_tube_str_set(e1_tube_info, "OUT");

    // 获取加速度传感器值并更新LED灯颜色
    imu_value = s2_imu_value_get(s2_imu_info);
    changelight(imu_value.acc_z);

    // 检查按键
    char key = s1_key_value_get(s1_key_info);
    if(key == '1') Status = AUTH;

    delay_ms(100);
}

void s_auth()
{
    e1_tube_str_set(e1_tube_info, "AUTH");
    if(s5_nfc_request(s5_nfc_info, 0x26, CardType) == MI_OK)
    {
        Status = IN;
    }
    else
    {
        Status = FAIL;
    }

    // 检查按键
    char key = s1_key_value_get(s1_key_info);
    if(key == '1') Status = OUT;

    delay_ms(100);
}

void s_in()
{
    // 读取温湿度值
    ths_value = s2_ths_value_get(s2_ths_info);
    int temp_int = (int)ths_value.temp;
    int humi_int = (int)ths_value.humi;
    sprintf(buf, "%02d%% %02d%%", temp_int, humi_int);
    e1_tube_str_set(e1_tube_info, buf);

    // 更新风扇转速
    change_speed(ths_value.temp);

    // 检查按键
    char key = s1_key_value_get(s1_key_info);
    if(key == '1') Status = OUT;

    delay_ms(100);
}

void s_fail()
{
    e1_tube_str_set(e1_tube_info, "FAIL");

    // 获取光照强度传感器值并根据光照强度更新LED亮度
    illuminance = s2_illuminance_value_get(s2_illuminance_info);
    update_led_by_illuminance(illuminance);

    // 检查按键
    char key = s1_key_value_get(s1_key_info);
    if(key == '1') Status = OUT;

    delay_ms(100);
}

void changelight(float acc_z)
{
    int red_brightness = 0;
    int green_brightness = 0;
    int blue_brightness = 0;

    if (acc_z >= -9.8 && acc_z < -3.0)
    {
        red_brightness = (int)((acc_z + 9.8) * 100 / 6.8);
    }
    else if (acc_z >= -3.0 && acc_z <= 3.0)
    {
        green_brightness = (int)((3.0 - acc_z) * 100 / 6.0);
    }
    else if (acc_z > 3.0 && acc_z <= 9.8)
    {
        blue_brightness = (int)((9.8 - acc_z) * 100 / 6.8);
    }

    e1_led_rgb_set(e1_led_info, red_brightness, green_brightness, blue_brightness);
}

void change_speed(float t)
{
    if (t < 25) t = 25;
    if (t > 35) t = 35;
    speed = (unsigned char)((t - 25) * 100 / 10);
    e2_fan_speed_set(e2_fan_info, speed);
}

void update_led_by_illuminance(unsigned int illuminance)
{
    int brightness = 100 - (int)((illuminance * 100) / 1000); // 假设光照强度在0到1000勒克斯范围内
    if (brightness < 0) brightness = 0;
    if (brightness > 100) brightness = 100;
    e1_led_rgb_set(e1_led_info, brightness, brightness, brightness); // 简化为同时调整RGB三个颜色的亮度
}