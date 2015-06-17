#include "ADC.h"
#include "FLASH.h"

ADC_ST_values ADC_ST_values_inst;

inline void ADC_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None ;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;

	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //APB2/4 = 21MHz (MAX 36MHz)
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	ADC_CommonInit(&ADC_CommonInitStructure);
	ADC_Init(ADC1,&ADC_InitStructure);
//	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_15Cycles);
	ADC_Cmd(ADC1,ENABLE);

}

void ADC_set_ST_values(ADC_ST_values* ADC_ST_values_structure)
{
	uint32_t ST_value = 0;
	uint16_t ADC_value = 0;
	float ADC_T30;
	float ADC_T110;
	// Get ST ST Vref value
	ST_value = FLASH_read(0x1FFF7A2A);
	ADC_value = ST_value;//(ST_value&0x0000FFFF);
	ADC_ST_values_structure->ADC_ST_Vref = (float)(ADC_value) * 3.3f/4095.0f;
	// Get ST ST T30 value
	ST_value = FLASH_read(0x1FFF7A2C);
	ADC_value = ST_value;//(ST_value&0x0000FFFF);
	ADC_T30 = (float)(ADC_value) * 3.3f/4095.0f;
	// Get ST ST T110 value
	ST_value = FLASH_read(0x1FFF7A2E);
	ADC_value = ST_value;//(ST_value&0x0000FFFF);
	ADC_T110 = (float)(ADC_value) * 3.3f/4095.0f;
	ADC_ST_values_structure->ADC_ST_avg_slope = ((ADC_T110 - ADC_T30)/80.0f);
	ADC_ST_values_structure->ADC_ST_V25 = 0.76;//(ADC_T110 - ADC_T30)*0.3125f + (1.375f*ADC_T30 - 0.375f*ADC_T110);
	ADC_ST_values_structure->ADC_ST_V30 = ADC_T30;
}

void ADC_measurement(Current_results* Current_results_struct)
{
	uint16_t ADC_value = 0;
	uint32_t ADC_mean_value = 0;
	float tempf;
	int i = 0;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_15Cycles);
	  // Start the conversion
	for(i=0; i<100; i++)
	{
		ADC_SoftwareStartConv(ADC1);
	  // Wait until conversion completion
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	  // Get the conversion value
		ADC_value = ADC_GetConversionValue(ADC1);
		ADC_mean_value += ADC_value;
	}
	tempf = (float)ADC_mean_value/100.0f;
	Current_results_struct->current_ADC_voltage = tempf *  Cali_reglin_inst.VCC/4095.0f;
}

uint32_t ADC_peak_measurement(Current_results* Current_results_struct)
{
	uint16_t ADC_value = 0;
	uint32_t ADC_mean_value = 0;
	float tempf;
	int i = 0;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_15Cycles);
	  // Start the conversion
	for(i=0; i<10; i++)
	{
		ADC_SoftwareStartConv(ADC1);
	  // Wait until conversion completion
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	  // Get the conversion value
		ADC_value = ADC_GetConversionValue(ADC1);
		ADC_mean_value += ADC_value;
	}
	return ADC_mean_value;

}
void ADC_temperature_measurement(Current_results* Current_results_struct, ADC_ST_values* ADC_ST_values_structure)
{
	uint16_t ADC_value = 0;
	uint32_t ADC_mean_value = 0;
	float tempf;
	int i = 0;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_18,1, ADC_SampleTime_480Cycles); //temperature
	ADC_TempSensorVrefintCmd(ENABLE);


	for(i=0; i<10; i++)
	{
		// Start the conversion
		ADC_SoftwareStartConv(ADC1);
		  // Wait until conversion completion
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		  // Get the conversion value
		ADC_value = ADC_GetConversionValue(ADC1);
		ADC_mean_value += ADC_value;
	}
	tempf = (float)(ADC_mean_value/10.0f) *  Cali_reglin_inst.VCC/4095.0f;

	Current_results_struct->current_ADC_temperature = ((tempf - ADC_ST_values_structure->ADC_ST_V25)/ADC_ST_values_structure->ADC_ST_avg_slope)+25.0f;
	if(Current_results_struct->current_ADC_temperature < 10 || Current_results_struct->current_ADC_temperature > 90) Current_results_struct->current_error_log_1 |= ERROR_TEMPERATURE;
	ADC_TempSensorVrefintCmd(DISABLE);
}

void ADC_reference_V_measurement(Current_results* Current_results_struct, ADC_ST_values* ADC_ST_values_structure)
{
	uint16_t ADC_value = 0;
	uint32_t ADC_mean_value = 0;
	float tempf;
	int i = 0;

	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17,1,ADC_SampleTime_28Cycles); //Vref

	for(i=0; i<20; i++)
	{
		  // Start the conversion
		ADC_SoftwareStartConv(ADC1);
		  // Wait until conversion completion
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		  // Get the conversion value
		ADC_value = ADC_GetConversionValue(ADC1);
		ADC_mean_value += ADC_value;
	}
	Current_results_struct->current_ADC_ref_voltage = (float)(ADC_mean_value/20.0f) *  Cali_reglin_inst.VCC/4095.0f;
	// check whether measured value is correct
	if(Current_results_struct->current_ADC_ref_voltage < ADC_ST_values_structure->ADC_ST_Vref - 0.01f || Current_results_struct->current_ADC_ref_voltage > ADC_ST_values_structure->ADC_ST_Vref + 0.01f)Current_results_struct->current_error_log_1 |= ERROR_V_REF;
	ADC_TempSensorVrefintCmd(DISABLE);
}

inline void power_count(Current_results* Current_results_struct, Cali_reglin* Cali_reglin_struct, uint8_t is_measurement_fast)
{
	if(is_measurement_fast)
	{
		Current_results_struct->current_power = Cali_reglin_struct->reglin_a * Current_results_struct->current_ADC_voltage + Cali_reglin_struct->reglin_b + (Current_results_struct->current_attenuation_fastcali - Cali_reglin_struct->attenuation);
	}
	else Current_results_struct->current_power = Cali_reglin_struct->reglin_a * Current_results_struct->current_ADC_voltage + Cali_reglin_struct->reglin_b + (Current_results_struct->current_attenuation - Cali_reglin_struct->attenuation);
}
