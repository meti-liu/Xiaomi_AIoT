#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "e1.h"
#include "s1.h"

int main()
{
    char key = 0;
    int v0 = 0;
    int v1 = 0;
    char buf[10] = {0};
    char v1_str[10] = ""; // 用于存储新输入的数字字符串
    int index = 0;

    
    e1_tube_info = e1_tube_init();
    s1_key_info = s1_key_init();

    while (1)
    {
     
        key = s1_key_value_get(s1_key_info);

        
        if (key >= '0' && key <= '9')
        {
            // 将按键值添加到新输入的数字字符串中
            if (index < sizeof(v1_str) - 1)
            {
                v1_str[index] = key;
                index++;
                v1_str[index] = '\0'; // 确保字符串以空字符结尾
            }

            v1 = atoi(v1_str); // 将字符串转换为整数,以整数形式存储v1便于后续数字运算

            sprintf(buf, "%d", v1);//再把整数换回字符数组显示在显像管上面
            e1_tube_str_set(e1_tube_info, buf);
            delay_ms(500);  
        }
    
        else if (key == '#')
        {
            
            v0 += v1;

        
            v1 = 0;
            index = 0;
            v1_str[0] = '\0';

     
            sprintf(buf, "%d", v0);
            e1_tube_str_set(e1_tube_info, buf);

            delay_ms(500);  
        }
       
        else if (key == '*')
        {
        
            v0 += v1;

        
            v1 = 0;
            index = 0;
            v1_str[0] = '\0';

           
            sprintf(buf, "%d", v0);
            e1_tube_str_set(e1_tube_info, buf);

            delay_ms(500); 
        }
        delay_ms(500);  // 防止一次按键识别成两次输入
    }
}