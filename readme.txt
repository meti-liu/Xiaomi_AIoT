基于AIOT实验平台的自动驾驶仿真系统

使用设备：
1. U1主控
2. S1 按键
3. S2 温湿度、六轴、照明
4. E1 灯
5. E1 数码管
6. E2风扇
7. S6，超声波传感器


E3 窗帘机模拟汽车方向盘，控制左右转向
E2 风扇模拟汽车发动机，其转速直接关联汽车的行驶速度
E1 灯模拟红绿灯，通过不同颜色的灯光变化来指示车辆行驶或停止
E1 数码管模拟车辆仪表盘，显示包括当前速度、方向和环境监测数据在内的各种车辆状态信息
S6 超声波传感器拟车辆的距离监测系统，实时检测周围障碍物并测量车辆与障碍物之间的距离



#  进入模式选择

#1  模式1：避障模式

	# 1 2 前方避障
	# 1 4 左方避障
	# 1 6 右方避障
	# 1 8 后方避障
	
数码管显示该方向上车辆与障碍物之间的距离


#2 红绿灯模式，控制车辆经过十字路口的运动
       
       # 2 1 绿灯行驶
       # 2 2 黄灯减速
       # 2 3 红灯停车


#3 手动操控模式   通过S2传感器模拟方向盘
		
      # 3 1 方向控制
      # 3 2 速度控制
  
 选择方向控制时，数码管显示当前的转向角度
 选择速度控制时，数码管显示当前的速度值


# 4 环境检测模式  

	# 4 1  检测当前温度
	# 4 2  检测当前湿度
	# 4 3  检测外界光照强度，并调整LED灯亮度


# 5 距离检测模式
 
        # 5  实时距离监测
       
数码管显示当前位置与目标地点距离值

