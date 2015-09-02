/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Generic media access Layer.
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

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "usb_conf.h"
#include "HMC624.h"
#include "ADC.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };
char buf[100] = {0};
char chartab[20] = {0};
uint8_t temp_float_tab[4] = {0};
float tempf = 0;
uint32_t temp_peak_detector = 0;

USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
uint16_t VCP_DataTx   (uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);

static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};


void float2Bytes(float val,uint8_t* bytes_array)
{
  // Create union of shared memory space
  union {
    float float_variable;
    uint8_t temp_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}

void double2Bytes(double val,uint8_t* bytes_array)
{
  // Create union of shared memory space
  union {
    double double_variable;
    uint8_t temp_array[8];
  } u;
  // Overite bytes of union with float variable
  u.double_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 8);
}
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{

  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    /* Set the new configuration */
    VCP_COMConfig(OTHER_CONFIG);
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype; 
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
  if (linecoding.datatype == 7)
  {
//    APP_Rx_Buffer[APP_Rx_ptr_in] = USART_ReceiveData(EVAL_COM1) & 0x7F;
  }
  else if (linecoding.datatype == 8)
  {
//    APP_Rx_Buffer[APP_Rx_ptr_in] = USART_ReceiveData(EVAL_COM1);
  }
  int i;

  for(i=0; i<Len; i++)
  {
	  APP_Rx_Buffer[APP_Rx_ptr_in++] = *(Buf+i);
  }

  
  /* To avoid buffer overflow */
  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
  {
    APP_Rx_ptr_in = 0;
  }  
  
  return USBD_OK;
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{

//	switch (Buf[0])
//			  {
//				case POWER_MEASUREMENT:
//					HMC624_set_attenuation(Current_results_inst.current_attenuation);
//					power_measurement(&Current_results_inst, 0);
//					clear_buf(buf,26);
//					buf[0] = POWER_MEASUREMENT;
//					float_pointer = buf+1;
//					float_pointer[0] = Current_results_inst.current_power;
//					float_pointer[1] = Current_results_inst.current_ADC_temperature;
//					float_pointer[2] = Current_results_inst.current_ADC_ref_voltage;
//					float_pointer[3] = Current_results_inst.current_detector_temperature;
//
//
//
//					buf[17] = Current_results_inst.current_error_log_1;
//					buf[18] = Current_results_inst.current_error_log_2;
//
////					VCP_DataTx(buf,19);
//
//				    DCD_EP_Tx (&USB_OTG_dev,
//				               CDC_IN_EP,
//				               buf,
//				               19);
//					USB_Rx_Cnt = 0;
//				break;
//
//				case POWER_MEASUREMENT_KALI:
//
//					set_attenuation_auto();
//					power_measurement(&Current_results_inst, 1);
//					clear_buf(buf,26);
//					buf[0] = POWER_MEASUREMENT_KALI;
//
//					float_pointer = buf+1;
//					*(float_pointer + 0) = Current_results_inst.current_power;
//					*(float_pointer + 1) = Current_results_inst.current_ADC_temperature;
//					*(float_pointer + 2) = Current_results_inst.current_ADC_ref_voltage;
//					*(float_pointer + 3) = Current_results_inst.current_detector_temperature;
//
//
//					buf[17] = Current_results_inst.current_error_log_1;
//					buf[18] = Current_results_inst.current_error_log_2;
//
////					VCP_DataTx(buf,19);
//					DCD_EP_Tx (&USB_OTG_dev,
//							    CDC_IN_EP,
//							    buf,
//								19);
//					USB_Rx_Cnt = 0;
//				break;
//
//				case FREQUENCY_MEASUREMENT:
//					frequency_measurement(&Current_results_inst);
//					clear_buf(buf,26);
//					buf[0] = FREQUENCY_MEASUREMENT;
//					double_pointer = buf + 1;
//					float_pointer = buf + 9;
//
//					double2Bytes(Current_results_inst.current_frequency, buf+1);
//					//*double_pointer = Current_results_inst.current_frequency;
//					*(float_pointer + 0) = Current_results_inst.current_ADC_temperature;
//					*(float_pointer + 1) = Current_results_inst.current_ADC_ref_voltage;
//					*(float_pointer + 2) = Current_results_inst.current_detector_temperature;
//
//
//					buf[21] = Current_results_inst.current_error_log_1;
//					buf[22] = Current_results_inst.current_error_log_2;
//
////					VCP_DataTx(buf,23);
//					DCD_EP_Tx (&USB_OTG_dev,
//								CDC_IN_EP,
//								buf,
//								23);
//
//					USB_Rx_Cnt = 0;
//				break;
//
//				case ATTENUATION_SET:
//				for (i=1; i<USB_Rx_Cnt; i++)
//				{
//					chartab[i-1] = Buf[i];
//				}
//
//
//				tempf_ptr = chartab;
//				set_attenuation(&Current_results_inst, *tempf_ptr);
//				clear_buf(buf,26);
//				buf[0] = ATTENUATION_SET;
//				buf[1] = 0x01;
//				//VCP_DataTx(buf,2);
//				DCD_EP_Tx (&USB_OTG_dev,
//							CDC_IN_EP,
//							buf,
//							2);
//
//	 			USB_Rx_Cnt = 0;
//				break;
//
//				case REGLINP_SET:
//
//					LED_on(LED_BLUE);
//
//					tempf_ptr = Buf + 1;
//					Cali_reglin_inst.reglin_a = *tempf_ptr;
//					tempf_ptr = Buf + 5;
//					Cali_reglin_inst.reglin_b = *tempf_ptr;
//					tempf_ptr = Buf + 9;
//					Cali_reglin_inst.VCC = *tempf_ptr;
//					tempf_ptr = Buf + 13;
//					Cali_reglin_inst.attenuation  = *tempf_ptr;
//
//					clear_buf(buf,26);
//					buf[0] = REGLINP_SET;
//					buf[1] = FLASH_write_reglin(&Cali_reglin_inst);
//					Cali_reglin_inst.is_clibrated = 1;
//
//					DCD_EP_Tx (&USB_OTG_dev,
//								CDC_IN_EP,
//								buf,
//								2);
//					//VCP_DataTx(buf,2);
//					USB_Rx_Cnt = 0;
//				break;
//
//				case SERIALNUM_SET:
//
//	//				sscanf(chartab, "%u", &serialnum);
//					serialnum = Buf[4];
//					serialnum = Buf[3] << 8 ;
//					serialnum = Buf[2] << 16 ;
//					serialnum = Buf[1] << 24 ;
//					clear_buf(buf,26);
//					buf[0] = SERIALNUM_SET;
//					buf[1] = FLASH_write_serialnum(serialnum);
//					//VCP_DataTx(buf,2);
//					DCD_EP_Tx (&USB_OTG_dev,
//								CDC_IN_EP,
//								buf,
//								2);
//					USB_Rx_Cnt = 0;
//				break;
//
//				case SERIALNUM_GET:
//					clear_buf(buf,10);
//					uint32_t serialnum_tmp = FLASH_read_serialnum();
//	//				sprintf(buf,"%u", FLASH_read_serialnum());
//					buf[0]  = SERIALNUM_GET;
//					buf[1]  = serialnum_tmp & 0x000000FF;
//					buf[2]  = (serialnum_tmp & 0x0000FF00)>>8;
//					buf[3]  = (serialnum_tmp & 0x00FF0000)>>16;
//					buf[4]  = (serialnum_tmp & 0xFF000000)>>24;
//					DCD_EP_Tx (&USB_OTG_dev,
//								CDC_IN_EP,
//								buf,
//								5);
//					//VCP_DataTx(buf,5);
//					USB_Rx_Cnt = 0;
//				break;
//
//				case FIRMWARE_VER_GET:
//					buf[0] = FIRMWARE_VER_GET;
//					buf[1] = firmware_version[0];
//					buf[2] = firmware_version[1];
//					buf[3] = firmware_version[2];
//					DCD_EP_Tx (&USB_OTG_dev,
//								CDC_IN_EP,
//								buf,
//								4);
//					//VCP_DataTx(buf,4);
//					USB_Rx_Cnt = 0;
//				break;
//				default:
//					USB_Rx_Cnt = 0;
//				  break;
//
//			  }
//		  LED_off(LED_BLUE);



	LED_on(LED_BLUE);

		  switch (Buf[0])
		  {
			case POWER_MEASUREMENT:

				HMC624_set_attenuation(Current_results_inst.current_attenuation);
				power_measurement(&Current_results_inst, 0);
				clear_buf(buf,26);
				buf[0] = POWER_MEASUREMENT;
				//buf+1 = Current_results_inst.current_power;
				float2Bytes(Current_results_inst.current_power, buf+1);
				float2Bytes(Current_results_inst.current_ADC_temperature, buf+5);
				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+9);
				float2Bytes(Current_results_inst.current_detector_temperature, buf+13);


				buf[17] = Current_results_inst.current_error_log_1;
				buf[18] = Current_results_inst.current_error_log_2;

			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               19);
//				USB_Rx_Cnt = 0;
			break;

			case POWER_MEASUREMENT_KALI:

				set_attenuation_auto();
				power_measurement(&Current_results_inst, 1);
				clear_buf(buf,26);
				buf[0] = POWER_MEASUREMENT_KALI;
				float2Bytes(Current_results_inst.current_power, buf+1);
				float2Bytes(Current_results_inst.current_ADC_temperature, buf+5);
				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+9);
				float2Bytes(Current_results_inst.current_detector_temperature, buf+13);

				buf[17] = Current_results_inst.current_error_log_1;
				buf[18] = Current_results_inst.current_error_log_2;

			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               19);
