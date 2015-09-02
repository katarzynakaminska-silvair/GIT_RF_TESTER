#include "TIM.h"

void TIM_delay_ms(uint16_t time)
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
	while (TIM4->CNT <= 2*time); 			// delay time ms
	TIM4->CR1 &= ~TIM_CR1_CEN;			//Counter disable
}

void TIM_delay_us(uint16_t time)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; 		// TIM4 clock enable
	TIM4->PSC = 42;								// TIM4 prescaler: 84MHZ/(PSC+1)
	TIM4->CR1 &= ~TIM_CR1_DIR;					// Counter used as upcounter
	TIM4->CR1 |= TIM_CR1_ARPE;					// Auto-reload preload enable
	TIM4->ARR = 65535;	///max time=65535		// TIM4 auto-reload value
	TIM4->EGR |= TIM_EGR_UG;					// Update generation
	TIM4->CR1 |= TIM_CR1_CEN;					//Counter enable

	TIM4->CNT = 0;						// counter reset
	TIM4->CR1 |= TIM_CR1_CEN;			//Counter enable
	while (TIM4->CNT <= 2*time); 			// delay time ms
	TIM4->CR1 &= ~TIM_CR1_CEN;			//Counter disable
}

inline void TIM_warm_up(void)
{
	set_warm_up_state(0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Prescaler = 41999; //84M/42k = 2k
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 2000-1;  //2kHz down to 1Hz (1s)
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TIM_InitStruct);

 	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
 	TIM_Cmd(TIM3,ENABLE);

}

void TIM2_init(void)
{
	RCC->AHB1ENR |= 0x01; // Clock for PortA
	RCC->APB1ENR |= 0x01; // Clock for Timer2
	RCC->APB1ENR |= 0x08; // Clock for Timer5

	GPIOA->MODER |= 0x00000008; // all inputs but: PA1 => AF mode
	GPIOA->AFR[0] |= 0x00000010; // select AF1 (TIM2) for PA1 -> TIM2_CH2


	TIM2->CCMR1 |= 0x0100; // Ch. 2 as TI2 //CC2S = 01
	TIM2->SMCR |= 0x0007; // Ext. clk mode 1 //SMS = 111
	TIM2->SMCR |= 0x0060; // TI2FP2 as ext. clock //TS = 110
	TIM2->SMCR |= 0x0080;// MSM = 1
	TIM2->CR2  |= 0x0010; //send enable of tim2 as trigger to tim5 //MMS = 001

	TIM5->SMCR |= 0x0006; //TIM5 in trig mode

	TIM2->CR1 |= 0x0001; // enable counting


//	TIM2->CCMR1 |= 0x0100; // Ch. 2 as TI2 //CC2S = 01
//	TIM2->SMCR |= 0x0007; // Ext. clk mode 1 //SMS = 111
//	TIM2->SMCR |= 0x0060; // TI2FP2 as ext. clock //TS = 110
//	TIM2->CR1 |= 0x0001; // enable counting



}










//void TIM2_init(void)
//{
//	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 		// TIM2 clock enable
//	TIM2->PSC = 0;								// TIM4 prescaler
//	TIM2->CR1 &= ~TIM_CR1_DIR;					// Counter used as upcounter
//	TIM2->CR1 |= TIM_CR1_ARPE;					// Auto-reload preload enable
//	TIM2->ARR = 65535;	///max time=65535		// TIM4 auto-reload value
//	TIM2->EGR |= TIM_EGR_UG;					// Update generation
//	TIM2->CR1 |= TIM_CR1_CEN;					//Counter enable

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

//	GPIO_InitTypeDef GPIO_InitStructure;
	  // TIM2_ETR pin configuration
//	GPIO_InitStructure.GPIO_Pin = SN74LV_COUNTER_OUTPUT_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//


//	GPIO_PinAFConfig(GPIOA, SN74LV_COUNTER_OUTPUT_1, GPIO_AF_TIM2);
//	TIM2->CCMR1 &= ~ TIM_CCMR1_CC2S_1; //0x0100; // Ch. 2 as TI2
//	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
//	TIM2->SMCR |= TIM_SMCR_SMS_2;//0x0007; // Ext. clk mode 1
//	TIM2->SMCR |= TIM_SMCR_SMS_1;
//	TIM2->SMCR |= TIM_SMCR_SMS_0;
//	TIM2->SMCR &= ~TIM_SMCR_TS_0;//0x0060; // TI2FP2 as ext. clock
//	TIM2->SMCR |= TIM_SMCR_TS_1;
//	TIM2->SMCR |= TIM_SMCR_TS_2;
//	TIM2->CR1 |= TIM_CR1_CEN;//0x0001; // enable counting
//
	//for ETR
//////////////////////////////////////////////////////////////////
//	TIM2->SMCR &= ~ TIM_SMCR_ETF_3; // filter 0
//	TIM2->SMCR &= ~ TIM_SMCR_ETF_2;
//	TIM2->SMCR &= ~ TIM_SMCR_ETF_1;
//	TIM2->SMCR &= ~ TIM_SMCR_ETF_0;
//	TIM2->SMCR &= ~ TIM_SMCR_ETPS_1; //select external signal prescaler 0
//	TIM2->SMCR &= ~ TIM_SMCR_ETPS_0;
//	TIM2->SMCR &= ~ TIM_SMCR_ETP;//select rising edge detection on the ETR pin
//	TIM2->SMCR |= TIM_SMCR_ECE; //select external source clock mode 2
//	TIM2->CR1 |= TIM_CR1_CEN;					//Counter enable
	////////////////////////////////////////////////////////////////
//}


//void TIM2_init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_ICInitTypeDef  TIM_ICInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	  //Enable Clock for Timer2
//	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//
//	  // TIM2_ETR pin configuration
//	  GPIO_InitStructure.GPIO_Pin = SN74LV_COUNTER_OUTPUT_1;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//GPIO_Mode_IN;
//	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	  GPIO_PinAFConfig(GPIOA, SN74LV_COUNTER_OUTPUT_1, GPIO_AF_TIM2);
//
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
//	  /* Enable the TIM2 global Interrupt */
//	  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	  NVIC_Init(&NVIC_InitStructure);
//
//	  TIM_TimeBaseStructure.TIM_Period = 65535;
//	  TIM_TimeBaseStructure.TIM_Prescaler = 0;
//	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//
//	 // TIM_ETRClockMode1Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);
//
//	  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
//	  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//	  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//	  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//	  TIM_ICInitStructure.TIM_ICFilter = 0x0;
//	  TIM_ICInit(TIM2, &TIM_ICInitStructure);
//
//	  TIM_Cmd(TIM2, ENABLE);
//	  /* Enable the CC2 Interrupt Request */
//	  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
////	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
//}
