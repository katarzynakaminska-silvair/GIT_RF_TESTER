#ifndef __ADC_H_
#define __ADC_H_

#include "stm32f4xx_conf.h"
#include "measurement functions.h"
#include "FLASH.h"

typedef struct
{
	float ADC_ST_Vref;
	float ADC_ST_V25;
	float ADC_ST_V30;
	float ADC_ST_avg_slope;
}ADC_ST_values;
extern ADC_ST_values ADC_ST_values_inst;

//Exported functions
inline void ADC_init(void);
void ADC_measurement(Current_results* Current_results_struct);
void ADC_temperature_measurement(Current_results* Current_results_struct, ADC_ST_values* ADC_ST_values_structure);
void ADC_reference_V_measurement(Current_results* Current_results_struct, ADC_ST_values* ADC_ST_values_structure);
inline void power_count(Current_results* Current_results_struct, Cali_reglin* Cali_reglin_struct , uint8_t is_measurement_fast);
void ADC_set_ST_values(ADC_ST_values* ADC_ST_values_structure);
#endif