//				USB_Rx_Cnt = 0;
			break;

			case FREQUENCY_MEASUREMENT:
				frequency_measurement(&Current_results_inst);
				clear_buf(buf,26);
				buf[0] = FREQUENCY_MEASUREMENT;
				double2Bytes(Current_results_inst.current_frequency, buf+1);
				float2Bytes(Current_results_inst.current_ADC_temperature, buf+9);
				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+13);
				float2Bytes(Current_results_inst.current_detector_temperature, buf+17);


				buf[21] = Current_results_inst.current_error_log_1;
				buf[22] = Current_results_inst.current_error_log_2;

			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               23);

//				USB_Rx_Cnt = 0;
			break;

			case ATTENUATION_SET:
			for (i=1; i<USB_Rx_Cnt; i++)
			{
				chartab[i-1] = Buf[i];
			}
//			sscanf(chartab, "%f", &tempf);
//			tempf = (float)atof(chartab);

			tempf_ptr = chartab;
			set_attenuation(&Current_results_inst, *tempf_ptr);
			clear_buf(buf,26);
			buf[0] = ATTENUATION_SET;
			buf[1] = 0x01;
		    DCD_EP_Tx (&USB_OTG_dev,
		               CDC_IN_EP,
		               buf,
		               2);

// 			USB_Rx_Cnt = 0;
			break;

			case REGLINP_SET:
				tempf_ptr = Buf + 1;
				Cali_reglin_inst.reglin_a = *tempf_ptr;
				tempf_ptr = Buf + 5;
				Cali_reglin_inst.reglin_b = *tempf_ptr;
				tempf_ptr = Buf + 9;
				Cali_reglin_inst.VCC = *tempf_ptr;
				tempf_ptr = Buf + 13;
				Cali_reglin_inst.attenuation  = *tempf_ptr;
				Cali_reglin_inst.is_clibrated = 1;

				clear_buf(buf,26);
				buf[0] = REGLINP_SET;
				buf[1] = FLASH_write_reglin(&Cali_reglin_inst);

			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               2);
