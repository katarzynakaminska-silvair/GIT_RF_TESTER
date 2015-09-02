#ifndef __HMC849_H_
#define __HMC849_H_


#include "stm32f4xx_conf.h"

#define HMC849_RF_SWITCH_VC GPIO_Pin_8  //GPIOA
#define HMC849_RF_SWITCH_EN GPIO_Pin_9  //GPIOC
#define HMC849_POWER RESET
#define HMC849_FREQUENCY SET

inline void HMC849_init(void);
void HMC849_set_mode(BitAction mode);
inline void HMC849_disable(void);


#endif
