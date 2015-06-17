#include "SN74LV.h"

inline void SN74LV_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin =  SN74LV_COUNTER_CRL;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SN74LV_set_mode(BitAction mode)
{
	GPIO_WriteBit(GPIOA, SN74LV_COUNTER_CRL, mode);
}
