#include "measurement functions.h"
#include "LED.h"
#include "FLASH.h"
#include "SN74LV.h"
#include "HMC849.h"
#include "HMC624.h"
#include "LED.h"
#include "TIM.h"
#include "ADC.h"

Current_results Current_results_inst;
extern Cali_reglin Cali_reglin_inst;

void Current_result_clear(Current_results* Current_results_struct)
{
	LED_off(LED_RED);
	Current_results_struct->current_error_log_1 = 0;
	Current_results_struct->current_error_log_2 = 0;
	Current_results_struct->current_ADC_ref_voltage = 0;
	Current_results_struct->current_ADC_temperature = 0;
	Current_results_struct->current_ADC_voltage = 0;
	Current_results_struct->current_power = 0;
	Current_results_struct->current_frequency = 0;
}
void power_measurement(Current_results* Current_results_struct, uint8_t is_measurement_fast)
{

	Current_result_clear(Current_results_struct);
	HMC849_set_mode(HMC849_POWER);
	if(Cali_reglin_inst.is_clibrated !=1)Current_results_struct->current_error_log_1 |= ERROR_CALIBRATION;
	if(!Current_results_struct->current_is_warmed_up)Current_results_struct->current_error_log_1 |= ERROR_WARM_UP;
	if(!Cali_reglin_inst.is_serialnum_set)Current_results_struct->current_error_log_1 |= ERROR_SERIAL_NUM;
	TIM_delay_ms(1);
	ADC_temperature_measurement(Current_results_struct, &ADC_ST_values_inst);
	ADC_reference_V_measurement(Current_results_struct, &ADC_ST_values_inst);
	ADC_measurement(Current_results_struct);
	power_count(Current_results_struct, &Cali_reglin_inst, is_measurement_fast);
	HMC849_disable();
	if(Current_results_struct->current_error_log_1)LED_on(LED_RED);

}
void frequency_measurement(Current_results* Current_results_struct)
{

	uint32_t cnt5 = 0; // TIM5 counter value
	double tim5_f = 84000000; //TIM5 clock frequency
	uint32_t tim2_cycle_num = 1200000; //number of measurement cycles
	uint32_t temp_cycle_num = 0;
	uint32_t 	cnt5_temp = 0;

	Current_result_clear(&Current_results_inst);
	HMC849_set_mode(HMC849_FREQUENCY);
	SN74LV_set_mode(SN74LV_REACT_ON_CLOCK);

	if(Cali_reglin_inst.is_clibrated !=1)Current_results_struct->current_error_log_1 |= ERROR_CALIBRATION;
	if(!Current_results_struct->current_is_warmed_up)Current_results_struct->current_error_log_1 |= ERROR_WARM_UP;
	if(!Cali_reglin_inst.is_serialnum_set)Current_results_struct->current_error_log_1 |= ERROR_SERIAL_NUM;
	ADC_temperature_measurement(Current_results_struct, &ADC_ST_values_inst);
	ADC_reference_V_measurement(Current_results_struct, &ADC_ST_values_inst);

	 __disable_irq();
	TIM2_init();

	while(TIM2->CNT < tim2_cycle_num)
		{
			temp_cycle_num = TIM2->CNT;
			cnt5_temp = TIM5->CNT;
		}

	cnt5 = TIM5->CNT;
	 __enable_irq();
	Current_results_struct->current_frequency = (((double)tim5_f * (double)tim2_cycle_num*1024.0)/((double)cnt5 * 1000000.0));

//	TIM5->CNT = 0;
//	TIM2->CNT = 0;

	TIM2->CR1 &= ~0x0001; //TIM2 disable counting
	TIM5->CR1 &= ~0x0001; //TIM5 disable counting
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM5);

	HMC849_disable();
	if(Current_results_struct->current_error_log_1)LED_on(LED_RED);

}
void set_attenuation_auto()
{
	float temp_power = -50.0;
	float temp_attenuation = 0.0;
	float ADC_voltage = 0.0;


	uint16_t ADC_value = 0;


	HMC849_set_mode(HMC849_POWER);
	HMC624_set_attenuation(31.5f);
	temp_attenuation = 31.5f;


	while(temp_power < -40.0f)
	{

		if(temp_attenuation != 31.5)
		{
			temp_attenuation -=  0.5f;
			HMC624_set_attenuation(temp_attenuation);
		}

		if(temp_attenuation == 0) break;

		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_144Cycles);
	  // Start the conversion
		ADC_SoftwareStartConv(ADC1);
	  // Wait until conversion completion
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	  // Get the conversion value
		ADC_value = ADC_GetConversionValue(ADC1);

		ADC_voltage = ADC_value *  Cali_reglin_inst.VCC/4095.0f;

		temp_power =  Cali_reglin_inst.reglin_a * ADC_voltage + Cali_reglin_inst.reglin_b + ( temp_attenuation - Cali_reglin_inst.attenuation );


	}

//	HMC849_disable();
	Current_results_inst.current_attenuation_fastcali = temp_attenuation;
	//jeli za du¿a moc to zostawia 31,5

// wybór t³ummienia patrzy jakie ma napiecie na ADC, jak za du¿e lub za ma³e to zmienia wartoæ t³umienia
}

void set_attenuation(Current_results* Current_results_struct, float max_power)
{
	float atten; //= 1;

	if(max_power <= -3.25f)
	{
		atten = 0.0f;
	}
	else if (max_power >=27.75f)
	{
		atten = 31.0f;
	}
	else
	{
		atten = roundf(max_power + 3.25f);//3.25 - 6; 3.25- t³umienie toru, -6 oczekiwana moc max na wejciu detektora
	}


	HMC624_set_attenuation(atten);
	Current_results_struct->current_attenuation = atten;
}


inline void set_warm_up_state(uint8_t is_warmed_up)
{
	if(is_warmed_up == 0) LED_on(LED_ORANGE);
	else
	{
		LED_off(LED_ORANGE);
		Current_results_inst.current_is_warmed_up = is_warmed_up;
	}
}

void clear_buf(char* buf, int bytes_to_clear)
{
	int k = 0;
	for ( k = 0; k <= bytes_to_clear; k++)
	{
		buf[k] = 0;
	}
}

