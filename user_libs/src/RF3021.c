#include "RF3021.h"

inline void RF3021_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin =  RF3021_RF_SWITCH_VC ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void RF3021_set_mode(BitAction mode)
{
	GPIO_WriteBit(GPIOA, RF3021_RF_SWITCH_VC, mode);
}
