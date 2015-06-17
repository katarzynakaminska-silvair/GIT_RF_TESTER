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



  
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

void delay_TIMER_ms(uint16_t time)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; 		// TIM4 clock enable
	TIM4->PSC = 41999;							// TIM4 prescaler: 84MHZ/(PSC+1)
	TIM4->CR1 &= ~TIM_CR1_DIR;					// Counter used as upcounter
	TIM4->CR1 |= TIM_CR1_ARPE;					// Auto-reload preload enable
	TIM4->ARR = 65535;	///max time=65535		// TIM4 auto-reload value
	TIM4->EGR |= TIM_EGR_UG;					// Update generation
	TIM4->CR1 |= TIM_CR1_CEN;					//Counter enable

	TIM4->CNT = 0;						// counter reset
	TIM4->CR1 |= TIM_CR1_CEN;			//Counter enable
	while (TIM4->CNT <= time); 			// delay time us
	TIM4->CR1 &= ~TIM_CR1_CEN;			//Counter disable
}




int main(void)
{
  __IO uint32_t i = 0;


	GPIO_InitTypeDef LED_GPIO_InitTypeDef;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	LED_GPIO_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	LED_GPIO_InitTypeDef.GPIO_OType = GPIO_OType_PP;
	LED_GPIO_InitTypeDef.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	LED_GPIO_InitTypeDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	LED_GPIO_InitTypeDef.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &LED_GPIO_InitTypeDef);





  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */  
 
  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
  
GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
  uint8_t temp[4] = {'t','e','s','t'};
  /* Main loop */
  while (1)
  {
	//(*VCP_DataTx_ptr)(&temp,4);

	  GPIO_ToggleBits(GPIOD, GPIO_Pin_0);
	  delay_TIMER_ms(20);

  }
} 

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
