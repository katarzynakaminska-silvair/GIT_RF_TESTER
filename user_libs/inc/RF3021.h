#ifndef __RF3021_H_
#define __RF3021_H_

#include "stm32f4xx_conf.h"

#define RF3021_RF_SWITCH_VC GPIO_Pin_8  //GPIOA
#define RF3021_POWER RESET
#define RF3021_FREQUENCY SET

inline void RF3021_init(void);
void RF3021_set_mode(BitAction mode);

#endif
