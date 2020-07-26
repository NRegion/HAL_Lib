# HAL_Lib
by NRegion
#### 介绍
STM32库，基于STM32CubeMX

#### 软件架构
##### HAL_BSP为硬件底层驱动
- I2C_Soft：软件I2C
    - 测试未通过
- LED：封装简单的LED操作
- OLED：驱动OLED显示屏
    - 目前只支持0.96寸128*64OLED(I2C)
    - 硬件I2C测试通过
    - 软件I2C测试未通过
- OLED_H：硬件I2C驱动OLED显示屏
- DS18B20：温度传感器DS18B20驱动，在F407@96MHz上测试通过
##### HAL_APP为软件应用
- NR_Printf：将printf用于USART输出
- NR_Delay：包含一个基于定时器的微秒延时函数
#### 安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明

1.  xxxx
2.  xxxx
3.  xxxx


