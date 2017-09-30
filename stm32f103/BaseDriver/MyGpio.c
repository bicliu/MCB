#include "MyGpio.h"


Gpio_Info led_array[LED_NUMBER]={
    {RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_13},
    {RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_14},
    {RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_15}
};

uint8_t LED1_FLASHING = 0;
uint8_t LED2_FLASHING = 0;
uint8_t LED3_FLASHING = 0;

void LED_ON(int index)
{
	if(index >= 0 && index < LED_NUMBER)
	{
		GPIO_ResetBits(led_array[index].gpio,led_array[index].pin);
	}
}

void LED_OFF(int index)
{
	if(index >= 0 && index < LED_NUMBER)
	{
		GPIO_SetBits(led_array[index].gpio,led_array[index].pin);
	}
}

void LEDGpio_Init(void)
{
	uint8_t i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	for(i=0;i<LED_NUMBER;i++)
	{
		RCC_APB2PeriphClockCmd( led_array[i].rcc , ENABLE);
		GPIO_InitStructure.GPIO_Pin = led_array[i].pin;
		GPIO_Init(led_array[i].gpio, &GPIO_InitStructure);
  }
		LED_OFF(0);
		LED_OFF(1);
		LED_OFF(2);
}

void LED_Change(uint8_t id)
{
	if(id >= LED_NUMBER)
	{
		return;
	}
	if(GPIO_ReadOutputDataBit(led_array[id].gpio, led_array[id].pin) == (uint8_t)Bit_RESET)
	{
		GPIO_SetBits(led_array[id].gpio, led_array[id].pin);
	}
	else
	{
		GPIO_ResetBits(led_array[id].gpio, led_array[id].pin);
	}
}

void LED_SetFlashing(uint8_t id, uint8_t bit)
{
	switch(id)
	{
		case LED1:
			LED1_FLASHING = bit;
			break;
		case LED2:
			LED2_FLASHING = bit;
			break;
		case LED3:
			LED3_FLASHING = bit;
			break;
		default:
			return;
	}
}

void LED_Flashing(void)
{
	if(LED1_FLASHING != 0)
	{
		LED_Change(LED1);
	}
	if(LED2_FLASHING != 0)
	{
		LED_Change(LED2);
	}
	if(LED3_FLASHING != 0)
	{
		LED_Change(LED3);
	}
}

void MyGpio_writeIO_Init(Gpio_Info port)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_APB2PeriphClockCmd( port.rcc , ENABLE);
	GPIO_InitStructure.GPIO_Pin = port.pin;
	GPIO_Init(port.gpio, &GPIO_InitStructure);
}

void MyGpio_SetBits(Gpio_Info port)
{
	GPIO_SetBits(port.gpio, port.pin);
}

void MyGpio_ResetBits(Gpio_Info port)
{
	GPIO_ResetBits(port.gpio, port.pin);
}

void MyGpio_WriteBit(Gpio_Info port, uint8_t bit)
{
	if(bit == 0)
		GPIO_WriteBit(port.gpio, port.pin, Bit_RESET);
	else
		GPIO_WriteBit(port.gpio, port.pin, Bit_SET);
}
