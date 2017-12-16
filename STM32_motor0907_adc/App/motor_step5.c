#include "stm32f10x.h"
#include "motor_step5.h"
#include "SysTickDelay.h"
#include "stm32f10x_adc.h"
#include <stdio.h>

#define MAX_PHASE	10
static char step5_phase = MAX_PHASE -1 ;
volatile bool clock_wise = TRUE;
volatile bool run_enable = TRUE;
#define PWM_DEFAULT_FRE (400-1)//1800
volatile  int pwm_fre = PWM_DEFAULT_FRE; //20k
#define DEFAULT_CURRENT 30//20~80 // %
volatile int motor_current = 0; //pwm_duty 8%~ 96%
#define DUTY_STEP  25 // 
//static int pwm_duty_gap =76;// pwm_fre/DUTY_STEP;
//static char duty_count = 0;
#define DEFAULT_SPEED 5000 //5ms ->5000 /STEP
static int target_speed = DEFAULT_SPEED;// 0.125s * 10 /DUTY_STEP =25ms  125000*10/50=
static int start_speed = DEFAULT_SPEED;
static int step5_acc = 0;
#define CIRCLE_STEP 500 // 500 STEP PER CIRCLE
volatile int step_counter = 0;
int target_step = 500;
static int actual_postion = 0;
static int position_limit = 0;
char  step5_status = M5_IDLE;
uint16_t last_adc_value = 0;
uint16_t adc_value = 0;

#define M5_PWM_PERIOD  20000
#define SystemCoreClock 72000000
uint16_t u16M5PWMPulsePer1 = 0;
//uint16_t u16TenStep[10][2];
uint16_t u16M5Phase_ON[10];
uint16_t u16M5Phase_OFF[10];


void M5_TIM1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint16_t u16M5PWMArr = 0;

	/* System Clocks Configuration */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
	
  /* TIM1 Configuration ---------------------------------------------------
   Generate 7 PWM signals with 4 different duty cycles:
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
   and Connectivity line devices and to 24 MHz for Low-Density Value line and
   Medium-Density Value line devices
   
   The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / (17570 * (psc+1))) - 1
   The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
  ----------------------------------------------------------------------- */
  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  //TimerPeriod = (SystemCoreClock / 17570 ) - 1;
	u16M5PWMArr = (SystemCoreClock / M5_PWM_PERIOD ) - 1;

	u16M5PWMPulsePer1 = (uint16_t) (((uint32_t) 1 * (u16M5PWMArr - 1)) / 100);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16M5PWMArr;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 Arr reload Enable */
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); 
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
}

void M5_TIM8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint16_t u16M5PWMArr = 0;

  /* System Clocks Configuration */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);// 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  //使能GPIO外设时钟使能

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  /* TIM8 Configuration ---------------------------------------------------
   Generate 7 PWM signals with 4 different duty cycles:
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
   and Connectivity line devices and to 24 MHz for Low-Density Value line and
   Medium-Density Value line devices
   
   The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
   The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
  ----------------------------------------------------------------------- */

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16M5PWMArr;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM8, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OC2Init(TIM8, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = u16M5PWMPulsePer1*50;
  TIM_OC3Init(TIM8, &TIM_OCInitStructure);

	/* TIM1 Arr reload Enable */
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	//TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM8, ENABLE);
	
}

void M5_TIM6_Config(uint32_t period)
{
  TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
  uint16_t u16M5PWMArr = 0;

  /* System Clocks Configuration */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  u16M5PWMArr = (SystemCoreClock / period ) - 1;
	
	/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 99;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = u16M5PWMArr;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM IT enable */
  TIM_ITConfig(TIM6, TIM_IT_Update | TIM_IT_Trigger, ENABLE);
  
}