//				USB_Rx_Cnt = 0;
			break;

			case SERIALNUM_SET:

//				sscanf(chartab, "%u", &serialnum);
				serialnum = Buf[4];
				serialnum = Buf[3] << 8 ;
				serialnum = Buf[2] << 16 ;
				serialnum = Buf[1] << 24 ;
				clear_buf(buf,26);
				buf[0] = SERIALNUM_SET;
				buf[1] = FLASH_write_serialnum(serialnum);
			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               2);
//				USB_Rx_Cnt = 0;
			break;

			case SERIALNUM_GET:
				clear_buf(buf,10);
				uint32_t serialnum_tmp = FLASH_read_serialnum();
//				sprintf(buf,"%u", FLASH_read_serialnum());
				buf[0]  = SERIALNUM_GET;
				buf[1]  = serialnum_tmp & 0x000000FF;
				buf[2]  = (serialnum_tmp & 0x0000FF00)>>8;
				buf[3]  = (serialnum_tmp & 0x00FF0000)>>16;
				buf[4]  = (serialnum_tmp & 0xFF000000)>>24;
//				VCP_DataTx(buf,5);
			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               5);
//				USB_Rx_Cnt = 0;
			break;

			case FIRMWARE_VER_GET:
				buf[0] = FIRMWARE_VER_GET;
				buf[1] = firmware_version[0];
				buf[2] = firmware_version[1];
				buf[3] = firmware_version[2];
