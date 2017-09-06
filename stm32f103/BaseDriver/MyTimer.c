#include "MyTimer.h"
#include "MyGpio.h"

uint32_t mytimer_arr;
uint16_t mytimer_psc;

uint32_t PB1_TIM_ARR = 1000;
uint16_t PB1_TIM_PSC = 26;
uint32_t PB2_TIM_ARR = 1000;
uint16_t PB2_TIM_PSC = 52;
uint32_t TIM_CLOCK_BASE = 1000000;

uint32_t mytimer_stepscount = 0;


void MyTimer_Start(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//uint16_t ChannelPulse = 0;
	/* TIM clock enable */
	if(MYTIMER_ISAPB1 == 1)
	{
    RCC_APB1PeriphClockCmd(MYTIMER_TIM_RCC, ENABLE);
		mytimer_arr = PB1_TIM_ARR;
		mytimer_psc = PB1_TIM_PSC;
	}
	else
	{
		RCC_APB2PeriphClockCmd(MYTIMER_TIM_RCC, ENABLE);
		mytimer_arr = PB2_TIM_ARR;
		mytimer_psc = PB2_TIM_PSC;
	}
	
	/*NVIC config*/
	NVIC_InitStructure.NVIC_IRQChannel = MYTIMER_TIM_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);
	
	//ChannelPulse = (uint16_t) (((uint32_t) 5 * (mytimer_arr - 1)) / 10);
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Period = mytimer_arr;
  TIM_TimeBaseStructure.TIM_Prescaler =(mytimer_psc-1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
  TIM_TimeBaseInit(MYTIMER_TIM, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(MYTIMER_TIM, MYTIMER_TIM_ITFLAG);
	TIM_ITConfig(MYTIMER_TIM,MYTIMER_TIM_IT,ENABLE);
  
	/* TIM counter enable */
  TIM_Cmd(MYTIMER_TIM, ENABLE);
}

void MyTimer_IT_Handler(void)
{
	if(TIM_GetITStatus(MYTIMER_TIM, MYTIMER_TIM_ITFLAG) != RESET)
	{
		mytimer_stepscount++;
		//Motor_ScanLmtBit();
		//Motor_ScanLmtBit_test();//for test
		//Motor_ScanREAD0Bit_test();
		if(mytimer_stepscount > 1000)
		{
			LED_Flashing();
			mytimer_stepscount = 0;
		}
	}
	TIM_ClearITPendingBit(MYTIMER_TIM, MYTIMER_TIM_ITFLAG);
}

void Timer_Init(Tim_Info timer)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//uint16_t ChannelPulse = 0;
	/* TIM clock enable */
	if(timer.isAPB1 == 1)
	{
    RCC_APB1PeriphClockCmd(timer.rcc, ENABLE);
		timer.arr = PB1_TIM_ARR;
		timer.psc = PB1_TIM_PSC;
	}
	else
	{
		RCC_APB2PeriphClockCmd(timer.rcc, ENABLE);
		timer.arr = PB2_TIM_ARR;
		timer.psc = PB2_TIM_PSC;
	}
	
	/*NVIC config*/
	NVIC_InitStructure.NVIC_IRQChannel = timer.irqn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);
	
	//ChannelPulse = (uint16_t) (((uint32_t) 5 * (mytimer_arr - 1)) / 10);
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Period = timer.arr;
  TIM_TimeBaseStructure.TIM_Prescaler =(timer.psc-1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
  TIM_TimeBaseInit(timer.name, &TIM_TimeBaseStructure);
}

void Timer_Start(Tim_Info timer)
{
	TIM_ClearFlag(timer.name, timer.itflag);
	TIM_ITConfig(timer.name,timer.it,ENABLE);
  
	/* TIM counter enable */
  TIM_Cmd(timer.name, ENABLE);
}

void Timer_Stop(Tim_Info timer)
{
	TIM_Cmd(timer.name, DISABLE);
}
