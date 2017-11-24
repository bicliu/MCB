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

void TIM8_PWM_Init(u16 arr,u16 psc)
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC,&GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
GPIO_Init(GPIOB,&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_CenterAligned3;//TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
		TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;//TIM_OCNPolarity_Low;//TIM_OCNPolarity_High
		TIM_OCInitStructure.TIM_OutputNState= TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//TIM_OCIdleState_Reset
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;//TIM_OCNIdleState_Reset //fail to set high

	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx



  TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 

	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM1
   
}

void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA,&GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_CenterAligned3;//TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
		TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;//TIM_OCNPolarity_High;//TIM_OCNPolarity_Low; //TIM_OCPolarity_Low TIM_OCPolarity_High
		TIM_OCInitStructure.TIM_OutputNState= TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//TIM_OCIdleState_Reset
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;//TIM_OCNIdleState_Reset //fail to set high

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);


  TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1   
}


void TIM4_Configuration(void)
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//????PWM??=72000000/900=80Khz
	/* ---------------------------------------------------------------
	TIM4CLK ?PCLK1=36MHz
	TIM4 Configuration: generate 1 PWM signals :
    TIM4CLK = 36 MHz, Prescaler = 0x0, TIM4 counter clock = 36 MHz
    TIM4 ARR Register = 900 => TIM4 Frequency = TIM4 counter clock/(ARR + 1)
    TIM4 Frequency = 36 KHz.
    TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR)* 100 
	TIM4CLK = 36 MHz, Prescaler = 0, TIM4 counter clock = 36MHz
	--------------------------------------------------------------- */
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 899; //???????????????????????????	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =0; //??????TIMx???????????  ???
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //??????:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM??????
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //??TIM_TimeBaseInitStruct?????????TIMx???????
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //???????:TIM????????2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //??????
	TIM_OCInitStructure.TIM_Pulse = 0; //????????????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //????:TIM???????
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//clay TIM_OCIdleState_Reset
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //??TIM_OCInitStruct???????????TIMx
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //??TIM_OCInitStruct???????????TIMx
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //??TIM_OCInitStruct???????????TIMx
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //??TIM_OCInitStruct???????????TIMx
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //??TIMx?CCR2????????
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //??TIMx?ARR????????
	
	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);  //??TIMx??
	}


