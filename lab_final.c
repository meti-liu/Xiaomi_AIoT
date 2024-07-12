#include <stdio.h>
#include "delay.h"
#include "e1.h"
#include "e2.h"
#include "e3.h"
#include "s1.h"
#include "s2.h"
#include "s5.h"
#include "s6.h"

// 定义模式状态
enum mode_t {
    INIT,
    NFC_CHECK,
    MODE_SELECTION,
    MODE1,
    MODE2,
    MODE3,
    MODE4,
    MODE5,
    DETECT_FRONT,
    DETECT_LEFT,
    DETECT_RIGHT,
    DETECT_BACK
};
enum mode_t mode = INIT;
static float ll = 1500;//与终点距离先设置为150
char buf[10] = {0};
unsigned int distance = 0;


unsigned char position = 50;
unsigned char speed = 50;

char key = 0;
char display_mode = 'P'; //position,speed
s2_imu_t imu_value;
s2_ths_t ths_value;
unsigned int illuminance = 0;//光照强度

// 前向声明
void update_display(char* message);
void update_display_distance(unsigned int distance);
void mode2_traffic_light_simulation();
void adjust_speed_and_position_based_on_distance(unsigned int distance);//模式一，避障模式
void mode3_direction_control(char key);
void mode4_environment_monitoring(char key);
void mode5_distance_tracking();

int main() 
{
    // 初始化设备
    e1_tube_info = e1_tube_init();
    e1_led_info = e1_led_init(); // LED灯初始化
    e2_fan_info = e2_fan_init();
    e3_curtain_info = e3_curtain_init();
    s1_key_info = s1_key_init();
    s2_imu_info = s2_imu_init(); // S2六轴传感器初始化
    s2_ths_info = s2_ths_init(); // S2温湿度传感器初始化
    s2_illuminance_info = s2_illuminance_init(); // S2光照强度传感器初始化
    s5_nfc_info = s5_nfc_init(); // NFC初始化
    s6_ultrasonic_info = s6_ultrasonic_init();



    update_display("HELO");
    e3_curtain_position_set(e3_curtain_info, position);
    e2_fan_speed_set(e2_fan_info, speed);       

    while (1) //主循环，模式选择
    {
        key = s1_key_value_get(s1_key_info);//识别S1按键，进入不同模式


        ll -= (float)speed / 40.0;//距离跟随speed而减小
        if (key == '#') 
        {
            mode = MODE_SELECTION;
            update_display("MODE");
            delay_ms(500);
            continue;
        }

        if (mode == INIT) 
        {
            update_display("HELO");
            mode = MODE_SELECTION;
        } 
        else if (mode == MODE_SELECTION) 
        {
            if (key == '1') 
            {
                mode = MODE1;
                update_display("01");
                delay_ms(1000);
            } 
            else if (key == '2') 
            {
                mode = MODE2;
                update_display("02");
                delay_ms(1000);
            } 
            else if (key == '3') 
            {
                mode = MODE3;
                update_display("03");
                delay_ms(1000);
            } 
            else if (key == '4') 
            {
                mode = MODE4;
                update_display("04");
                delay_ms(1000);
            } 
            else if (key == '5') 
            {
                mode = MODE5;
                update_display("05");
                delay_ms(1000);
            }

        } 
        else if (mode == MODE1) //避障下的四个方向选择
        {
            if (key == '2') {
                mode = DETECT_FRONT;
                update_display("F");
                delay_ms(2000);
            } else if (key == '4') {
                mode = DETECT_LEFT;
                update_display("L");
                delay_ms(2000);
            } else if (key == '6') {
                mode = DETECT_RIGHT;
                update_display("R");
                delay_ms(2000);
            } else if (key == '8') {
                mode = DETECT_BACK;
                update_display("B");
                delay_ms(2000);
            }
        }

        else if (mode == MODE2) mode2_traffic_light_simulation();
        else if (mode == MODE3) mode3_direction_control(key);
        else if (mode == MODE4) mode4_environment_monitoring(key);
        else if (mode == MODE5) mode5_distance_tracking();
        

        else if (mode == DETECT_FRONT || mode == DETECT_LEFT || mode == DETECT_RIGHT || mode == DETECT_BACK) 
        {
            distance = s6_ultrasonic_distance_get(s6_ultrasonic_info);
            update_display_distance(distance);
            adjust_speed_and_position_based_on_distance(distance);
        }

        delay_ms(50);

    }

}

