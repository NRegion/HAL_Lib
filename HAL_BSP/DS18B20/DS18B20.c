#include "DS18B20.h"

#define Delay_ms(x)   HAL_Delay(x)

static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out_PP(void);
static void DS18B20_Rst(void);
static uint8_t DS18B20_Presence(void);
static uint8_t DS18B20_ReadBit(void);
static uint8_t DS18B20_ReadByte(void);
static void DS18B20_WriteByte(uint8_t dat);
static void DS18B20_SkipRom(void);
static void DS18B20_MatchRom(void);

/* 函数体 --------------------------------------------------------------------*/
/**
 * 函数功能:
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
/* 进行5000微秒以内的延时*/
//void delay_us(uint16_t us)
//{
//	/* 中断会打断微秒延时函数，
//	 * 但计数器计数不会停
//	 * 防止计数器增加到65535之后重新开始计数
//	 */
//   uint16_t differ=50000-us;
//	
//   HAL_TIM_Base_Start(&htim2);
//   __HAL_TIM_SET_COUNTER(&htim2,differ);

//   while(differ<50000)
//   {
//       differ=__HAL_TIM_GET_COUNTER(&htim2);
//   }

//   HAL_TIM_Base_Stop(&htim2);
//}

static void DS18B20_Delay(uint16_t time)
{
	delay_us(time);
}


//static void DS18B20_Delay(uint16_t time)
//{
//  int8_t i;
//	while(time)
//	{		
//		for (i = 0; i < 10; i++)
//		{
//		}
//	time--;
//	}
//}

/**
 * 函数功能: DS18B20 初始化函数
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
uint8_t DS18B20_Init(void)
{
DS18B20_Dout_GPIO_CLK_ENABLE();

DS18B20_Mode_Out_PP();
        
        DS18B20_Dout_HIGH();
        
        DS18B20_Rst();

return DS18B20_Presence ();
}


/**
 * 函数功能: 使DS18B20-DATA引脚变为上拉输入模式
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_Mode_IPU(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

/* 串口外设功能GPIO配置 */
GPIO_InitStruct.Pin   = DS18B20_Dout_PIN;
GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull  = GPIO_PULLUP;
HAL_GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);
        
}

/**
 * 函数功能: 使DS18B20-DATA引脚变为推挽输出模式
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_Mode_Out_PP(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

/* 串口外设功能GPIO配置 */
GPIO_InitStruct.Pin = DS18B20_Dout_PIN;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);         
}

/**
 * 函数功能: 主机给从机发送复位脉冲
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_Rst(void)
{
        /* 主机设置为推挽输出 */
        DS18B20_Mode_Out_PP();
        
        DS18B20_Dout_LOW();

        /* 主机至少产生480us的低电平复位信号 */
        DS18B20_Delay(750);
        
        /* 主机在产生复位信号后，需将总线拉高 */
        DS18B20_Dout_HIGH();
        
        /*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
        DS18B20_Delay(15);
}

/**
 * 函数功能: 检测从机给主机返回的存在脉冲
 * 输入参数: 无
 * 返 回 值: 0：成功，1：失败
 * 说    明：无
 */
static uint8_t DS18B20_Presence(void)
{
        uint8_t pulse_time = 0;
        
        /* 主机设置为上拉输入 */
        DS18B20_Mode_IPU();
        
        /* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号
        * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
        */
        while( DS18B20_Data_IN() && pulse_time<100 )
        {
                pulse_time++;
                DS18B20_Delay(1);
        }        
        /* 经过100us后，存在脉冲都还没有到来*/
        if( pulse_time >=100 )
                return 1;
        else
                pulse_time = 0;
        
        /* 存在脉冲到来，且存在的时间不能超过240us */
        while( !DS18B20_Data_IN() && pulse_time<240 )
        {
                pulse_time++;
                DS18B20_Delay(1);
        }        
        if( pulse_time >=240 )
                return 1;
        else
                return 0;
}

/**
 * 函数功能: 从DS18B20读取一个bit
 * 输入参数: 无
 * 返 回 值: 读取到的数据
 * 说    明：无
 */
static uint8_t DS18B20_ReadBit(void)
{
        uint8_t dat;
        
        /* 读0和读1的时间至少要大于60us */        
        DS18B20_Mode_Out_PP();
        /* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
        DS18B20_Dout_LOW();
        DS18B20_Delay(10);
        
        /* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
        DS18B20_Mode_IPU();
        //Delay_us(2);
        
        if( DS18B20_Data_IN() == SET )
                dat = 1;
        else
                dat = 0;
        
        /* 这个延时参数请参考时序图 */
        DS18B20_Delay(45);
        
        return dat;
}

/**
 * 函数功能: 从DS18B20读一个字节，低位先行
 * 输入参数: 无
 * 返 回 值: 读到的数据
 * 说    明：无
 */
