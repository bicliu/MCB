/**********************************************
File Name: ALIENTEK MINISTM32 实验7 定时器中断实验(库)
Revision:
Corporation:
Microcontroller Corporation:ST
Microcontroller name:STM32F103RBT6 ARM
Compiler: Keil MDK_ARM 4.11
Author: moweidong
E-mail: moweidongsabbit@21cn.com
Date : 2010.12.05
*********************************************/
//仿真仪器设备:
//ALIENTEK_MiniSTM32_REVB0开发板
//JLINK V7
/*********************************************
//库
//CMSIS:V1.20
//STM32F10x_StdPeriph_Driver:V3.1.1
*********************************************/

//本程序使用了printf用于在LCD和串口显示数据,因些对Option for Target属性有如下更改
//Target选项页:勾中Use MicroLIB(使用微库)

//LED0:PA8
//LED1:PD2	TIM3_ETR

//注意:在禁用JTAG接后后可这样恢复:
//1.设置KEIL为JTAG或SW下载模式
//2.给板上电,按下板上的复位按键不放,点击KEIL下的FLASH下载按钮,使JTAG或SW下载工具检测完成接口后立刻放开,
//此目白在于争取IC在上电的时候有控制权,使在未进入禁用JTAG或SW生效的时候取可控制IC进行烧录

#include "stm32f10x.h"
//#include "GLCD.h"
//#include "USART.h"
#include "eval.h"
#include "SysTickDelay.h"
#include "UART_INTERFACE.h"
#include <stdio.h>
#include "motor_servo.h"
#include "motor_step.h"
#include "motor_step5.h"
#include "msg_handler.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
//clay mask TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void TIM3_Configuration(void);




void GPIO_Configuration(void)
	{
	//GPIO_InitTypeDef GPIO_InitStructure;
				
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 Rx (PA.10) as input floating */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);#ifdef USE_STM3210C_EVAL

	/* Initialize LEDs and Key Button mounted on STM3210X-EVAL board */       
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	}

//系统中断管理
void NVIC_Configuration(void)
	{ 
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	//设置优先级分组：先占优先级0位,从优先级4位
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置优先级分组：先占优先级2位,从优先级2位
	
	//设置向量表的位置和偏移
	#ifdef  VECT_TAB_RAM  
		/* Set the Vector Table base location at 0x20000000 */ 
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 		//向量表位于RAM
	#else  /* VECT_TAB_FLASH  */
		/* Set the Vector Table base location at 0x08000000 */ 
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   //向量表位于FLASH
	#endif

//	/* Enable the TIM3 for motor0 global Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	
		/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//USART1中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
	
	}

//配置系统时钟,使能各外设时钟
void RCC_Configuration(void)
	{
	SystemInit();	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
	//	|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
	//	|RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
	//	|RCC_APB2Periph_ADC1  | RCC_APB2Periph_AFIO 
	//	|RCC_APB2Periph_SPI1, ENABLE );
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL ,ENABLE );
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 
	//	|RCC_APB1Periph_USART3|RCC_APB1Periph_TIM2	                           
	//	, ENABLE );
	}
	




//配置所有外设
void Init_All_Periph(void)
	{
	
	RCC_Configuration();	
	
	NVIC_Configuration();

	GPIO_Configuration();
	
	//USART1_Configuration();
	USART_Configuration(9600);//115200 9600
	USART_RX_IntteruptEnable(USED_COM_NUMBER);		//接收中断使能
	
	}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
//void Delay(__IO uint32_t nCount)
//	{
//	for(; nCount != 0; nCount--);
//	}

	u8 KEY_Scan(void)
	{	 
	static u8 key_up=1;//按键按松开标志	
	if(key_up&&((STM_EVAL_PBGetState(Button_KEY1) == 0x00)||(STM_EVAL_PBGetState(Button_WAKEUP) == 0x01)))
		{
		delay_ms(10);//去抖动 
		key_up=0;

		if (STM_EVAL_PBGetState(Button_KEY1) == 0x00)		//按键按下:低电平有效
			{
			return 2;
			}
		if (STM_EVAL_PBGetState(Button_WAKEUP) == 0x01)		//按键按下:高电平有效
			{
			return 3;
			}
		}
		else if((STM_EVAL_PBGetState(Button_KEY1) == 0x01)&&(STM_EVAL_PBGetState(Button_WAKEUP) == 0x00)) key_up=1; 	    
	return 0;// 无按键按下
	}


