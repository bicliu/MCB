#ifndef _GPIO_H_
#define _GIPO_H_
#include "stm32f10x.h"

#define LED1       0
#define LED2       1
#define LED3       2
#define LED_NUMBER 3

#define LED_FLASHING_OFF   0
#define LED_FLASHING_ON    1


typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}Gpio_Info;

//extern Gpio_Info led_array[LED_NUMBER];
//#define LED_ON(x)       GPIO_ResetBits(led_array[x].gpio,led_array[x].pin);
//#define LED_OFF(x)      GPIO_SetBits(led_array[x].gpio,led_array[x].pin);
/*LED*/
void LED_ON(int index);
void LED_OFF(int index);
void LEDGpio_Init(void);
void LED_SetFlashing(uint8_t id, uint8_t bit);
void LED_Flashing(void);

/*out io*/
void MyGpio_writeIO_Init(Gpio_Info port);
void MyGpio_SetBits(Gpio_Info port);
void MyGpio_ResetBits(Gpio_Info port);
void MyGpio_WriteBit(Gpio_Info port, uint8_t bit);

/*pwm io*/
void MyGpio_PWMIO_Init(Gpio_Info port);
#endif