static uint8_t DS18B20_ReadByte(void)
{
        uint8_t i, j, dat = 0;        
        
        for(i=0; i<8; i++)
        {
                j = DS18B20_ReadBit();               
                dat = (dat) | (j<<i);
        }
        
        return dat;
}

/**
 * 函数功能: 写一个字节到DS18B20，低位先行
 * 输入参数: dat：待写入数据
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_WriteByte(uint8_t dat)
{
        uint8_t i, testb;
        DS18B20_Mode_Out_PP();
        
        for( i=0; i<8; i++ )
        {
                testb = dat&0x01;
                dat = dat>>1;               
                /* 写0和写1的时间至少要大于60us */
                if (testb)
                {                        
                        DS18B20_Dout_LOW();
                        /* 1us < 这个延时 < 15us */
                        DS18B20_Delay(8);
                        
                        DS18B20_Dout_HIGH();
                        DS18B20_Delay(58);
                }               
                else
                {                        
                        DS18B20_Dout_LOW();
                        /* 60us < Tx 0 < 120us */
                        DS18B20_Delay(70);
                        
                        DS18B20_Dout_HIGH();               
                        /* 1us < Trec(恢复时间) < 无穷大*/
                        DS18B20_Delay(2);
                }
        }
}

/**
 * 函数功能: 跳过匹配 DS18B20 ROM
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_SkipRom ( void )
{
        DS18B20_Rst();                  
        DS18B20_Presence();                 
        DS18B20_WriteByte(0XCC);                /* 跳过 ROM */        
}

/**
 * 函数功能: 执行匹配 DS18B20 ROM
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
static void DS18B20_MatchRom ( void )
{
        DS18B20_Rst();                  
        DS18B20_Presence();                 
        DS18B20_WriteByte(0X55);                /* 匹配 ROM */        
}


/*
* 存储的温度是16 位的带符号扩展的二进制补码形式
* 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
*
*         |---------整数----------|-----小数 分辨率 1/(2^4)=0.0625----|
* 低字节  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
*
*
*         |-----符号位：0->正  1->负-------|-----------整数-----------|
* 高字节  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
*
*
* 温度 = 符号位 + 整数 + 小数*0.0625
*/
/**
 * 函数功能: 在跳过匹配 ROM 情况下获取 DS18B20 温度值
 * 输入参数: 无
 * 返 回 值: 温度值
 * 说    明：无
 */
float DS18B20_GetTemp_SkipRom ( void )
{
        uint8_t tpmsb, tplsb;
        short s_tem;
        float f_tem;
        
        
        DS18B20_SkipRom ();
        DS18B20_WriteByte(0X44);                                /* 开始转换 */
        
        
        DS18B20_SkipRom ();
				DS18B20_WriteByte(0XBE);                                /* 读温度值 */
        
        tplsb = DS18B20_ReadByte();                 
        tpmsb = DS18B20_ReadByte();
        
        
        s_tem = tpmsb<<8;
        s_tem = s_tem | tplsb;
        
        if( s_tem < 0 )                /* 负温度 */
                f_tem = (~s_tem+1) * 0.0625;        
        else
                f_tem = s_tem * 0.0625;
        
        return f_tem;         
}

/**
 * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值
 * 输入参数: ds18b20_id：用于存放 DS18B20 序列号的数组的首地址
 * 返 回 值: 无
 * 说    明：无
 */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
        uint8_t uc;
                
        DS18B20_WriteByte(0x33);       //读取序列号
        
        for ( uc = 0; uc < 8; uc ++ )
        ds18b20_id [ uc ] = DS18B20_ReadByte();        
}

/**
 * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值
 * 输入参数: ds18b20_id：存放 DS18B20 序列号的数组的首地址
 * 返 回 值: 温度值
 * 说    明：无
 */
float DS18B20_GetTemp_MatchRom ( uint8_t * ds18b20_id )
{
        uint8_t tpmsb, tplsb, i;
        short s_tem;
        float f_tem;
        
        
        DS18B20_MatchRom ();            //匹配ROM
        
for(i=0;i<8;i++)
                DS18B20_WriteByte ( ds18b20_id [ i ] );        
        
        DS18B20_WriteByte(0X44);                                /* 开始转换 */

        
        DS18B20_MatchRom ();            //匹配ROM
        
        for(i=0;i<8;i++)
                DS18B20_WriteByte ( ds18b20_id [ i ] );        
        
        DS18B20_WriteByte(0XBE);                                /* 读温度值 */
        
        tplsb = DS18B20_ReadByte();                 
        tpmsb = DS18B20_ReadByte();
        
        
        s_tem = tpmsb<<8;
        s_tem = s_tem | tplsb;
        
        if( s_tem < 0 )                /* 负温度 */
                f_tem = (~s_tem+1) * 0.0625;        
        else
                f_tem = s_tem * 0.0625;
        
        return f_tem;                 
}

/************************END OF FILE************************/
