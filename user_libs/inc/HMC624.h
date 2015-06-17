#ifndef __HMC624_H_
#define __HMC624_H_


#include "stm32f4xx_conf.h"
#include "measurement functions.h"


//GPIOD
#define HMC624_ATTEN_LE GPIO_Pin_9
#define HMC624_ATTEN_D0 GPIO_Pin_10
#define HMC624_ATTEN_D1 GPIO_Pin_11
#define HMC624_ATTEN_D2 GPIO_Pin_12
#define HMC624_ATTEN_D3 GPIO_Pin_13
#define HMC624_ATTEN_D4 GPIO_Pin_14
#define HMC624_ATTEN_D5 GPIO_Pin_15

//Exported functions
inline void HMC624_init(void);
inline void HMC624_set_attenuation(float attenuation);

#endif
