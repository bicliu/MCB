/**
  ******************************************************************************
  * @file    MYPWM.c
  * @author  Bic
  * @version V1.0.0
  * @date    17-October-2017
  * @brief   This file contains all the functions prototypes for the Usart.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "MyPWM.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MyPWM_IOENABLE    Bit_RESET
#define MyPWM_IODISABLE   Bit_SET
#define MyPWM_CW    Bit_RESET
#define MyPWM_ACW   Bit_SET

/*tim period 20000 Hz*/
#define PWM_DEFAULT_PERIOD    20000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t u16MyPWMPeriod = 0;
uint16_t u16MyPWMPulse = 0;

tMyPWM_PWMCtl_t tMyPWMCtlArray[4];
//uint32_t ui4PWMTIM1TotalStep = 0,ui4PWMTIM2TotalStep = 0,ui4PWMTIM3TotalStep = 0,ui4PWMTIM4TotalStep = 0;
//uint32_t ui4PWMTIM1StepCount = 0,ui4PWMTIM2StepCount = 0,ui4PWMTIM3StepCount = 0,ui4PWMTIM4StepCount = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
	
void MyPWM_RCC_Configuration(void)
{
  /* TIM2CH2, TIM3_CH1, TIM1_CH1, TIM4_CH1
		GPIOA 1 , GPIOA 6, GPIOA 8, GPIOB 6 clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | 
												RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
}

void MyPWM_GPIO_Configuration(void)
{
	/*PA8   PA1     PA6    PB6
	  TIM1  TIM2    TIM3   TIM4*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void MyPWM_CtlIO_Config(void)
{
	int i = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	tMyPWMCtlArray[0].tEnableIO.rcc = RCC_APB2Periph_GPIOB;
	tMyPWMCtlArray[0].tEnableIO.gpio = GPIOB;
	tMyPWMCtlArray[0].tEnableIO.pin = GPIO_Pin_12;
	tMyPWMCtlArray[0].tClkWiseIO.rcc = RCC_APB2Periph_GPIOC;
	tMyPWMCtlArray[0].tClkWiseIO.gpio = GPIOC;
	tMyPWMCtlArray[0].tClkWiseIO.pin = GPIO_Pin_6;
	
	tMyPWMCtlArray[1].tEnableIO.rcc = RCC_APB2Periph_GPIOB;
	tMyPWMCtlArray[1].tEnableIO.gpio = GPIOB;
	tMyPWMCtlArray[1].tEnableIO.pin = GPIO_Pin_13;
	tMyPWMCtlArray[1].tClkWiseIO.rcc = RCC_APB2Periph_GPIOC;
	tMyPWMCtlArray[1].tClkWiseIO.gpio = GPIOC;
	tMyPWMCtlArray[1].tClkWiseIO.pin = GPIO_Pin_7;
	
	tMyPWMCtlArray[2].tEnableIO.rcc = RCC_APB2Periph_GPIOB;
	tMyPWMCtlArray[2].tEnableIO.gpio = GPIOB;
	tMyPWMCtlArray[2].tEnableIO.pin = GPIO_Pin_14;
	tMyPWMCtlArray[2].tClkWiseIO.rcc = RCC_APB2Periph_GPIOC;
	tMyPWMCtlArray[2].tClkWiseIO.gpio = GPIOC;
	tMyPWMCtlArray[2].tClkWiseIO.pin = GPIO_Pin_8;
	
	tMyPWMCtlArray[3].tEnableIO.rcc = RCC_APB2Periph_GPIOB;
	tMyPWMCtlArray[3].tEnableIO.gpio = GPIOB;
	tMyPWMCtlArray[3].tEnableIO.pin = GPIO_Pin_15;
	tMyPWMCtlArray[3].tClkWiseIO.rcc = RCC_APB2Periph_GPIOC;
	tMyPWMCtlArray[3].tClkWiseIO.gpio = GPIOC;
	tMyPWMCtlArray[3].tClkWiseIO.pin = GPIO_Pin_9;
	
	for(i = 0; i < 4; i++)
	{
		RCC_APB2PeriphClockCmd(tMyPWMCtlArray[i].tEnableIO.rcc, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = tMyPWMCtlArray[i].tEnableIO.pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(tMyPWMCtlArray[i].tEnableIO.gpio, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(tMyPWMCtlArray[i].tClkWiseIO.rcc, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = tMyPWMCtlArray[i].tClkWiseIO.pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(tMyPWMCtlArray[i].tClkWiseIO.gpio, &GPIO_InitStructure);
	}
}

void MyPWM_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

void MyPWM_TIM_periodCalc(uint32_t uiPeriod, uint32_t uiDutyCycle)
{
	u16MyPWMPeriod = (SystemCoreClock / uiPeriod ) - 1;
	if(uiDutyCycle <= 100)
	{
		u16MyPWMPulse = (uint16_t) (((uint32_t) uiDutyCycle * (u16MyPWMPeriod - 1)) / 100);
	}
	else
	{
		u16MyPWMPulse = (uint16_t) (((uint32_t) 50 * (u16MyPWMPeriod - 1)) / 100);
	}
}

void MyPWM_TIM1_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16MyPWMPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16MyPWMPulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	/* TIM1 Arr reload Enable */
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
}

