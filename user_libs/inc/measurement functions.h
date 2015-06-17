#ifndef __MEASUREMENT_FUNCTIONS_H_
#define __MEASUREMENT_FUNCTIONS_H_

#include "stm32f4xx_conf.h"
//#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
//#define BYTETOBINARY(byte)  \
//  (byte & 0x80 ? 1 : 0), \
//  (byte & 0x40 ? 1 : 0), \
//  (byte & 0x20 ? 1 : 0), \
//  (byte & 0x10 ? 1 : 0), \
//  (byte & 0x08 ? 1 : 0), \
//  (byte & 0x04 ? 1 : 0), \
//  (byte & 0x02 ? 1 : 0), \
//  (byte & 0x01 ? 1 : 0)

#define ERROR_V_REF 		0b00000001
#define ERROR_TEMPERATURE 	0b00000010
#define ERROR_CALIBRATION 	0b00000100
#define ERROR_WARM_UP	 	0b00001000
#define ERROR_SERIAL_NUM	0b00010000

typedef struct
{
	float current_ADC_voltage;
	float current_power;
	float current_attenuation;
	float current_attenuation_fastcali;
	float current_ADC_temperature;
	float current_detector_temperature;
	float current_ADC_ref_voltage;
	double current_frequency;
	uint8_t current_error_log_1;
	uint8_t current_error_log_2;
	uint8_t current_is_warmed_up;
	uint32_t current_peak_value;
} Current_results;

extern Current_results Current_results_inst;

void power_measurement(Current_results* Current_results_struct, uint8_t is_measurement_fast);
void frequency_measurement();
void set_attenuation(Current_results* Current_results_struct, float max_power);
inline void set_warm_up_state(uint8_t is_warmed_up);

#endif
