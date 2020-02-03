#include "LED.h"

void Blink(void)
{
	HAL_GPIO_TogglePin(LED_GPIO,LED_PIN_1);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(LED_GPIO,LED_PIN_1);	
	HAL_Delay(200);
}

void Blinkn(uint16_t LED_PIN,uint16_t n)
{
	for (int i = 0; i < n; i++)
	{
		HAL_GPIO_TogglePin(LED_GPIO,LED_PIN);
		HAL_Delay(200);
		HAL_GPIO_TogglePin(LED_GPIO,LED_PIN);
		HAL_Delay(200);
	}
}

void LED(uint16_t LED_PIN,uint8_t state)
{
	HAL_GPIO_WritePin(LED_GPIO,LED_PIN,state);
}

