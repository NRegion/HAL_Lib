#ifndef _LED_H
#define _LED_H

#include "main.h"
#include "gpio.h"

#define LED_GPIO  GPIOF
#define LED_PIN_1 GPIO_PIN_9
#define LED_PIN_2 GPIO_PIN_10
#define LED_PIN_3

#define LHIGH GPIO_PIN_SET;
#define LLOW  GPIO_PIN_RESET;

void Blink(void);
void Blinkn(uint16_t LED_PIN,uint16_t n);
void LED(uint16_t LED_PIN,uint8_t state);

#endif
