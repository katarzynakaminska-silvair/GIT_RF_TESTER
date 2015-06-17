#ifndef __SN74LV_H_
#define __SN74LV_H_

#include "stm32f4xx_conf.h"

#define SN74LV_COUNTER_CRL  GPIO_Pin_10 //GPIOA
#define SN74LV_REACT_ON_CLOCK RESET
#define SN74LV_NOT_REACT_ON_CLOCK SET

#define SN74LV_COUNTER_OUTPUT_1  GPIO_Pin_1 //GPIOA TIM2_CH2, TIM5_CH2
#define SN74LV_COUNTER_OUTPUT_2  GPIO_Pin_5 //GPIOA TIM2_ETR

inline void SN74LV_init(void);
void SN74LV_set_mode(BitAction mode);

#endif
