#include "IIC_Soft.h"

volatile uint8_t I2C_FastMode;

void I2C_Soft_delay()
{
    __nop(); __nop(); __nop();
    __nop(); __nop(); __nop();
    __nop(); __nop(); __nop();

    if (!I2C_FastMode)
    {
        uint8_t i = 15;
        while (i--);
    }
}

void SDA_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SDA_GPIO,&GPIO_InitStruct);
}
 
 
void SDA_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SDA_GPIO,&GPIO_InitStruct);
}
 
void SCL_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NRI2C_SCL_PORT,&GPIO_InitStruct);
}

void SCL_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = NRI2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
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
int I2C_Soft_Start()
{
    SDA_Output();
    SDA_H;
    SCL_H;
    I2C_Soft_delay();
    SDA_L;
    I2C_Soft_delay();
    SCL_L;
    // if (!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
    // SDA_L;
    // I2C_Soft_delay();
    // if (SDA_read) return 0;	//SDA线为高电平则总线出错,退出
    // SDA_L;
    // I2C_Soft_delay();
    return 1;
}

//产生IIC停止信号
void I2C_Soft_Stop()
{
    SDA_Output();
    SCL_L;
    I2C_Soft_delay();
    SDA_L;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();
    SDA_H;
    I2C_Soft_delay();
}

void I2C_Soft_Ask()
{
    SCL_L;
    I2C_Soft_delay();
    SDA_L;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();
    SCL_L;
    I2C_Soft_delay();
}

void I2C_Soft_NoAsk()
{
    SCL_L;
    I2C_Soft_delay();
    SDA_H;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();
    SCL_L;
    I2C_Soft_delay();
}

int I2C_Soft_WaitAsk(void) 	 //返回为:=1无ASK,=0有ASK
{
    uint8_t ErrTime = 0;
    SCL_L;
    I2C_Soft_delay();
    SDA_H;
    I2C_Soft_delay();
    SCL_H;
    I2C_Soft_delay();
    while (SDA_read)
    {
        ErrTime++;
        if (ErrTime>50)
        {
            I2C_Soft_Stop();
            return 1;
        }
    }
    SCL_L;
    I2C_Soft_delay();
    return 0;
}

void I2C_Soft_SendByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i = 8;
    while (i--)
    {
        SCL_L;
        I2C_Soft_delay();
        if (SendByte & 0x80)
            SDA_H;
        else
            SDA_L;
        SendByte <<= 1;
        I2C_Soft_delay();
        SCL_H;
        I2C_Soft_delay();
    }
    SCL_L;
}

//读1个字节，ack=1时，发送ACK，ack=0，发送NACK
uint8_t I2C_Soft_ReadByte(uint8_t ask)  //数据从高位到低位//
{
    uint8_t i = 8;
    uint8_t ReceiveByte = 0;

    SDA_H;
    while (i--)
    {
        ReceiveByte <<= 1;
        SCL_L;
        I2C_Soft_delay();
        SCL_H;
        I2C_Soft_delay();
        if (SDA_read)
        {
            ReceiveByte |= 0x01;
        }
    }
    SCL_L;

    if (ask)
        I2C_Soft_Ask();
    else
        I2C_Soft_NoAsk();
    return ReceiveByte;
}


// IIC写一个字节数据
uint8_t IIC_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAsk())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAsk();
    I2C_Soft_SendByte(REG_data);
    I2C_Soft_WaitAsk();
    I2C_Soft_Stop();
    return 0;
}

// IIC读1字节数据
uint8_t IIC_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *REG_data)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAsk())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAsk();
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2C_Soft_WaitAsk();
    *REG_data = I2C_Soft_ReadByte(0);
    I2C_Soft_Stop();
    return 0;
}

// IIC写n字节数据
uint8_t IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAsk())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAsk();
    while (len--)
    {
        I2C_Soft_SendByte(*buf++);
        I2C_Soft_WaitAsk();
    }
    I2C_Soft_Stop();
    return 0;
}

// IIC读n字节数据
uint8_t IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{
    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1);
    if (I2C_Soft_WaitAsk())
    {
        I2C_Soft_Stop();
        return 1;
    }
    I2C_Soft_SendByte(REG_Address);
    I2C_Soft_WaitAsk();

    I2C_Soft_Start();
    I2C_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2C_Soft_WaitAsk();
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
