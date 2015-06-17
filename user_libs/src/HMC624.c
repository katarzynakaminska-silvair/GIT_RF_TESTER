#include "HMC624.h"

inline void HMC624_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin =  HMC624_ATTEN_LE | HMC624_ATTEN_D0 | HMC624_ATTEN_D1 | HMC624_ATTEN_D2 |HMC624_ATTEN_D3 | HMC624_ATTEN_D4 | HMC624_ATTEN_D5;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &GPIO_InitStructure);


}

//TODO: optimize this function
inline void HMC624_set_attenuation(float attenuation)
{
	uint8_t config_record =  ((int)roundf((attenuation*2.0f)));
	GPIO_ResetBits(GPIOD,HMC624_ATTEN_LE);

	GPIO_SetBits(GPIOD,HMC624_ATTEN_D0 | HMC624_ATTEN_D1 | HMC624_ATTEN_D2 |HMC624_ATTEN_D3 | HMC624_ATTEN_D4| HMC624_ATTEN_D5);
	if((config_record & 0b00100000)>>5) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D5);
	if((config_record & 0b00010000)>>4) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D4);
	if((config_record & 0b00001000)>>3) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D3);
	if((config_record & 0b00000100)>>2) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D2);
	if((config_record & 0b00000010)>>1) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D1);
	if((config_record & 0b00000001)   ) GPIO_ResetBits(GPIOD, HMC624_ATTEN_D0);

//	uint8_t tab_att[7] = {0};
//	tab_att[0] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D0);
//	tab_att[1] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D1);
//	tab_att[2] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D2);
//	tab_att[3] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D3);
//	tab_att[4] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D4);
//	tab_att[5] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_D5);
//	tab_att[6] = GPIO_ReadOutputDataBit(GPIOD, HMC624_ATTEN_LE);

//	uint16_t temp_GPIO_state;
//	temp_GPIO_state = GPIO_ReadInputData( GPIOD);

	GPIO_SetBits(GPIOD,HMC624_ATTEN_LE);

}