void TIM6_Configuration(uint16_t interval)
	{
		TIM_TimeBaseInitTypeDef  TIM6_TimeBaseStructure;
//		TIM_OCInitTypeDef  TIM_OCInitStructure;
		uint16_t peroid,scaler;
	/* TIM6 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
//					peroid = interval ;
//		bak_peroid = peroid;
//	scaler = 0;
		//--FOR TIMER INT------
	if ((interval > 999)){ // > 100ms
			scaler = 7200;//ms
			peroid = interval / 100 ;
		}
		else if ((interval > 99)&&(interval < 1000)){ // 100ms~10ms
			scaler = 720; //100us
			peroid = interval /10;
		}
		else { //< 1000 us
			scaler = 72;//10us
			peroid = interval ;
		}


	/* ---------------------------------------------------------------
	TIM6CLK 即PCLK1=36MHz
	TIM6CLK = 36 MHz, Prescaler = 7200, TIM6 counter clock = 5K,即改变一次为5K,周期就为10K
	--------------------------------------------------------------- */
	/* Time base configuration */
	TIM6_TimeBaseStructure.TIM_Period = peroid;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM6_TimeBaseStructure.TIM_Prescaler =(scaler-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM6_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM6_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM6_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	/* Enables the Update event for TIM6 */
	//TIM_UpdateDisableConfig(TIM6,ENABLE); 	//使能 TIM6 更新事件 
	
	/* TIM IT enable */
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM6, //TIM6
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
//	
	/* TIM6 enable counter */
	//TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
//	
//	TIM6_TimeBaseStructure.TIM_Period = 2000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
//	TIM6_TimeBaseStructure.TIM_Prescaler =(1); //设置用来作为TIMx时钟频率除数的预分频值  不分频
//	TIM6_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
//	TIM6_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM6, &TIM6_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//	
//	/* Output Compare Active Mode configuration: Channel1 */
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2 TIM_OCMode_PWM2 for ch2
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//	TIM_OC1Init(TIM6, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
//	//TIM_OC3Init(TIM6, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
//	TIM_OC1PreloadConfig(TIM6, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器
//	//TIM_OC3PreloadConfig(TIM6,TIM_OCPreload_Enable);
//		
//	TIM_ARRPreloadConfig(TIM6, ENABLE); //使能TIMx在ARR上的预装载寄存器
//	
	
	
//	TIM_SetCompare1(TIM6,1000);
	/* TIM6 enable counter */
//	TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
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
	TIM6_Configuration(speed);//25 MStarget_speed*10
}

bool is_able_to_run(){
	return TRUE ;//clay test
	if(step_counter >= target_step){
		step5_status = M5_ARRIVED_STOP;
		return FALSE;
		}
	// position limit

	if(clock_wise){
			if( actual_postion > position_limit){
				step5_status = M5_LIMIT_STOP;
				return FALSE;
				}
			else
				actual_postion ++;
		}
	else{
			if(actual_postion <= 0 ){
				step5_status = M5_LIMIT_STOP;
				return FALSE;
				}
			else 
				actual_postion --;
		}
	
	step_counter++;
	return TRUE ;
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

	#if 1//TIM8_PWM
	TIM_SelectOCxM(TIM8, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_3, TIM_OCMode_PWM1);	
	#endif

#if 0//STEP5_DEBUG
	FN_PHASE_L_ON;
	F_PHASE_OUT_PWM;
	F_PHASE_H_ON;
#endif

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

void test_step10(){
	if(clock_wise){
		step5_phase = (step5_phase + 1) % 10;
		}
	else{
		step5_phase = (step5_phase + 10 -1)%10;
		}
		STEP5_CLK_TOGGLE();

	TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);

	#if 1//TIM8_PWM
	TIM_SelectOCxM(TIM8, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_3, TIM_OCMode_PWM1);	
	#endif


	switch(step5_phase){
		case 0:
			
			//A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			//CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			A_PHASE_OUT_L;
			C_PHASE_OUT_L;
			A_PHASE_H_ON;
			CN_PHASE_L_ON;

			//A_PHASE_OUT_H;
			//C_PHASE_OUT_H;

			break;
		case 1:

			//A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			//B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			//CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			//DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			A_PHASE_OUT_PWM;
			B_PHASE_OUT_PWM;
			C_PHASE_OUT_PWM;
			D_PHASE_OUT_PWM;
			A_PHASE_H_ON;
			CN_PHASE_L_ON;
			B_PHASE_H_ON;
			DN_PHASE_L_ON;
			break;
		case 2:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			//B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			//DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			B_PHASE_OUT_L;
			D_PHASE_OUT_L;
			B_PHASE_H_ON;
			DN_PHASE_L_ON;
	
			break;
		case 3:	
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			//B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			//C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			//DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			//EN_PHASE_L_OFF;

			B_PHASE_OUT_PWM;
			C_PHASE_OUT_PWM;
			D_PHASE_OUT_PWM;
			E_PHASE_OUT_PWM;
			B_PHASE_H_ON;
			C_PHASE_H_ON;
			DN_PHASE_L_ON;
			EN_PHASE_L_ON;
			break;
		case 4:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			//C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			//EN_PHASE_L_OFF;

			C_PHASE_OUT_L;
			E_PHASE_OUT_L;
			C_PHASE_H_ON;
			EN_PHASE_L_ON;
			break;
		case 5:
			A_PHASE_H_OFF;
			//AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			//C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			//D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			//EN_PHASE_L_OFF;

			A_PHASE_OUT_PWM;
			C_PHASE_OUT_PWM;
			D_PHASE_OUT_PWM;
			E_PHASE_OUT_PWM;
			AN_PHASE_L_ON;
			C_PHASE_H_ON;
			D_PHASE_H_ON;
			EN_PHASE_L_ON;
			break;
		case 6:
			A_PHASE_H_OFF;
			//AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			//D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			A_PHASE_OUT_L;
			D_PHASE_OUT_L;
			D_PHASE_H_ON;
			AN_PHASE_L_ON;
			break;
		case 7:
			A_PHASE_H_OFF;
			//AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			//BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			//D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			//E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			A_PHASE_OUT_PWM;
			B_PHASE_OUT_PWM;
			D_PHASE_OUT_PWM;
			E_PHASE_OUT_PWM;
			AN_PHASE_L_ON;
			E_PHASE_H_ON;
			D_PHASE_H_ON;
			BN_PHASE_L_ON;
			break;
		case 8:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			//BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			//E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			B_PHASE_OUT_L;
			E_PHASE_OUT_L;
			E_PHASE_H_ON;
			BN_PHASE_L_ON;
			break;
		case 9:
			//A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			//BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			//CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			//E_PHASE_H_OFF;
			EN_PHASE_L_OFF;

			A_PHASE_OUT_PWM;
			B_PHASE_OUT_PWM;
			C_PHASE_OUT_PWM;
			E_PHASE_OUT_PWM;
			BN_PHASE_L_ON;
			E_PHASE_H_ON;
			A_PHASE_H_ON;
			CN_PHASE_L_ON;
			break;
		default:
			break;
		}
/*
			delay_ms(20);
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			*/
}

void test_step(){
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

	#if 1//TIM8_PWM
	TIM_SelectOCxM(TIM8, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM(TIM8, TIM_Channel_3, TIM_OCMode_PWM1);	
	#endif


	switch(step5_phase){
		case 0:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			delay_ms(1);
			A_PHASE_H_ON;
			//AN_PHASE_L_ON;
			//B_PHASE_H_ON;
			BN_PHASE_L_ON;
			//C_PHASE_H_ON;
			//CN_PHASE_L_ON;
			//D_PHASE_H_ON;
			//DN_PHASE_L_ON;
			//E_PHASE_H_ON;
			//EN_PHASE_L_ON;		
			break;
		case 1:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			delay_ms(1);
			//A_PHASE_H_ON;
			//AN_PHASE_L_ON;
			B_PHASE_H_ON;
			//BN_PHASE_L_ON;
			//C_PHASE_H_ON;
			CN_PHASE_L_ON;
			//D_PHASE_H_ON;
			//DN_PHASE_L_ON;
			//E_PHASE_H_ON;
			//EN_PHASE_L_ON;		
			break;
		case 2:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			delay_ms(1);
			//A_PHASE_H_ON;
			//AN_PHASE_L_ON;
			//B_PHASE_H_ON;
			//BN_PHASE_L_ON;
			C_PHASE_H_ON;
			//CN_PHASE_L_ON;
			//D_PHASE_H_ON;
			DN_PHASE_L_ON;
			//E_PHASE_H_ON;
			//EN_PHASE_L_ON;		
			break;
		case 3:	
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			delay_ms(1);
			//A_PHASE_H_ON;
			//AN_PHASE_L_ON;
			//B_PHASE_H_ON;
			//BN_PHASE_L_ON;
			//C_PHASE_H_ON;
			//CN_PHASE_L_ON;
			D_PHASE_H_ON;
			//DN_PHASE_L_ON;
			//E_PHASE_H_ON;
			EN_PHASE_L_ON;		
			break;
		case 4:
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			delay_ms(1);
			//A_PHASE_H_ON;
			AN_PHASE_L_ON;
			//B_PHASE_H_ON;
			//BN_PHASE_L_ON;
			//C_PHASE_H_ON;
			//CN_PHASE_L_ON;
			//D_PHASE_H_ON;
			//DN_PHASE_L_ON;
			E_PHASE_H_ON;
			//EN_PHASE_L_ON;		
			break;
		default:
			break;
		}
/*
			delay_ms(20);
			A_PHASE_H_OFF;
			AN_PHASE_L_OFF;
			B_PHASE_H_OFF;
			BN_PHASE_L_OFF;
			C_PHASE_H_OFF;
			CN_PHASE_L_OFF;
			D_PHASE_H_OFF;
			DN_PHASE_L_OFF;
			E_PHASE_H_OFF;
			EN_PHASE_L_OFF;
			*/
}

//static bool test_int = FALSE;
void STEP5_motor_phase_INT(void){
		test_step10_test2();
}

void STEP5_io_all_low(){
	A_PHASE_OUT_L;
	B_PHASE_OUT_L;
	C_PHASE_OUT_L;
	D_PHASE_OUT_L;
	E_PHASE_OUT_L;
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
	//TIM1_PWM_Init(pwm_fre,0);	
	//TIM8_PWM_Init(pwm_fre,0);	
	TIM1_PWM_Init(399,8);	
	TIM8_PWM_Init(399,8);	//clay test
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

//外部中断初始化函数
void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟


    //GPIOb.5--clk 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    //GPIOb.8  dir 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    //GPIOB.9	stop  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	/*	
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
 
 
   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
	*/
 
}

void STEP5_INT_IO(){

		
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTC时钟

	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = CLK_PIN;//PA15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(CLK_PORT, &GPIO_InitStructure);//初始化GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = DIR_PIN;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(DIR_PORT, &GPIO_InitStructure);//初始化GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = STOP_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(STOP_PORT, &GPIO_InitStructure);//初始化GPIOA.0
	
	EXTIX_Init();
}

void STEP5_IO_init(){
		GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE);//|RCC_APB2Periph_AFIO

	#if 0//STEP5_DEBUG
	//for test only
	GPIO_InitStructure.GPIO_Pin = CLK5_PIN; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CLK5_PORT, &GPIO_InitStructure);
	STEP5_CLK_L();//default for rasing edge  	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //test
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif


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
	#if (STEP5_DEBUG == 0)
	STEP5_INT_IO(); //clay disable
	#endif
#if IO_MODE

#else
	#if STEP5_DEBUG
	Tim6_int_init();// will disable  when use INT mode
	STEP5_set_speed(DEFAULT_SPEED);//25 MStarget_speed*10 // will disable  when use INT mode
	#endif
	STEP5_pwm_set_fre(PWM_DEFAULT_FRE);
	STEP5_pwm_current(DEFAULT_CURRENT);
	//STEP5_io_all_low();
	step5_status = M5_INITED;
	STEP5_pwm_disable();

#if 0//STEP5_DEBUG
	//test only 
	F_PHASE_H_ON;
	FN_PHASE_L_ON;
	F_PHASE_OUT_PWM;
#endif

//motor_current = pwm_fre+100;
/*
	A_PHASE_OUT_PWM;
	B_PHASE_OUT_PWM;
	C_PHASE_OUT_PWM;
	E_PHASE_OUT_PWM;
	D_PHASE_OUT_PWM;
*/			
/*		
	#if TIM8_PWM
	TIM8_PWM_Init(pwm_fre,0); //80k --799, 20k 
	//TIM_SetCompare1(TIM8,900);	
//	TIM_SetCompare2(TIM8,900);	
	//TIM_SetCompare3(TIM8,900);	
//	D_PHASE_OUT_L;
//	E_PHASE_OUT_L;
	
	#else
	TIM4_Configuration();
	TIM_SetCompare1(TIM4,100);	
	TIM_SetCompare2(TIM4,100);	
	TIM_SetCompare3(TIM4,100);	
	TIM_SetCompare4(TIM4,100);	
	#endif


	TIM1_PWM_Init(pwm_fre,0);//
//	A_PHASE_OUT_L;
//	B_PHASE_OUT_L;
//	C_PHASE_OUT_L;



	#if 0
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
	//#else
	// out put enable
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
	
	#endif
	*/
	//STEP5_io_all_low();
//	pwm_duty_gap = pwm_fre/(DUTY_STEP*2);
//	motor_current = pwm_fre -200 ;// pwm_fre/4 = 100 -25% ;//
	//TIM_Cmd(TIM6, ENABLE); 
#endif//IO_MODE
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
	//delay_ms(200);
	/*
	Step5_STOP_INT();
	Step5_DIR_INT();
	Step5_CLK_INT();
	TEST5_CLK_TOGGLE();
	*/
	//TEST5_CLK_TOGGLE();
	delay_ms(2);
}
