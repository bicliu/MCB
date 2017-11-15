/**
  ******************************************************************************
  * @file    MyPWM_m5.h
  * @author  Bic
  * @version V1.0.0
  * @date    17-October-2017
  * @brief   This file contains all the functions prototypes for the PWM.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYPWM_M5_H
#define __MYPWM_M5_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/*tim1 period 20000 Hz*/
#define PWM_DEFAULT_PERIOD    20000
#define STEP_DEFSULT_PERIOD   20

extern uint16_t CCR_Val;

void M5_TIM1_Init(uint32_t period);
void M5_TIM8_Init(uint32_t period);
void M5_TIM6_Init(uint32_t period);
void M5_Step(void);
void M5_Start(void);

#ifdef __cplusplus
}
#endif
#endif
