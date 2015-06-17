#include "LED.h"

inline void LED_init()
{
	GPIO_InitTypeDef LED_GPIO_InitTypeDef;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	LED_GPIO_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	LED_GPIO_InitTypeDef.GPIO_OType = GPIO_OType_PP;
	LED_GPIO_InitTypeDef.GPIO_Pin =  LED_GREEN | LED_BLUE | LED_ORANGE | LED_RED ;
	LED_GPIO_InitTypeDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	LED_GPIO_InitTypeDef.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &LED_GPIO_InitTypeDef);
}

inline void LED_on(uint16_t LED)
{
	 GPIO_SetBits(GPIOD,LED);
}

inline void LED_off(uint16_t LED)
{
	 GPIO_ResetBits(GPIOD,LED);
}
inline void LED_toggle(uint16_t LED)
{
	GPIO_ToggleBits(GPIOD,LED);
}