//				VCP_DataTx(buf,4);
			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               4);
//				USB_Rx_Cnt = 0;
			break;
			case PEAK_DETECTOR:

				HMC624_set_attenuation(Current_results_inst.current_attenuation);
				Current_results_inst.current_peak_value = ADC_peak_measurement(&Current_results_inst);


				Current_result_clear(&Current_results_inst);
				HMC849_set_mode(HMC849_POWER);
				if(Cali_reglin_inst.is_clibrated !=1)Current_results_inst.current_error_log_1 |= ERROR_CALIBRATION;
				if(!Current_results_inst.current_is_warmed_up)Current_results_inst.current_error_log_1 |= ERROR_WARM_UP;
				if(!Cali_reglin_inst.is_serialnum_set)Current_results_inst.current_error_log_1 |= ERROR_SERIAL_NUM;
				TIM_delay_ms(1);
				ADC_temperature_measurement(&Current_results_inst, &ADC_ST_values_inst);
				ADC_reference_V_measurement(&Current_results_inst, &ADC_ST_values_inst);


				for(i = 0; i<33333; i++)
				{
					temp_peak_detector = ADC_peak_measurement(&Current_results_inst);
					if( temp_peak_detector < Current_results_inst.current_peak_value) Current_results_inst.current_peak_value = temp_peak_detector;
				}

				tempf = (float)Current_results_inst.current_peak_value/10.0f;
				tempf = tempf *  Cali_reglin_inst.VCC/4095.0f;

				tempf = Cali_reglin_inst.reglin_a * tempf + Cali_reglin_inst.reglin_b + (Current_results_inst.current_attenuation - Cali_reglin_inst.attenuation);
				buf[0] = PEAK_DETECTOR;
				float2Bytes(tempf, buf+1);
				float2Bytes(Current_results_inst.current_ADC_temperature, buf+5);
				float2Bytes(Current_results_inst.current_ADC_ref_voltage, buf+9);
				float2Bytes(Current_results_inst.current_detector_temperature, buf+13);


				buf[17] = Current_results_inst.current_error_log_1;
				buf[18] = Current_results_inst.current_error_log_2;


			    DCD_EP_Tx (&USB_OTG_dev,
			               CDC_IN_EP,
			               buf,
			               19);

			    HMC849_disable();
				if(Current_results_inst.current_error_log_1)LED_on(LED_RED);
//				USB_Rx_Cnt = 0;
			break;
			default:
//				USB_Rx_Cnt = 0;
			  break;

		  }
		  LED_off(LED_BLUE);
 
  return USBD_OK;
}

/**
  * @brief  VCP_COMConfig
  *         Configure the COM Port with default values or values received from host.
  * @param  Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG
  *         to set a configuration received from the host.
  * @retval None.
  */
static uint16_t VCP_COMConfig(uint8_t Conf)
{

  return USBD_OK;
}

/**
  * @brief  EVAL_COM_IRQHandler
  *         
  * @param  None.
  * @retval None.
  */
void USART3_IRQHandler(void)
{
	  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	  {
	    /* Send the received data to the PC Host*/
	    VCP_DataTx (0,0);
	  }

	  /* If overrun condition occurs, clear the ORE flag and recover communication */
	  if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	  {
	    (void)USART_ReceiveData(USART3);
	  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