void MyPWM_TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16MyPWMPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16MyPWMPulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	/* TIM1 Arr reload Enable */
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
}

void MyPWM_TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16MyPWMPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16MyPWMPulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	/* TIM1 Arr reload Enable */
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
}

void MyPWM_TIM4_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16MyPWMPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16MyPWMPulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	
	/* TIM1 Arr reload Enable */
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);
}

void MyPWM_TIM_Init(void)
{
	MyPWM_RCC_Configuration();
	
	MyPWM_GPIO_Configuration();
	
	//MyPWM_NVIC_Configuration();
	
	MyPWM_TIM_periodCalc(PWM_DEFAULT_PERIOD, 50);
	
	MyPWM_TIM1_Config();
	
	MyPWM_TIM2_Config();
	
	MyPWM_TIM3_Config();
	
	MyPWM_TIM4_Config();
	
	MyPWM_CtlIO_Config();
}

void MyPWM_TIM1_Run(void)
{
	TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM2); 
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
	
	/*clear flag & enable IT*/
	//TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
	//TIM_ITConfig(TIM1,TIM_IT_CC1,ENABLE);
	
	/* TIM counter enable */
  	TIM_Cmd(TIM1, ENABLE);
	
	/* TIM Main Output Enable */
  	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void MyPWM_TIM1_Stop(void)
{
	TIM_Cmd(TIM1, DISABLE);
	//TIM_ITConfig(TIM1,TIM_IT_CC1,DISABLE);
	
	//TIM_ForcedOC1Config(TIM1, TIM_ForcedAction_InActive);
	//TIM_ForcedOC2Config(TIM1, TIM_ForcedAction_InActive);
}

void MyPWM_TIM1_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle)
{
	MyPWM_TIM_periodCalc(uiPeriod,uiDutyCycle);
	TIM1->ARR = u16MyPWMPeriod;
	//TIM1->CCR1 = u16MyPWMPulse;
	TIM_SetCompare1(TIM1, u16MyPWMPulse);
}

void MyPWM_TIM2_Run(void)
{
	TIM_SelectOCxM(TIM2, TIM_Channel_2, TIM_OCMode_PWM2); 
	TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Enable);
	
	/*clear flag & enable IT*/
	//TIM_ClearFlag(TIM2, TIM_FLAG_CC1);
	//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
	
	/* TIM counter enable */
  	TIM_Cmd(TIM2, ENABLE);
	
	/* TIM Main Output Enable */
  	TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

