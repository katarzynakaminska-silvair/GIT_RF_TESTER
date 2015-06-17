#ifndef __TIM_H_
#define __TIM_H_

#include "stm32f4xx_conf.h"
#include "measurement functions.h"
#include "SN74LV.h"

void TIM_delay_ms(uint16_t time);
void TIM_warm_up(void);
inline void TIM_warm_up(void);

#endif
