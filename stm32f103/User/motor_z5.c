#include "motor_z5.h"
#include "MyGpio.h"
#include "MyTimer.h"

uint16_t z5_array[M5_Z5] = {0x240, 0x90, 0x24, 0x9, 0x102};
uint16_t z10_array[M5_IO_NUMBER] = {0x210, 0x294, 0x84, 0xA5, 0x21, 0x129, 0x108, 0x14A, 0x42, 0x252};

Gpio_Info m5io_array[M5_IO_NUMBER];
Tim_Info  m5_timer;

uint8_t m5_iobit = 0;
uint8_t m5_clcwise = 0;
uint8_t m5_model = 0;
uint8_t m5_Zindex = 0;

void M5_Configure(void)
{
	uint8_t i = 0;
	m5io_array[0].gpio = GPIOA;
	m5io_array[0].rcc = RCC_APB2Periph_GPIOA;
	m5io_array[0].pin = GPIO_Pin_8;
	
	m5io_array[1].gpio = GPIOB;
	m5io_array[1].rcc = RCC_APB2Periph_GPIOB;
	m5io_array[1].pin = GPIO_Pin_13;
	
	m5io_array[2].gpio = GPIOA;
	m5io_array[2].rcc = RCC_APB2Periph_GPIOA;
	m5io_array[2].pin = GPIO_Pin_9;
	
	m5io_array[3].gpio = GPIOB;
	m5io_array[3].rcc = RCC_APB2Periph_GPIOB;
	m5io_array[3].pin = GPIO_Pin_14;
	
	m5io_array[4].gpio = GPIOA;
	m5io_array[4].rcc = RCC_APB2Periph_GPIOA;
	m5io_array[4].pin = GPIO_Pin_10;
	
	m5io_array[5].gpio = GPIOB;
	m5io_array[5].rcc = RCC_APB2Periph_GPIOB;
	m5io_array[5].pin = GPIO_Pin_15;
	
	m5io_array[6].gpio = GPIOC;
	m5io_array[6].rcc = RCC_APB2Periph_GPIOC;
	m5io_array[6].pin = GPIO_Pin_7;
	
	m5io_array[7].gpio = GPIOB;
	m5io_array[7].rcc = RCC_APB2Periph_GPIOB;
	m5io_array[7].pin = GPIO_Pin_0;
	
	m5io_array[8].gpio = GPIOC;
	m5io_array[8].rcc = RCC_APB2Periph_GPIOC;
	m5io_array[8].pin = GPIO_Pin_8;
	
	m5io_array[9].gpio = GPIOB;
	m5io_array[9].rcc = RCC_APB2Periph_GPIOB;
	m5io_array[9].pin = GPIO_Pin_1;
	
	
	for(i = 0; i < M5_IO_NUMBER; i++)
	{
		MyGpio_writeIO_Init(m5io_array[i]);
		MyGpio_ResetBits(m5io_array[i]);
	}
	
	m5_timer.name = TIM7;
	m5_timer.isAPB1 = 1;
	m5_timer.rcc = RCC_APB1Periph_TIM7;
	m5_timer.irqn = TIM7_IRQn;
	m5_timer.itflag = TIM_FLAG_Update;
	m5_timer.it = TIM_IT_Update;
	//m5_timer.cnl = TIMR_CNL_1;
	m5_timer.ithandler = M5_TIM_IT_Handler;
	m5_timer.arr = PB1_TIM_ARR;
	m5_timer.psc = PB1_TIM_PSC;
	m5_timer.count = 0;
	
	Timer_Init(m5_timer);
}

void M5_Start(void)
{
	Timer_Start(m5_timer);
}

void M5_Stop(void)
{
	Timer_Stop(m5_timer);
}

void M5_TIM_IT_Handler(void)
{
	uint16_t m5_a = 0;
	uint8_t i = 0;
	if(TIM_GetITStatus(m5_timer.name, m5_timer.itflag) != RESET)
	{
		m5_timer.count++;

		if(m5_iobit == 0)
			m5_iobit = 1;
		else
			m5_iobit = 0;
		
		if(m5_clcwise == 0)
				m5_Zindex++;
			else
				m5_Zindex--;
		
		if(0 == m5_model)
			m5_a = z5_array[m5_Zindex%5];
		else
			m5_a = z10_array[m5_Zindex%5];
		for(i = 0; i < 10; i++)
		{
			//GPIO_WriteBit(m5io_array[i].gpio,m5io_array[i].pin, (BitAction)(m5_iobit & (m5_a & ((1 << i) >> i))));
			//Gpio_Writebit(m5io_array[i], (BitAction)(m5_iobit & (m5_a & ((1 << i) >> i))));
			//MyGpio_Writebit(m5io_array[i], (m5_iobit & ((m5_a & (1 << i)) >> i)));
			if((m5_iobit & ((m5_a & (1 << i)) >> i)) == 1)
				MyGpio_SetBits(m5io_array[i]);
			else
				MyGpio_ResetBits(m5io_array[i]);
		}
	}
	TIM_ClearITPendingBit(m5_timer.name, m5_timer.itflag);
}

