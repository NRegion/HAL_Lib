#ifndef _I2C_SOFT_H
#define	_I2C_SOFT_H

#include "main.h"
#include "stdint.h"

/***************I2C GPIO定义******************/
#define NRI2C_SCL_GPIO	    GPIOB
#define NRI2C_SDA_GPIO	    GPIOB
#define NRI2C_SCL_PIN		GPIO_Pin_6
#define NRI2C_SDA_PIN		GPIO_Pin_7
/*********************************************/

// #define SCL_H         ANO_GPIO_I2C->BSRRL = I2C_Pin_SCL
// #define SCL_L         ANO_GPIO_I2C->BSRRH = I2C_Pin_SCL
// #define SDA_H         ANO_GPIO_I2C->BSRRL = I2C_Pin_SDA
// #define SDA_L         ANO_GPIO_I2C->BSRRH = I2C_Pin_SDA
// #define SCL_read      ANO_GPIO_I2C->IDR  & I2C_Pin_SCL
// #define SDA_read      ANO_GPIO_I2C->IDR  & I2C_Pin_SDA

#define SCL_H         HAL_GPIO_WritePin(NRI2C_SCL_GPIO,NRI2C_SCL_PIN,GPIO_PIN_SET)
#define SCL_L         HAL_GPIO_WritePin(NRI2C_SCL_GPIO,NRI2C_SCL_PIN,GPIO_PIN_RESET)
#define SDA_H         HAL_GPIO_WritePin(NRI2C_SDA_GPIO,NRI2C_SDA_PIN,GPIO_PIN_SET)
#define SDA_L         HAL_GPIO_WritePin(NRI2C_SDA_GPIO,NRI2C_SDA_PIN,GPIO_PIN_RESET)
#define SCL_read      HAL_GPIO_ReadPin(NRI2C_SCL_GPIO,NRI2C_SCL_PIN)
#define SDA_read      HAL_GPIO_ReadPin(NRI2C_SDA_GPIO,NRI2C_SDA_PIN)

extern volatile uint8_t I2C_FastMode;

void I2C_Soft_Start(void);
void I2C_Soft_SendByte(uint8_t SendByte);
uint8_t I2C_Soft_ReadByte(uint8_t);
uint8_t IIC_Write_1Byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
uint8_t IIC_Read_1Byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t *REG_data);
uint8_t IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
uint8_t IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);

void I2c_Soft_delay(void);

#endif
