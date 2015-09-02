
/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "core_cm4.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "usbd_cdc_vcp.h"
#include "HMC624.h"
#include "ADC.h"
#include "LED.h"
#include "FLASH.h"
#include "SN74LV.h"
#include "HMC849.h"
#include "TIM.h"

//static uint8_t firmware_version[3] = {1,1,1}; //major, minor, release

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
extern uint8_t USB_Rx_Buffer   [CDC_DATA_MAX_PACKET_SIZE];
extern uint32_t APP_Rx_length ;
extern uint16_t USB_Rx_Cnt;
//char buf[100] = {0};
//extern uint8_t is_warmed_up;

//uint32_t serialnum;
//char chartab[20] = {0};
//int i, k = 0, m = 0;
//float tempf = 0;
//float* tempf_ptr;
//uint8_t temp_float_tab[4] = {0};

//enum command_codes { 	POWER_MEASUREMENT = 0x01,
//						POWER_MEASUREMENT_KALI = 0x02,
//						FREQUENCY_MEASUREMENT = 0x03,
//						ATTENUATION_SET = 0x04,
//						REGLINP_SET = 0x05,
//						SERIALNUM_SET = 0x06,
//						SERIALNUM_GET = 0x07,
//						FIRMWARE_VER_GET = 0x08
//					};

//void float2Bytes(float val,uint8_t* bytes_array)
//{
//  // Create union of shared memory space
//  union {
//    float float_variable;
//    uint8_t temp_array[4];
//  } u;
//  // Overite bytes of union with float variable
//  u.float_variable = val;
//  // Assign bytes to input array
//  memcpy(bytes_array, u.temp_array, 4);
//}
//
//void double2Bytes(double val,uint8_t* bytes_array)
//{
//  // Create union of shared memory space
//  union {
//    double double_variable;
//    uint8_t temp_array[8];
//  } u;
//  // Overite bytes of union with float variable
//  u.double_variable = val;
//  // Assign bytes to input array
//  memcpy(bytes_array, u.temp_array, 8);
//}

int main(void)
{

	SystemInit();
	LED_init();
	LED_on(LED_GREEN);

	HMC624_init();
	ADC_init();
	ADC_set_ST_values(&ADC_ST_values_inst);
	SN74LV_init();
	RF3021_init();
	//set_attenuation(&Current_results_inst, 15);
	HMC624_set_attenuation(10);
	TIM_warm_up();
	HMC849_init();
//	float *float_pointer;
//	double *double_pointer;


	//user switch
///////////////////////////////////////////////
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
//
//	EXTI_InitTypeDef   ExtiInitStructure;
//	ExtiInitStructure.EXTI_Line = EXTI_Line0;
//	ExtiInitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	ExtiInitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	ExtiInitStructure.EXTI_LineCmd = ENABLE;
//
//	EXTI_Init(&ExtiInitStructure);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
//
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
/////////////////////////////////////////////////////////



  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
            &USR_cb);

/////////////////////////////////////////////////////////