void update_display(char* message) 
{
    char buf[5] = {0}; // 四位数码管显示
    snprintf(buf, sizeof(buf), "%s", message);
    e1_tube_str_set(e1_tube_info, buf);
}

void update_display_distance(unsigned int distance) 
{
    char buf[5] = {0}; // 四位数码管显示
    snprintf(buf, sizeof(buf), "%4d", distance);
    e1_tube_str_set(e1_tube_info, buf);
}

void adjust_speed_and_position_based_on_distance(unsigned int distance) //避障
{
    if (distance < 450) 
    {
        e1_led_rgb_set(e1_led_info, 100, 0, 0); // 红灯快速闪烁
        delay_ms(50);
        e1_led_rgb_set(e1_led_info, 0, 0, 0);
        delay_ms(50);

        if (mode == DETECT_FRONT) 
        {
            speed = 0; // 减速
            e2_fan_speed_set(e2_fan_info, speed);
        } else if (mode == DETECT_BACK) 
        {
            speed = 100; // 加速
            e2_fan_speed_set(e2_fan_info, speed);
        } else if (mode == DETECT_LEFT) 
        {
            position = 0; // 向右转
            e3_curtain_position_set(e3_curtain_info, position);
        } else if (mode == DETECT_RIGHT) 
        {
            position = 100; // 向左转
            e3_curtain_position_set(e3_curtain_info, position);
        }
    } else if (distance < 700) 
    {
        e1_led_rgb_set(e1_led_info, 100, 100, 0); // 黄灯闪烁
        delay_ms(50);
        e1_led_rgb_set(e1_led_info, 0, 0, 0);
        delay_ms(50);
        
        // 恢复到默认状态
        speed = 50;
        position = 50;
        e2_fan_speed_set(e2_fan_info, speed);
        e3_curtain_position_set(e3_curtain_info, position);
    } 
    else 
    {
        e1_led_rgb_set(e1_led_info, 0, 100, 0); // 绿灯常亮
        
        // 恢复到默认状态
        speed = 50;
        position = 50;
        e2_fan_speed_set(e2_fan_info, speed);
        e3_curtain_position_set(e3_curtain_info, position);
    }
}

