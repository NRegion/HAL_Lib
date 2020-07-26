#ifndef _OLED_H_
#define _OLED_H_

#include "main.h"
#include "i2c.h"

// the I2C address of oled
#define OLED_I2C_ADDRESS    0x78

//-----------------OLED接口定义----------------
#define  OLED_SCL_GPIO GPIOB
#define  OLED_SDA_GPIO GPIOB

#define  OLED_SCL_PIN  GPIO_PIN_6
#define  OLED_SDA_PIN  GPIO_PIN_7

#define OLED_I2C       hi2c1

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define uint8_t unsigned char
#define u32 unsigned int

void OLED_ClearPoint(uint8_t x,uint8_t y);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1);
void OLED_ShowNum(uint8_t x,uint8_t y,u32 num,uint8_t len,uint8_t size1);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1);
void OLED_ScrollDisplay(uint8_t num,uint8_t space);
void OLED_WR_BP(uint8_t x,uint8_t y);
void OLED_ShowPicture(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);
void OLED_Init(void);
#endif
