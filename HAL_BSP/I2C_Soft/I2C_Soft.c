#include "I2C_Soft.h"

// volatile uint8_t I2C_FastMode;

// void delay_us(4)
// {
//     __nop(); __nop(); __nop();
//     __nop(); __nop(); __nop();
//     __nop(); __nop(); __nop();

//     if (!I2C_FastMode)
//     {
//         uint8_t i = 15;
//         while (i--);
//     }
// }

void delay_us(uint32_t i)
{
    uint32_t temp;
    SysTick->LOAD=9*i;              //设置重装数值, 72MHZ时
    SysTick->CTRL=0X01;             //使能，减到零是无动作，采用外部时钟源
    SysTick->VAL=0;                 //清零计数器
    
    do
    {
        temp=SysTick->CTRL;          //读取当前倒计数值
    }
    while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
        
    SysTick->CTRL=0;         //关闭计数器
    SysTick->VAL=0;          //清空计数器
}

void SDA_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SDA_GPIO,&GPIO_InitStruct);
}
 
 
void SDA_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SDA_GPIO,&GPIO_InitStruct);
}

void SCL_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SCL_GPIO,&GPIO_InitStruct);
}

void I2C_Init(void)
{
	
	SCL_Output();
	SDA_Output();
	SCL_H;
	SDA_H;
}

//产生IIC起始信号
void I2C_Soft_Start()
{
    SDA_Output();
    SDA_H;
    SCL_H;
    delay_us(4);
    SDA_L;
    delay_us(4);
    SCL_L;
    // if (!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
    // SDA_L;
    // delay_us(4);
    // if (SDA_read) return 0;	//SDA线为高电平则总线出错,退出
    // SDA_L;
    // delay_us(4);
}

//产生IIC停止信号
void I2C_Soft_Stop()
{
    SDA_Output();
    SCL_L;
    SDA_L;
    delay_us(4);
    SCL_H;
    SDA_H;
    delay_us(4);
}

void I2C_Soft_Ack()
{
    SCL_L;
    delay_us(4);
    SDA_Output();
    SDA_L;
    delay_us(4);
    SCL_H;
    delay_us(4);
    SCL_L;
    delay_us(4);
}

void I2C_Soft_NAck()
{
    SCL_L;
    delay_us(4);
    SDA_Output();
    SDA_H;
    delay_us(4);
    SCL_H;
    delay_us(4);
    SCL_L;
    delay_us(4);
}

int I2C_Soft_WaitAck(void) 	 //返回为:=1无Ack,=0有Ack
{
    uint8_t ErrTime = 0;
    SDA_Input();
    SCL_L;
    delay_us(4);
    SDA_H;
    delay_us(1);
    SCL_H;
    delay_us(1);
    while (SDA_Read)
    {
        ErrTime++;
        if (ErrTime>250)
        {
            I2C_Soft_Stop();
            return 1;
        }
    }
    SCL_L;
    delay_us(4);
    return 0;
}

void I2C_Soft_SendByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i = 8;
    while (i--)
    {
        SCL_L;
        delay_us(4);
        if (SendByte & 0x80)
            SDA_H;
        else
            SDA_L;
        SendByte <<= 1;
        delay_us(4);
        SCL_H;
        delay_us(4);
    }
    SCL_L;
}

//读1个字节，ack=1时，发送ACK，ack=0，发送NACK
uint8_t I2C_Soft_ReadByte(uint8_t Ack)  //数据从高位到低位//
{
    uint8_t i = 8;
    uint8_t ReceiveByte = 0;

    SDA_H;
    while (i--)
    {
        ReceiveByte <<= 1;
        SCL_L;
        delay_us(4);
        SCL_H;
        delay_us(4);
        if (SDA_Read)
        {
            ReceiveByte |= 0x01;
        }
    }
    SCL_L;

    if (Ack)
        I2C_Soft_Ack();
    else
        I2C_Soft_NAck();
    return ReceiveByte;
}


// IIC写一个字节数据
uint8_t IIC_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAck())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAck();
    I2C_Soft_SendByte(REG_data);
    I2C_Soft_WaitAck();
    I2C_Soft_Stop();
    return 0;
}

// IIC读1字节数据
uint8_t IIC_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAck())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAck();
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2C_Soft_WaitAck();
    *REG_data = I2C_Soft_ReadByte(0);
    I2C_Soft_Stop();
    return 0;
}

// IIC写n字节数据
uint8_t IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAck())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAck();
    while (len--)
    {
        I2C_Soft_SendByte(*buf++);
        I2C_Soft_WaitAck();
    }
    I2C_Soft_Stop();
    return 0;
}

// IIC读n字节数据
uint8_t IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAck())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAck();

    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2C_Soft_WaitAck();
    while (len)
    {
        if (len == 1)
        {
            *buf = I2C_Soft_ReadByte(0);
        }
        else
        {
            *buf = I2C_Soft_ReadByte(1);
        }
        buf++;
        len--;
    }
    I2C_Soft_Stop();
    return 0;
}


/**************END OF FILE************/
