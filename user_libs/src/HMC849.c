#include "HMC849.h"

inline void HMC849_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin =  HMC849_RF_SWITCH_VC ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  HMC849_RF_SWITCH_EN ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, HMC849_RF_SWITCH_EN);
}
void HMC849_set_mode(BitAction mode)
{
	GPIO_ResetBits(GPIOC, HMC849_RF_SWITCH_EN);
	GPIO_WriteBit(GPIOA, HMC849_RF_SWITCH_VC, mode);
//	GPIO_WriteBit(GPIOC, GPIO_Pin_9, RESET);
}

inline void HMC849_disable(void)
{
	GPIO_SetBits(GPIOC, HMC849_RF_SWITCH_EN);
}
