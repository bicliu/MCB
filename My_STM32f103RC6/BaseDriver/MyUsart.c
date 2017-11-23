/**
  ******************************************************************************
  * @file    MyUsart.c
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
#include "MyUsart.h"
#include <stdarg.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
	