void mode2_traffic_light_simulation() //红绿灯
{
    // 模拟交通灯的逻辑实现
    for (int i = 5; i > 0; i--) 
    {
        snprintf(buf, sizeof(buf), "%d---", i); // 显示绿色倒计时
        update_display(buf);
        e1_led_rgb_set(e1_led_info, 0, 100, 0); 
        delay_ms(1000);
    }
    for (int i = 3; i > 0; i--) 
    {
        snprintf(buf, sizeof(buf), "%d---", i); // 显示黄色倒计时
        update_display(buf);
        e1_led_rgb_set(e1_led_info, 100, 100, 0); 
        speed=25;
        e2_fan_speed_set(e2_fan_info, speed);
        delay_ms(1000);
    }
    for (int i = 5; i > 0; i--) 
    {
        snprintf(buf, sizeof(buf), "----", i); // 显示红色倒计时
        update_display(buf);
        e1_led_rgb_set(e1_led_info, 100, 0, 0); 
        speed=0;
        e2_fan_speed_set(e2_fan_info, speed);
        delay_ms(1000);
    }
    for (int i = 5; i > 0; i--) 
    {
    snprintf(buf, sizeof(buf), "----", i); // 显示绿色倒计时
    update_display(buf);
    e1_led_rgb_set(e1_led_info, 0, 100, 0); 
    speed=50;
    e2_fan_speed_set(e2_fan_info, speed);

    delay_ms(1000);
    }

    mode = MODE_SELECTION;
    update_display("MODE");
}
void mode3_direction_control(char key) //手动控制模式
{
    // 切换显示模式
    if (key == '1') display_mode = 'P';
    else if (key == '2') display_mode = 'S';
    

    // 读取加速度传感器的值
    imu_value = s2_imu_value_get(s2_imu_info);


    // 根据加速度值调整汽车的方向和速度
    if (imu_value.acc_y > 3) 
    {
        position += 2; // 向左转
        if (position > 99&&position<120) position = 99; // 限制最大值为99
    } else if (imu_value.acc_y < -3) 
    {
        position -= 2; // 向右转
        if (position < 0||position>200) position = 0; // 限制最小值为0
    }

    if (imu_value.acc_x > 5) 
    {
        speed += 5; // 加速
        if (speed > 99&&speed<120) speed = 99; // 限制最大值为99
    } 
    else if (imu_value.acc_x > 0) 
    {
        speed += 2; // 加速
        if (speed > 99&&speed<120) speed = 99; 
    } 
    else if (imu_value.acc_x < -5) 
    {
        speed -= 5; // 减速
        if (speed < 0||speed>200) speed = 0; // 限制最小值为0
    } 
    else if (imu_value.acc_x < 0) 
    {
        speed -= 2; // 减速
        if (speed < 0||speed>200) speed = 0; 
    }

    // 更新汽车的方向和速度
    e3_curtain_position_set(e3_curtain_info, position);
    e2_fan_speed_set(e2_fan_info, speed);

    // 更新显示
    if (display_mode == 'P') 
    {
        snprintf(buf, sizeof(buf), "%02d", position);//方向盘位置显示
        e1_led_rgb_set(e1_led_info, position, 0, position); 

    } else if (display_mode == 'S') 
    {
        snprintf(buf, sizeof(buf), "%02d", speed);
        e1_led_rgb_set(e1_led_info, 0, speed, speed); //速度显示
    }
    e1_tube_str_set(e1_tube_info, buf);

    // 延时50ms
    //delay_ms(50);
}

void mode4_environment_monitoring(char key) 
{
    s2_ths_t ths_value;
    unsigned int illuminance;

    if (key == '1') display_mode = 'A';
    else if (key == '2') display_mode = 'B';
    else if (key == '3') display_mode = 'C';


    if (display_mode == 'A') 
    {
        ths_value = s2_ths_value_get(s2_ths_info);
        snprintf(buf, sizeof(buf), "T:%2.1f", ths_value.temp);//温度显示
        e1_led_rgb_set(e1_led_info, 30, 70, 50); 
    } 
    else if (display_mode == 'B')
    {
        ths_value = s2_ths_value_get(s2_ths_info);
        snprintf(buf, sizeof(buf), "H:%2.1f", ths_value.humi);//湿度显示
       e1_led_rgb_set(e1_led_info, 20, 20, 70); 
    } 
    else if (display_mode == 'C') 
    {
        illuminance = s2_illuminance_value_get(s2_illuminance_info);
        int brightness = 100 - (int)((illuminance * 100) / 1000);//光照强度以百分比形式显示
        if (brightness < 0) brightness = 0;
        if (brightness > 100) brightness = 100;
        snprintf(buf, sizeof(buf), "L:%4d", 100-brightness);
        e1_led_rgb_set(e1_led_info, brightness, 0, brightness); 
    }

    e1_tube_str_set(e1_tube_info, buf);
	delay_ms(50);
}

void mode5_distance_tracking() //距离检测模式
{


    if (ll <= 0) 
    {
        update_display("DONE");
        while (1); // 终止大循环
    }

    snprintf(buf, sizeof(buf), "D:%3.0f", ll);
    e1_tube_str_set(e1_tube_info, buf);

    delay_ms(500);
}











