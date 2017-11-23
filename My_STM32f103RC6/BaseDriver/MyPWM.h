/**
  ******************************************************************************
  * @file    FourPWM.h
  * @author  Bic
  * @version V1.0.0
  * @date    17-October-2017
  * @brief   This file contains all the functions prototypes for the Usart.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MYPWM_H_
#define _MYPWM_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
	 
typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}MyPWM_GPIOTypeDef;

typedef struct{
	MyPWM_GPIOTypeDef tClkWiseIO;
	MyPWM_GPIOTypeDef tEnableIO;
}tMyPWM_PWMCtl_t;

typedef struct{
	uint32_t     timerRcc;
	
}MyPWM_InitTypeDef;
	 
void MyPWM_TIM_Init(void);
	 
void MyPWM_TIM1_Run(void);
void MyPWM_TIM1_Stop(void);
void MyPWM_TIM1_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle);

void MyPWM_TIM2_Run(void);
void MyPWM_TIM2_Stop(void);
void MyPWM_TIM2_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle);

void MyPWM_TIM3_Run(void);
void MyPWM_TIM3_Stop(void);
void MyPWM_TIM3_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle);
	 
void MyPWM_TIM4_Run(void);
void MyPWM_TIM4_Stop(void);
void MyPWM_TIM4_SetSpeed(uint32_t uiPeriod, uint32_t uiDutyCycle);

void MyPWM_TIM_IOEnable(int index);
void MyPWM_TIM_IODisable(int index);
void MyPWM_TIM_ClkWise(int index);
void MyPWM_TIM_AntiClkWise(int index);

#ifdef __cplusplus
}
#endif
#endif