//  Cali_reglin_inst.reglin_a = -40.1143759204;
//  Cali_reglin_inst.reglin_b = 24.995142;
//  Cali_reglin_inst.attenuation = 10;
//  Cali_reglin_inst.is_clibrated = 1;
//  Cali_reglin_inst.VCC = 3.2812;
//
//  FLASH_write_reglin(&Cali_reglin_inst);

  ///////////////////////////////////////////////////////////

  FLASH_read_reglin(&Cali_reglin_inst);
  Cali_reglin_inst.is_serialnum_set = 1;
  if(Cali_reglin_inst.is_clibrated !=1)
  {
		Cali_reglin_inst.reglin_a = -40.1143759204;
		Cali_reglin_inst.reglin_b = 24.995142;
		Cali_reglin_inst.attenuation = 10;
		Cali_reglin_inst.VCC = 3.2812;
  }
  HMC624_set_attenuation(10.0); // to be deleted
  Current_results_inst.current_attenuation = 10;//change to 31,5



  while (1)
  {
	  __WFI();
  }
}
//	  if(USB_Rx_Cnt !=0)
//		  switch (USB_Rx_Buffer[0])
//		  {
//			case POWER_MEASUREMENT:
//				HMC624_set_attenuation(Current_results_inst.current_attenuation);
//				power_measurement(&Current_results_inst, 0);
//				clear_buf(buf,26);
//				buf[0] = POWER_MEASUREMENT;
//				float_pointer = buf+1;
//				float_pointer[0] = Current_results_inst.current_power;
//				float_pointer[1] = Current_results_inst.current_ADC_temperature;
//				float_pointer[2] = Current_results_inst.current_ADC_ref_voltage;
//				float_pointer[3] = Current_results_inst.current_detector_temperature;
//
////				buf+1 = Current_results_inst.current_power;
////				float2Bytes(Current_results_inst.current_power, buf+1);
////				float2Bytes(Current_results_inst.current_ADC_temperature, buf+5);
////				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+9);
////				float2Bytes(Current_results_inst.current_detector_temperature, buf+13);
//
//
//				buf[17] = Current_results_inst.current_error_log_1;
//				buf[18] = Current_results_inst.current_error_log_2;
//
//				VCP_DataTx(buf,19);
//				USB_Rx_Cnt = 0;
//			break;
//
//			case POWER_MEASUREMENT_KALI:
//
//				set_attenuation_auto();
//				power_measurement(&Current_results_inst, 1);
//				clear_buf(buf,26);
//				buf[0] = POWER_MEASUREMENT_KALI;
//
//				float_pointer = buf+1;
//				*(float_pointer + 0) = Current_results_inst.current_power;
//				*(float_pointer + 1) = Current_results_inst.current_ADC_temperature;
//				*(float_pointer + 2) = Current_results_inst.current_ADC_ref_voltage;
//				*(float_pointer + 3) = Current_results_inst.current_detector_temperature;
//
////				float2Bytes(Current_results_inst.current_power, buf+1);
////				float2Bytes(Current_results_inst.current_ADC_temperature, buf+5);
////				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+9);
////				float2Bytes(Current_results_inst.current_detector_temperature, buf+13);
//
//				buf[17] = Current_results_inst.current_error_log_1;
//				buf[18] = Current_results_inst.current_error_log_2;
//
//				VCP_DataTx(buf,19);
//				USB_Rx_Cnt = 0;
//			break;
//
//			case FREQUENCY_MEASUREMENT:
//				frequency_measurement(&Current_results_inst);
//				clear_buf(buf,26);
//				buf[0] = FREQUENCY_MEASUREMENT;
//				double_pointer = buf + 1;
//				float_pointer = buf + 9;
//
//				double2Bytes(Current_results_inst.current_frequency, buf+1);
//				//*double_pointer = Current_results_inst.current_frequency;
//				*(float_pointer + 0) = Current_results_inst.current_ADC_temperature;
//				*(float_pointer + 1) = Current_results_inst.current_ADC_ref_voltage;
//				*(float_pointer + 2) = Current_results_inst.current_detector_temperature;
////				double2Bytes(Current_results_inst.current_frequency, buf+1);
////				float2Bytes(Current_results_inst.current_ADC_temperature, buf+9);
////				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+13);
////				float2Bytes(Current_results_inst.current_detector_temperature, buf+17);
//
//				buf[21] = Current_results_inst.current_error_log_1;
//				buf[22] = Current_results_inst.current_error_log_2;
//
//				VCP_DataTx(buf,23);
//
//				USB_Rx_Cnt = 0;
//			break;
//
//			case ATTENUATION_SET:
//			for (i=1; i<USB_Rx_Cnt; i++)
//			{
//				chartab[i-1] = USB_Rx_Buffer[i];
//			}
////			sscanf(chartab, "%f", &tempf);
////			tempf = (float)atof(chartab);
//
//			tempf_ptr = chartab;
//			set_attenuation(&Current_results_inst, *tempf_ptr);
//			clear_buf(buf,26);
//			buf[0] = ATTENUATION_SET;
//			buf[1] = 0x01;
//			VCP_DataTx(buf,2);
//
// 			USB_Rx_Cnt = 0;
//			break;
//
//			case REGLINP_SET:
//
//				LED_on(LED_BLUE);
//
//				tempf_ptr = USB_Rx_Buffer + 1;
//				Cali_reglin_inst.reglin_a = *tempf_ptr;
//				tempf_ptr = USB_Rx_Buffer + 5;
//				Cali_reglin_inst.reglin_b = *tempf_ptr;
//				tempf_ptr = USB_Rx_Buffer + 9;
//				Cali_reglin_inst.VCC = *tempf_ptr;
//				tempf_ptr = USB_Rx_Buffer + 13;
//				Cali_reglin_inst.attenuation  = *tempf_ptr;
//
//				clear_buf(buf,26);
//				buf[0] = REGLINP_SET;
//				buf[1] = FLASH_write_reglin(&Cali_reglin_inst);
//				Cali_reglin_inst.is_clibrated = 1;
//
//				VCP_DataTx(buf,2);
//				USB_Rx_Cnt = 0;
//			break;
//
//			case SERIALNUM_SET:
//
////				sscanf(chartab, "%u", &serialnum);
//				serialnum = USB_Rx_Buffer[4];
//				serialnum = USB_Rx_Buffer[3] << 8 ;
//				serialnum = USB_Rx_Buffer[2] << 16 ;
//				serialnum = USB_Rx_Buffer[1] << 24 ;
//				clear_buf(buf,26);
//				buf[0] = SERIALNUM_SET;
//				buf[1] = FLASH_write_serialnum(serialnum);
//				VCP_DataTx(buf,2);
//				USB_Rx_Cnt = 0;
//			break;
//
//			case SERIALNUM_GET:
//				clear_buf(buf,10);
//				uint32_t serialnum_tmp = FLASH_read_serialnum();
////				sprintf(buf,"%u", FLASH_read_serialnum());
//				buf[0]  = SERIALNUM_GET;
//				buf[1]  = serialnum_tmp & 0x000000FF;
//				buf[2]  = (serialnum_tmp & 0x0000FF00)>>8;
//				buf[3]  = (serialnum_tmp & 0x00FF0000)>>16;
//				buf[4]  = (serialnum_tmp & 0xFF000000)>>24;
//				VCP_DataTx(buf,5);
//				USB_Rx_Cnt = 0;
//			break;
//
//			case FIRMWARE_VER_GET:
//				buf[0] = FIRMWARE_VER_GET;
//				buf[1] = firmware_version[0];
//				buf[2] = firmware_version[1];
//				buf[3] = firmware_version[2];
//				VCP_DataTx(buf,4);
//				USB_Rx_Cnt = 0;
//			break;
//			default:
//				USB_Rx_Cnt = 0;
//			  break;
//
//		  }
//	  LED_off(LED_BLUE);
//	  }
//}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
