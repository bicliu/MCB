#ifndef _MOTOR_Z5_H_
#define _MOTOR_Z5_H_
#include "stm32f10x.h"

#define M5_Z5           5
#define M5_IO_NUMBER    10

void M5_Configure(void);
void M5_TIM_IT_Handler(void);

void M5_Start(void);
void M5_Stop(void);

#endif