void Tim6_int_init(void){

	NVIC_InitTypeDef NVIC_InitStructure;
		/* Enable the TIM6 for motor0 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3中断 PWM PA6/PA7
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

}

// ms per step 
void STEP5_set_speed(int speed){
//	target_speed = speed;
	//TIM6_Configuration(speed);//25 MStarget_speed*10
	M5_TIM6_Config(speed);
}

void M5_Phase_TurnOff(uint8_t i)
{
	switch(i)
	{
		case 0:
			A_PHASE_H_OFF;
			break;
		case 1:
			B_PHASE_H_OFF;
			break;
		case 2:
			C_PHASE_H_OFF;
			break;
		case 3:
			D_PHASE_H_OFF;
			break;
		case 4:
			E_PHASE_H_OFF;
			break;
		case 5:
			F_PHASE_H_OFF;
			break;
		case 6:
			AN_PHASE_L_OFF;
			break;
		case 7:
			BN_PHASE_L_OFF;
			break;
		case 8:
			CN_PHASE_L_OFF;
			break;
		case 9:
			DN_PHASE_L_OFF;
			break;
		case 10:
			EN_PHASE_L_OFF;
			break;
		case 11:
			FN_PHASE_L_OFF;
			break;
		default:
			break;
		}

}

void M5_SetCompare(int i, uint8_t ui8Per)
{
	uint16_t ui16Pulse = 0;
	if(ui8Per > 100)
		ui8Per = 100;
	ui16Pulse = (uint16_t) (ui8Per * u16M5PWMPulsePer1);
	switch(i)
	{
		case 0 :
			TIM_SetCompare1(TIM1, ui16Pulse);
			break;
		case 1 :
			TIM_SetCompare2(TIM1, ui16Pulse);
			break;
		case 2 :
			TIM_SetCompare3(TIM1, ui16Pulse);
			break;
		case 3 :
			TIM_SetCompare2(TIM8, ui16Pulse);
			break;
		case 4 :
			TIM_SetCompare3(TIM8, ui16Pulse);
			break;
		case 5 :
			TIM_SetCompare1(TIM8, ui16Pulse);
			break;
		default:
			break;
	}
}

void M5_Phase_TurnOn(uint8_t i)
{
	switch(i)
	{
		case 0:
			A_PHASE_H_ON;
			break;
		case 1:
			B_PHASE_H_ON;
			break;
		case 2:
			C_PHASE_H_ON;
			break;
		case 3:
			D_PHASE_H_ON;
			break;
		case 4:
			E_PHASE_H_ON;
			break;
		case 5:
			F_PHASE_H_ON;
			break;
		case 6:
			AN_PHASE_L_ON;
			break;
		case 7:
			BN_PHASE_L_ON;
			break;
		case 8:
			CN_PHASE_L_ON;
			break;
		case 9:
			DN_PHASE_L_ON;
			break;
		case 10:
			EN_PHASE_L_ON;
			break;
		case 11:
			FN_PHASE_L_ON;
			break;
		default:
			break;
	}
}

void M5_StateStep(void)
{
	uint16_t u16TurnOff = 0, u16TurnOn = 0;
	uint8_t i = 0;
	
	TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);

	TIM_SelectOCxM(TIM8, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_3, TIM_OCMode_PWM1);	
	
	u16TurnOff = u16M5Phase_OFF[step5_phase];
	u16TurnOn = u16M5Phase_ON[step5_phase];
	
	for(i = 0; i < 12; i++)
	{
		if( 1 == ((u16TurnOff & (1 << i)) >> i) )
		{
			M5_Phase_TurnOff(i);
		}
	}
	
	for(i = 0; i < 6; i++)
	{
		M5_SetCompare(i, 50);
	}
	
	for(i = 0; i < 12; i++)
	{
		if( 1 == ((u16TurnOn & (1 << i)) >> i) )
		{
			M5_Phase_TurnOn(i);
		}
	}
}


void test_step10_test2(){
	if(clock_wise){
		step5_phase = (step5_phase + 1) % MAX_PHASE;
		}
	else{
		step5_phase = (step5_phase + MAX_PHASE -1)%MAX_PHASE;
		}
		STEP5_CLK_TOGGLE();

	TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);

	TIM_SelectOCxM(TIM8, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_3, TIM_OCMode_PWM1);	

	switch(step5_phase){
		case 0:
			AN_PHASE_L_OFF;
			A_PHASE_OUT_PWM;
			A_PHASE_H_ON;
			
			BN_PHASE_L_OFF;
			B_PHASE_OUT_PWM;
			B_PHASE_H_ON;
			
			C_PHASE_H_OFF;
			C_PHASE_OUT_PWM;
			CN_PHASE_L_ON;
			
			D_PHASE_H_OFF;
			D_PHASE_OUT_L;
			DN_PHASE_L_ON;
			
			E_PHASE_H_OFF;
			E_PHASE_OUT_PWM;
			EN_PHASE_L_ON;
			break;
		case 1:
			AN_PHASE_L_OFF;
			A_PHASE_OUT_L;
			A_PHASE_H_ON;
			
			BN_PHASE_L_OFF;
			B_PHASE_OUT_PWM;
			B_PHASE_H_ON;
			
			C_PHASE_H_OFF;
			C_PHASE_OUT_PWM;
			CN_PHASE_L_ON;
			
			D_PHASE_H_OFF;
			D_PHASE_OUT_PWM;
			DN_PHASE_L_ON;
			
			EN_PHASE_L_OFF;
			E_PHASE_OUT_PWM;
			E_PHASE_H_ON;
			break;
		case 2:
			AN_PHASE_L_OFF;
			A_PHASE_OUT_PWM;
			A_PHASE_H_ON;
			
			B_PHASE_H_OFF;
			B_PHASE_OUT_PWM;
			BN_PHASE_L_ON;

			C_PHASE_H_OFF;
			C_PHASE_OUT_L;
			CN_PHASE_L_ON;
			
			D_PHASE_H_OFF;
			D_PHASE_OUT_PWM;
			DN_PHASE_L_ON;
			
			EN_PHASE_L_OFF;
			E_PHASE_OUT_PWM;
			E_PHASE_H_ON;
			break;
		case 3:
			AN_PHASE_L_OFF;
			A_PHASE_OUT_PWM;
			A_PHASE_H_ON;
			
			B_PHASE_H_OFF;
			B_PHASE_OUT_PWM;
			BN_PHASE_L_ON;
			
			C_PHASE_H_OFF;
			C_PHASE_OUT_PWM;
			CN_PHASE_L_ON;
			
			DN_PHASE_L_OFF;
			D_PHASE_OUT_PWM;
			D_PHASE_H_ON;
			
			EN_PHASE_L_OFF;
			E_PHASE_OUT_L;
			E_PHASE_H_ON;
			break;
		case 4:
			A_PHASE_H_OFF;
			A_PHASE_OUT_PWM;
			AN_PHASE_L_ON;

			B_PHASE_H_OFF;
			B_PHASE_OUT_L;
			BN_PHASE_L_ON;
			
			C_PHASE_H_OFF;
			C_PHASE_OUT_PWM;
			CN_PHASE_L_ON;
			
			DN_PHASE_L_OFF;
			D_PHASE_OUT_PWM;
			D_PHASE_H_ON;
			
			EN_PHASE_L_OFF;
			E_PHASE_OUT_PWM;
			E_PHASE_H_ON;
			break;
		case 5:
			A_PHASE_H_OFF;
			A_PHASE_OUT_PWM;
			AN_PHASE_L_ON;
			
			B_PHASE_H_OFF;
			B_PHASE_OUT_PWM;
			BN_PHASE_L_ON;
			
			CN_PHASE_L_OFF;
			C_PHASE_OUT_PWM;
			C_PHASE_H_ON;
			
			DN_PHASE_L_OFF;
			D_PHASE_OUT_L;
			D_PHASE_H_ON;
			
			EN_PHASE_L_OFF;
			E_PHASE_OUT_PWM;
			E_PHASE_H_ON;
			break;
		case 6:
			A_PHASE_H_OFF;
			A_PHASE_OUT_L;
			AN_PHASE_L_ON;
			
			B_PHASE_H_OFF;
			B_PHASE_OUT_PWM;
			BN_PHASE_L_ON;
			
			CN_PHASE_L_OFF;
			C_PHASE_OUT_PWM;
			C_PHASE_H_ON;
			
			
			DN_PHASE_L_OFF;
			D_PHASE_OUT_PWM;
			D_PHASE_H_ON;
			
			E_PHASE_H_OFF;
			E_PHASE_OUT_PWM;
			EN_PHASE_L_ON;
			break;
		case 7:
			A_PHASE_H_OFF;
			A_PHASE_OUT_PWM;
			AN_PHASE_L_ON;
			
			BN_PHASE_L_OFF;
			B_PHASE_OUT_PWM;
			B_PHASE_H_ON;
			CN_PHASE_L_OFF;
			C_PHASE_OUT_L;
			C_PHASE_H_ON;
			
			DN_PHASE_L_OFF;
			D_PHASE_OUT_PWM;
			D_PHASE_H_ON;
			
			E_PHASE_H_OFF;
			E_PHASE_OUT_PWM;
			EN_PHASE_L_ON;
			break;
		case 8:
			A_PHASE_H_OFF;
			A_PHASE_OUT_PWM;
			AN_PHASE_L_ON;
			
			BN_PHASE_L_OFF;
			B_PHASE_OUT_PWM;
			B_PHASE_H_ON;
			
			CN_PHASE_L_OFF;
			C_PHASE_OUT_PWM;
			C_PHASE_H_ON;
			
			D_PHASE_H_OFF;
			D_PHASE_OUT_PWM;
			DN_PHASE_L_ON;
			
			E_PHASE_H_OFF;
			E_PHASE_OUT_L;
			EN_PHASE_L_ON;
			break;
		case 9:
			AN_PHASE_L_OFF;
			A_PHASE_OUT_PWM;
			A_PHASE_H_ON;
			
			BN_PHASE_L_OFF;
			B_PHASE_OUT_L;
			B_PHASE_H_ON;
			
			CN_PHASE_L_OFF;
			C_PHASE_OUT_PWM;
			C_PHASE_H_ON;
			
			D_PHASE_H_OFF;
			D_PHASE_OUT_PWM;
			DN_PHASE_L_ON;
			
			E_PHASE_H_OFF;
			E_PHASE_OUT_PWM;
			EN_PHASE_L_ON;
			break;
		default:
			break;
		}

}

//static bool test_int = FALSE;
void STEP5_motor_phase_INT(void){
		test_step10_test2();
}

void STEP5_pwm_enable(){

	AN_PHASE_L_ON;
	BN_PHASE_L_ON;
	CN_PHASE_L_ON;
	DN_PHASE_L_ON;
	EN_PHASE_L_ON;
	A_PHASE_H_ON;
	B_PHASE_H_ON;
	C_PHASE_H_ON;
	D_PHASE_H_ON;
	E_PHASE_H_ON;

//TEST
	F_PHASE_H_ON;
	FN_PHASE_L_ON;
}

void STEP5_pwm_disable(){
	AN_PHASE_L_OFF;
	BN_PHASE_L_OFF;
	CN_PHASE_L_OFF;
	DN_PHASE_L_OFF;
	EN_PHASE_L_OFF;
	A_PHASE_H_OFF;
	B_PHASE_H_OFF;
	C_PHASE_H_OFF;
	D_PHASE_H_OFF;
	E_PHASE_H_OFF;
}

void STEP5_pwm_set_fre(int fre){
	pwm_fre = fre;
	//TIM1_PWM_Init(399,8);	
	//TIM8_PWM_Init(399,8);	//clay test
	M5_TIM1_Init();
	M5_TIM8_Init();
}

void STEP5_pwm_current(char percent){
	//if(percent < 95)
		{
			motor_current = pwm_fre -pwm_fre*(percent)/100 ;
			//motor_current = 200;//clay test
		}
}

		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//GPIO_Pin_1->0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 

void Step5_CLK_INT(){
	if(CLK_STATU == 0){
		//printf("\n\r clk failling ");
		//STEP5_motor_phase_INT();
		}
	else{
		//printf("\n\r clk raising ");
		STEP5_motor_phase_INT();
		}
}
void Step5_DIR_INT(){
	if(DIR_STATU == 0){
		//printf("\n\r DIR failling ");
		clock_wise = FALSE;
		}
	else{
		//printf("\n\r DIR raising ");
		clock_wise = TRUE;
		}
}
void Step5_STOP_INT(){
	if(STOP_STATU == 0){
		//printf("\n\r stop failling ");
		STEP5_stop_run();
		run_enable = FALSE;
		}
	else{
		//printf("\n\r STOp raising ");
		run_enable = TRUE;
		STEP5_start_run();
		}

}


void STEP5_IO_init(){
		GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE);//|RCC_APB2Periph_AFIO

	GPIO_InitStructure.GPIO_Pin = EN1_PIN|EN2_PIN; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EN1_PORT, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	#if IO_MODE
	GPIO_InitStructure.GPIO_Pin = A_PIN | B_PIN | C_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AN_PIN |BN_PIN |CN_PIN |DN_PIN |EN_PHASE_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = D_PIN | E_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	A_IO_L();
	AN_IO_L();
	B_IO_L();
	BN_IO_L();
	C_IO_L();
	CN_IO_L();
	D_IO_L();
	DN_IO_L();
	E_IO_L();
	EN_IO_L();
	#endif
}

void STEP5_motor_init(void){
	
	STEP5_IO_init();
	Adc_Init();

	Tim6_int_init();// will disable  when use INT mode
	STEP5_set_speed(DEFAULT_SPEED);//25 MStarget_speed*10 // will disable  when use INT mode

	STEP5_pwm_set_fre(PWM_DEFAULT_FRE);
	STEP5_pwm_current(DEFAULT_CURRENT);
	//STEP5_io_all_low();
	step5_status = M5_INITED;
	STEP5_pwm_disable();
}


void STEP5_stop_run(void){
	//STEP5_io_all_low();
	#if STEP5_DEBUG
	TIM_Cmd(TIM6, DISABLE); // will disable  when use INT mode
	#endif
}

void STEP5_start_run(void){
	STEP5_pwm_enable();
	#if STEP5_DEBUG
	TIM_Cmd(TIM6, ENABLE); // will disable  when use INT mode
	#endif
	step5_status = M5_RUN;
	EN1_ENABLE();
	EN2_ENABLE();
}


 void STEP5_set_clockwise(bool wise){
	clock_wise = wise;
}


void STEP5_set_taget_position(int steps){
	target_step =  steps ;
 	}

 void STEP5_reset_position_counter(){
	step_counter = 0 ;
 	}

void STEP5_set_position_limit(int limit){
	position_limit = limit;
}

char STEP5_get_status(void){
	return step5_status;
}

void STEP5_set_target_speed( int speed){
	target_speed = speed;
	STEP5_set_speed(target_speed);
}

void STEP5_set_start_speed( int speed){
	start_speed = speed;
}

void STEP5_set_acc(int acc){ 
	step5_acc =acc;
}


void STEP5_loop(){
	adc_value = Get_Adc_Average(ADC_Channel_0,10);//ADC_Channel_1
	if((adc_value > (last_adc_value +50))||(adc_value < (last_adc_value - 50))){
			
			#if DEAD_AREA
			//0~4000 ---mapping 15~87
			last_adc_value = adc_value /40 ;
			if((last_adc_value > 15) && (last_adc_value < 87))
				STEP5_pwm_current(last_adc_value/ 40);
			#else
			// 4~97 %
			last_adc_value = adc_value /40 ;
			if((last_adc_value > 3) && (last_adc_value < 98))
				STEP5_pwm_current(last_adc_value);
			#endif
			last_adc_value = adc_value;
		}
	delay_ms(2);
}