volatile int duty = 20;//ms
volatile int step_delay = 10;//ms

int main(void)
	{  
		int counter = 0;
 
//		u8 Key_Vlaue; 
//	u8 len=0;	
//	u16 times=0; 
		
	Init_All_Periph();
	SysTick_Initaize();
	STM_EVAL_LEDOff(LED1);	 //熄灭LED0
	STM_EVAL_LEDOff(LED2);	 //熄灭LED0

#if 0

		SERVO_M_init();
		SERVO_M_set_clockwise(FALSE);
		SERVO_M_set_step_interval(100); //us
		SERVO_M_start();
//		
		STEP_M_init();
		STEP_M_set_excitation(3);//div 8 --24000
		STEP_M_set_clock(SLOW); //speed
		//STEP_M_set_peroid(250);//just for hw pwm
		STEP_M_reset_counter();
		STEP_M_set_plus_num_per_circle(800*20);// uint_16 65536
		STEP_M_set_target_position(20, 30);
		
		STEP_MOT_set_clockwise(TRUE);
		STEP_M_DECAY(0);
		STEP_M_set_enable(TRUE);
		STEP_M_start_run();
//		//read id from eeprom
		printf("\nREPORT#ID@%d#STATUS@ready\n",BOARD_ID);
	STM_EVAL_PBInit(Button_WAKEUP,Mode_GPIO);
	STM_EVAL_PBInit(Button_KEY1, Mode_GPIO);	
#else
	STEP5_motor_init();
	STEP5_start_run();
	//TIM_Cmd(TIM6, ENABLE); 
	//A_IO_L();
	//AN_IO_H();
	//Adc_Init();
	printf("\n\r test  \n");
#endif	
	while(1)
		{
			//printf("\n uncompleted circle =%d , ",STEP_M_get_uncompleted_circle());
//			delay_ms(5);
			STEP5_loop();
			//
			//counter=Get_Adc_Average(ADC_Channel_0,10);
			//printf("\n\r counter =%d percent = %d",counter,( counter / 40) );
//			counter++ ;
			delay_ms(step_delay);
			//	STM_EVAL_LEDOn(LED2);	 //熄灭LED0
	//STM_EVAL_LEDOff(LED2);	 //熄灭LED0
			if(counter < 50){
				counter++;
				//printf("\n\r counter = %d",counter);
				//STEP5_motor_phase_INT();
				//delay_ms(duty);
				/*A_PHASE_H_OFF;
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
			else{
				//STEP5_stop_run();
			}
			msg_process();
			
			#if 0
						msg_process();
			delay_ms(50);	
			Key_Vlaue=KEY_Scan();//得到键值
			if(Key_Vlaue)
				{						   
				switch(Key_Vlaue)
					{				 
					case 1://for servo motor int
						//STM_EVAL_LEDToggle(LED1);
						break;
					case 2: //KEY 1
						STM_EVAL_LEDToggle(LED1);
						//STEP_M_stop_run();//NO NEED
						if( SLOW - GAP*counter > FAST)
							counter ++ ;
						STEP_M_set_clock(SLOW - GAP*counter);						
						//STEP_M_start_run();
						break;
					case 3:				//wakeup
						//STM_EVAL_LEDToggle(LED1);
						STM_EVAL_LEDToggle(LED2);
						//STEP_M_stop_run();//NO NEED
					  if(counter>0)
							counter --;
						STEP_M_set_clock(SLOW - GAP*counter);					
						//STEP_M_start_run();
						break;
					}
					
					DEBUG("\n counter = %d",counter);
				} 
			
			Key_Vlaue=KEY_Scan();//得到键值
			if(Key_Vlaue)
				{						   
				switch(Key_Vlaue)
					{				 
					case 1://for servo motor int
						STEP5_CLK_L();
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
						STEP5_CLK_H();
						break;
					case 2: //KEY 1
						STEP5_CLK_L();
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
						STEP5_CLK_H();
						break;
					case 3:				//wakeup
						STEP5_CLK_L();
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
						delay_ms(20);
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
						delay_ms(20);
						STEP5_CLK_L();
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
						delay_ms(20);
						TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable); 
						TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);						
						STEP5_CLK_H();
						break;
					}
					
					DEBUG("\n counter = %d",counter);
				} 
			#endif

		}//while
	}//main





#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif





