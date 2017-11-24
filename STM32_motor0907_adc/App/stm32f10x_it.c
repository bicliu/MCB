/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "eval.h"  
//#include "GLCD.h"
//#include <stdio.h>
#include "motor_servo.h"
#include "motor_step.h"
#include "motor_step5.h"
#include "SysTickDelay.h"
#include "msg_handler.h"



/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

//外部中断0服务程序
void EXTI0_IRQHandler(void)
	{
//方法一:
//	delay_ms(10);//消抖
//	if (STM_EVAL_PBGetState(Button_WAKEUP) == 0x01)		//按键按下:高电平有效
//		{
//		STM_EVAL_LEDToggle(LED1);
//		STM_EVAL_LEDToggle(LED2);
//		}		 
//	//EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
//	/* Clear the Key Button EXTI line pending bit */  
//	EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);  //清除EXTI0线路挂起位

//方法二:
//	if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)	  //检查指定的EXTI0线路触发请求发生与否
//		{
//		/* Clear the Key Button EXTI line pending bit */  
//		EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);  //清除EXTI0线路挂起位
//		/* Toggle LED1/2 */
//		STM_EVAL_LEDToggle(LED1);
//		STM_EVAL_LEDToggle(LED2);
//		}
	}

void EXTI9_5_IRQHandler(){
	//STM_EVAL_LEDToggle(LED2);
//	STM_EVAL_LEDToggle(LED1);
	if(EXTI_GetITStatus(CLK_INT_LINE) != RESET){
		Step5_CLK_INT();
		}
	EXTI_ClearITPendingBit(CLK_INT_LINE);
	
	if(EXTI_GetITStatus(DIR_INT_LINE) != RESET){
		Step5_DIR_INT();
		}
	EXTI_ClearITPendingBit(DIR_INT_LINE);
	
	if(EXTI_GetITStatus(STOP_INT_LINE) != RESET){
		Step5_STOP_INT();
		}
	EXTI_ClearITPendingBit(STOP_INT_LINE);
}

//外部中断15~10服务程序
void EXTI15_10_IRQHandler(void)
	{			

	#if 1
	//printf(" \n\r EXTI15_10_IRQHandler ");
	STM_EVAL_LEDToggle(LED2);
//	STM_EVAL_LEDToggle(LED1);
	if(EXTI_GetITStatus(CLK_INT_LINE) != RESET){
		//Step5_CLK_INT();
		}
	EXTI_ClearITPendingBit(CLK_INT_LINE);
	
	if(EXTI_GetITStatus(DIR_INT_LINE) != RESET){
		//Step5_DIR_INT();
		}
	EXTI_ClearITPendingBit(DIR_INT_LINE);
	
	if(EXTI_GetITStatus(STOP_INT_LINE) != RESET){
		//Step5_STOP_INT();
		}
	EXTI_ClearITPendingBit(STOP_INT_LINE);
EXTI_ClearITPendingBit(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9);
	#else
	delay_ms(10);    //消抖			 
	if (STM_EVAL_PBGetState(Button_KEY0) == 0x00)		//按键按下:低电平有效
		{
		//STM_EVAL_LEDToggle(LED2);
			 SERVO_M_fault_out();
		}
	else if (STM_EVAL_PBGetState(Button_KEY1) == 0x00)		//按键按下:低电平有效
		{
		//STM_EVAL_LEDToggle(LED2);
		}
	//EXTI->PR=1<<13;     //清除LINE13上的中断标志位  
	//EXTI->PR=1<<15;     //清除LINE15上的中断标志位  
	/* Clear the Key Button EXTI line pending bit */  
	EXTI_ClearITPendingBit(BUTTON0_EXTI_LINE);  //清除EXTI13线路挂起位
	EXTI_ClearITPendingBit(BUTTON1_EXTI_LINE);  //清除EXTI15线路挂起位
	#endif
	}


/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
	{
	}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
	{
	}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)   //TIM3中断
	{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		/* Pin PD.02 toggling with frequency = 10KHz */
		//GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)));
		// STM_EVAL_LEDToggle(LED2);
			//SERVO_M_run_step(); //no use, for pwm
			//STEP_M_run_step();//STEP_M_CLK_toggle();
		}
	}



void TIM6_IRQHandler(void)   //TIM3中断
	{
	//STM_EVAL_LEDToggle(LED1);
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		/* Pin PD.02 toggling with frequency = 10KHz */
		//STEP_M_CLK_toggle();//step motor use pwm
		//	SERVO_M_run_step();
		STEP5_motor_phase_INT();
		//STM_EVAL_LEDToggle(LED2);
		}
	}
	
/**
  * @brief  This function handles TIM5 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM5_IRQHandler(void)   //TIM3中断
	{
		//STM_EVAL_LEDToggle(LED1);
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		/* Pin PD.02 toggling with frequency = 10KHz */
		//STEP_M_CLK_toggle();//step motor use pwm
		//	SERVO_M_run_step();
		//STEP5_int();
			//STM_EVAL_LEDToggle(LED2);
		}
	}
	
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
//u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
////接收状态
////bit7，接收完成标志
////bit6，接收到0x0d
////bit5~0，接收到的有效字节数目
//u8 USART_RX_STA=0;       //接收状态标记

//注意,读取USARTx->SR能避免莫名其妙的错误 
void USART1_IRQHandler(void)	//串口1中断服务程序
	{
	u8 Res;
	//STM_EVAL_LEDToggle(LED2);
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x80)==0)//接收未完成
			{
			if(USART_RX_STA&0x40)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x80;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x40;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3F]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}  		 
		}
	}


void USART2_IRQHandler(void)	//串口1中断服务程序
	{
	u8 Res;
	//STM_EVAL_LEDToggle(LED2);
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x80)==0)//接收未完成
			{
			if(USART_RX_STA&0x40)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x80;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x40;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3F]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}  		 
		}
	}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
