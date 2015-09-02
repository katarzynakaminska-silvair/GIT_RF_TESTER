/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Header for usbd_cdc_vcp.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core.h"
#include "usbd_conf.h"

#include "measurement functions.h"
#include "FLASH.h"

#include "ADC.h"
#include "LED.h"
#include "FLASH.h"
#include "SN74LV.h"
#include "HMC849.h"
#include "TIM.h"

extern Cali_reglin Cali_reglin_inst;
extern Current_results Current_results_inst;

enum command_codes { 	POWER_MEASUREMENT = 0x01,
						POWER_MEASUREMENT_KALI = 0x02,
						FREQUENCY_MEASUREMENT = 0x03,
						ATTENUATION_SET = 0x04,
						REGLINP_SET = 0x05,
						SERIALNUM_SET = 0x06,
						SERIALNUM_GET = 0x07,
						FIRMWARE_VER_GET = 0x08,
						PEAK_DETECTOR = 0x09
					};
extern uint8_t USB_Rx_Buffer   [CDC_DATA_MAX_PACKET_SIZE];
extern uint32_t APP_Rx_length ;
extern uint16_t USB_Rx_Cnt;
extern char buf[100];
extern uint8_t is_warmed_up;
uint32_t serialnum;
extern char chartab[20];
int i;//, k = 0, m = 0;
extern float tempf;
float* tempf_ptr;
extern uint8_t temp_float_tab[4];
//float *float_pointer;
//double *double_pointer;
static uint8_t firmware_version[3] = {1,1,2}; //major, minor, release
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;

/* Exported typef ------------------------------------------------------------*/
/* The following structures groups all needed parameters to be configured for the 
   ComPort. These parameters can modified on the fly by the host through CDC class
   command class requests. */
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/
/* The following define is used to route the USART IRQ handler to be used.
   The IRQ handler function is implemented in the usbd_cdc_vcp.c file. */
          


#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len);

#endif /* __USBD_CDC_VCP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