void MyPWM_TIM2_Stop(void)
{
	TIM_Cmd(TIM2, DISABLE);
	//TIM_ITConfig(TIM2,TIM_IT_CC1,DISABLE);
	
	//TIM_ForcedOC1Config(TIM2, TIM_ForcedAction_InActive);
	//TIM_ForcedOC2Config(TIM2, TIM_ForcedAction_InActive);
}

void MyPWM_TIM2_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle)
{
	MyPWM_TIM_periodCalc(uiPeriod,uiDutyCycle);
	TIM2->ARR = u16MyPWMPeriod;
	//TIM2->CCR2 = u16MyPWMPulse;
	TIM_SetCompare2(TIM2, u16MyPWMPulse);
}

void MyPWM_TIM3_Run(void)
{
	TIM_SelectOCxM(TIM3, TIM_Channel_1, TIM_OCMode_PWM2); 
	TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);
	
	/*clear flag & enable IT*/
	//TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
	//TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
	
	/* TIM counter enable */
  	TIM_Cmd(TIM3, ENABLE);
	
	/* TIM Main Output Enable */
  	TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void MyPWM_TIM3_Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);
	//IM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
	
	//TIM_ForcedOC1Config(TIM3, TIM_ForcedAction_InActive);
	//TIM_ForcedOC2Config(TIM3, TIM_ForcedAction_InActive);
}

void MyPWM_TIM3_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle)
{
	MyPWM_TIM_periodCalc(uiPeriod,uiDutyCycle);
	TIM3->ARR = u16MyPWMPeriod;
	//TIM3->CCR1 = u16MyPWMPulse;
	TIM_SetCompare1(TIM3, u16MyPWMPulse);
}

void MyPWM_TIM4_Run(void)
{
	TIM_SelectOCxM(TIM4, TIM_Channel_1, TIM_OCMode_PWM2); 
	TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable);
	
	/*clear flag & enable IT*/
	//TIM_ClearFlag(TIM4, TIM_FLAG_CC1);
	//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);
	
	/* TIM counter enable */
  	TIM_Cmd(TIM4, ENABLE);
	
	/* TIM Main Output Enable */
  	TIM_CtrlPWMOutputs(TIM4, ENABLE);
}

void MyPWM_TIM4_Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);
	//IM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);
	
	//TIM_ForcedOC1Config(TIM4, TIM_ForcedAction_InActive);
	//TIM_ForcedOC2Config(TIM4, TIM_ForcedAction_InActive);
}

void MyPWM_TIM4_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle)
{
	MyPWM_TIM_periodCalc(uiPeriod,uiDutyCycle);
	TIM4->ARR = u16MyPWMPeriod;
	//TIM4->CCR1 = u16MyPWMPulse;
	TIM_SetCompare1(TIM4, u16MyPWMPulse);
}

void MyPWM_TIM_IOEnable(int index)
{
	if(index >= 0 && index < 3)
	{
		GPIO_WriteBit(tMyPWMCtlArray[index].tEnableIO.gpio, tMyPWMCtlArray[index].tEnableIO.pin, MyPWM_IOENABLE);
	}
}

void MyPWM_TIM_IODisable(int index)
{
	if(index >= 0 && index < 3)
	{
		GPIO_WriteBit(tMyPWMCtlArray[index].tEnableIO.gpio, tMyPWMCtlArray[index].tEnableIO.pin, MyPWM_IODISABLE);
	}
}

void MyPWM_TIM_ClkWise(int index)
{
	if(index >= 0 && index < 3)
	{
		GPIO_WriteBit(tMyPWMCtlArray[index].tClkWiseIO.gpio, tMyPWMCtlArray[index].tClkWiseIO.pin, MyPWM_CW);
	}
}

void MyPWM_TIM_AntiClkWise(int index)
{
	if(index >= 0 && index < 3)
	{
		GPIO_WriteBit(tMyPWMCtlArray[index].tClkWiseIO.gpio, tMyPWMCtlArray[index].tClkWiseIO.pin, MyPWM_ACW);
	}
}
