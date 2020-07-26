#include "NR_Delay.h"

/* 进行5000微秒以内的延时*/
void delay_us(uint16_t us)
{
	/* 中断会打断微秒延时函数，
	 * 但计数器计数不会停
	 * 防止计数器增加到65535之后重新开始计数
	 */
   uint16_t differ=50000-us;
	
   HAL_TIM_Base_Start(&delay_tim);
   __HAL_TIM_SET_COUNTER(&delay_tim,differ);

   while(differ<50000)
   {
       differ=__HAL_TIM_GET_COUNTER(&delay_tim);
   }

   HAL_TIM_Base_Stop(&delay_tim);
}

/************************END OF FILE************************/
