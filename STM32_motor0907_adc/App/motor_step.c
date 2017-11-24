#include "stm32f10x.h"
#include "motor_step.h"
#include "SysTickDelay.h"

static char div_mode = 0; //excitation mode
/*volatile*/static int plus_counter = 8000*4; //PLUS = plus_counter/2
static uint16_t plus_per_circle = 0;
//target position set by host
static uint16_t target_circle = 0; //use plus number to caculate
static uint16_t target_phase = 0;
//motor current position , the plus number had sent
/*static*/ uint16_t current_circle = STEP_M_DEFAULT_CIRCLE_PLUS; //count in plus number
//static uint16_t current_phase = 0;
STEP_M_STATUS run_status = M_IDLE ;// 0: run , 1, stop by command , 2, stop when run to position
static uint16_t current_speed = 0 ;//
static uint16_t target_speed = 0;// v1=v0+at target_speed = current_speed + acc *  acc_time
static uint16_t start_speed = 0;//
static uint16_t acc = STEP_M_DEFAULT_ACC ;//  
//static uint16_t acc_time = 0 ;// t = (v1-v0)/a
//static char acc_step = 0 ;
static char acc_delay = STEP_M_ACC_DELAY ;

/*
//excitation setting //default state at start-up/reset
M3/s2	M2/s3	M1/s4	MODE_exciation
L			L			L		2 PHASE
//div1,200 ,10~617HZ (800)
L			L			H		1-2PHASE
//div2,400   ~2.38K (200~300)
L			H			L		W1-2
//div4,800    4.545k (100)
L			H			H		2W1-2
//div8,1600 ~9.8k (50)
H			L			L		4W1-2
//div16,3200 ~16k (30)
H			L			H		8W1-2
//div32,6400
H			H			L		16W1-2
//div64,12800
H			H			H		32W1-2
//div128, 25600 0~15k : max ~ 66/2us , 65535->7Hz ,33->14.5kHz

for mode  L			H			L		W1-2 //div4,800    4.545k (100)
#define SLOW 600 //50*1000--10Hz
#define FAST 	100   //50 10K
#define GAP  10 
STEP = (SLOW -FAST)/GAP = 50
*/

void STEP_M_set_excitation(char mode){
	div_mode = mode;
	switch(mode){
		case 0:
			STEP_M3_L();
			STEP_M2_L();
			STEP_M1_L();
			break;
		case 1:
			STEP_M3_L();
			STEP_M2_L();
			STEP_M1_H();
			break;
		case 2:
			STEP_M3_L();
			STEP_M2_H();
			STEP_M1_L();
			break;
		case 3:
			STEP_M3_L();
			STEP_M2_H();
			STEP_M1_H();
			break;
		case 4:
			STEP_M3_H();
			STEP_M2_L();
			STEP_M1_L();
			break;
		case 5:
			STEP_M3_H();
			STEP_M2_L();
			STEP_M1_H();
			break;
		case 6:
			STEP_M3_H();
			STEP_M2_H();
			STEP_M1_L();
			break;
		case 7:
			STEP_M3_H();
			STEP_M2_H();
			STEP_M1_H();
			break;
		default:
			div_mode = 0;
		break;
	}
}


/*	CLK pin step signal iputt allows advancing excitation step 
VCC == H 
CLK rasing edge : excitation step feed
CLK failing	edge:	excitation step hold
*/
void STEP_M_CLK_toggle(void){
//	//GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];		//取反输出寄存器数据
//	if(plus_counter > 0 )
		{ //must even
		//GPIOA->ODR ^= GPIO_Pin_12;//test
			TEST_CLK();
		//CLK_PORT->ODR ^= CLK_PIN;
			STEP_CLK_TOGGLE();
		//plus_counter--;
	}
//	else{
//		STM_EVAL_LEDOn(1);//LED2
//	}
}

/* CW/CCW 
L: CW  FORWORD  excitation: 1->2->3->4
H: CCCW BACKWOARD	excation: 6->5->4
at clk rasing edge
*/
void STEP_MOT_set_clockwise(bool wise){
	if(wise){
		STEP_CW_H();
	}
	else{
		STEP_CW_L();
	}
}

/*
When the ENABLE pin is set Low, the output is forced OFF and goes to high impedance. However, the internal logic
circuits are operating, so the excitation position proceeds when the CLK is input. Therefore, when ENABLE pin is
returned to High, the output level conforms to the excitation position proceeded by the CLK input.
*/
void STEP_M_set_enable(bool enable){
	if(enable)
		STEP_EN_H();
	else
		STEP_EN_L();
}

/*
FDT 
>3.5v	SLOW DECAY
1.1V~3.1V OR OPEN : MIXED DECAY
<0.8V	FAST DECAY
*/
void STEP_M_DECAY(char fdt){
	if(fdt ==0)
		STEP_FDT_L();
	else
			STEP_FDT_H();
}

void STEP_M_IO_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE);//|RCC_APB2Periph_AFIO
	
  GPIO_InitStructure.GPIO_Pin = EN_PIN;//STEP_EN PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EN_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = CW_PIN;//STEP_CW PA11 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CW_PORT, &GPIO_InitStructure);

		/*GPIOA Configuration: TIM3 channel1*/	  //设置该引脚为复用输出功能,输出TIM3 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = CLK_PIN; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = FDT_PIN;//M1_FDT PC9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(FDT_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = M1_PIN;////M1_M1 	PC10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(M1_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = M2_PIN;////M1_M2 	PC11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(M2_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = M3_PIN;////M1_M3 	PC12
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(M3_PORT, &GPIO_InitStructure);

	STEP_CLK_L();//default for rasing edge  	
	STEP_M3_L();
	STEP_M2_L();
	STEP_M1_L();
	STEP_CW_L();
	STEP_EN_L();
	STEP_FDT_L();
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //test
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

//static int bak_peroid =100;

void TIM_Configuration(uint16_t interval)
	{
		TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
//		TIM_OCInitTypeDef  TIM_OCInitStructure;
		uint16_t peroid,scaler;
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
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
	TIM3CLK 即PCLK1=36MHz
	TIM3CLK = 36 MHz, Prescaler = 7200, TIM3 counter clock = 5K,即改变一次为5K,周期就为10K
	--------------------------------------------------------------- */
	/* Time base configuration */
	TIM3_TimeBaseStructure.TIM_Period = peroid; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM3_TimeBaseStructure.TIM_Prescaler =(scaler-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM3_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	/* Enables the Update event for TIM3 */
	//TIM_UpdateDisableConfig(TIM3,ENABLE); 	//使能 TIM3 更新事件 
	
	/* TIM IT enable */
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM3
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
//	
	/* TIM3 enable counter */
	//TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
//	
//	TIM3_TimeBaseStructure.TIM_Period = 2000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
//	TIM3_TimeBaseStructure.TIM_Prescaler =(1); //设置用来作为TIMx时钟频率除数的预分频值  不分频
//	TIM3_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
//	TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//	
//	/* Output Compare Active Mode configuration: Channel1 */
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2 TIM_OCMode_PWM2 for ch2
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
//	//TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器
//	//TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
//		
//	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
//	
	
	
//	TIM_SetCompare1(TIM3,1000);
	/* TIM3 enable counter */
//	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
	}


void STEP_M_timer_init(void){

	NVIC_InitTypeDef NVIC_InitStructure;
		/* Enable the TIM5 for motor0 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断 PWM PA6/PA7
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

}

// 0~15k : max ~ 66/2us , 65535->7Hz ,33->14.5kHz
void STEP_M_set_clock(uint16_t us){
	TIM_Configuration(us);
	current_speed = us ;
//	TIM_Configuration(32768);
}

void STEP_M_set_start_speed(uint16_t us){
	start_speed = us;
}

void STEP_M_set_target_speed(uint16_t us){
	target_speed = us;
}

void STEP_M_set_speed_acc(uint16_t us){
	acc = us;
}

void STEP_M_get_acc_delay(){
	uint16_t acc_time = 0 ;
	if((acc_time == 0)&&(acc != 0)){
		acc_time = (target_speed - target_speed) / acc ;
		acc_delay = acc_time / STEP_M_ACC_STEP;
	}
}

void STEP_M_set_peroid(int percent){
	
	//TIM_SetCompare1(TIM3,bak_peroid*percent/100); 
}
void STEP_M_start_run(void){
	run_status = M_UNIFORM_SPEED_RUN ;
	TIM_Cmd(TIM3, ENABLE); 
}

void STEP_M_stop_run(void){ //sudden stop
	run_status = M_SUDDEN_STOP;
	TIM_Cmd(TIM3, DISABLE);
	STEP_M_set_enable(FALSE);
	current_speed = 0;
}

void STEP_M_dec_stop(void){
	/*uint16_t  distance;
	char i; 
	
	if (current_speed > start_speed){
		distance = (current_speed - start_speed) / STEP_M_ACC_STEP;
		for(i=0;i< STEP_M_ACC_STEP;i++){
			current_speed -= distance;
			TIM_Configuration(current_speed);
			delay_ms(acc_delay);
		}
	}
	*/
	//t= (v0-v1)/a
	uint16_t acc_time ,i,gap;
	gap = (acc * acc_delay)  /(1000 ) ;// acc plus/s , delay: ms ,so / 1000
	acc_time = (current_speed * 1000 )/acc ;
	for(i=0;i< acc_time;){
			TIM_Configuration(current_speed);
			delay_ms(acc_delay);
			i += acc_delay;
			current_speed -= gap ;
			if(current_speed <= 0 ) break;
		}
	
	current_speed = 0;
	TIM_Cmd(TIM3, DISABLE);
	STEP_M_set_enable(FALSE);
}

void STEP_M_acc_start(){
	bool speed_acc = TRUE ;
	uint16_t acc_time ,i,gap;
	gap = (acc * acc_delay)  /(1000 ) ;// acc plus/s , delay: ms ,so / 1000
	if( target_speed > start_speed ){
		acc_time = ( target_speed - start_speed )* 1000 /acc ;
		}
	else{
		acc_time = ( start_speed - target_speed  )* 1000 /acc ;
		speed_acc = FALSE;
		}
	current_speed = start_speed;		
	for(i=0;i< acc_time;){
			TIM_Configuration(current_speed);
			delay_ms(acc_delay);
			i += acc_delay;
			if(speed_acc){
				current_speed += gap ;
				if(current_speed > target_speed ) break;
			}
			else{
					current_speed -= gap ;
					if(current_speed < target_speed ) break;
				}
		}
	
	current_speed = target_speed;
	TIM_Configuration(current_speed);
}

void STEP_M_init(void){
	STEP_M_IO_init();
	STEP_M_timer_init();
}

void STEP_M_set_plus_num_per_circle(uint16_t num){
	if(div_mode == 0)  //TODO GET FROM div_mode
		plus_per_circle = num;
	plus_per_circle = num;
}

void STEP_M_reset_counter(){
	current_circle = 0 ;
//	current_phase = 0 ;
	plus_counter = 0	;
}

void STEP_M_set_target_position(uint16_t circle, uint16_t phase){
	target_circle = circle ;
	target_phase = phase ;
	current_circle = target_circle;
//	current_phase = target_phase;
	if(current_circle == 0)
		plus_counter = target_phase * 2;//for timer interrupt 2times just one plus
	else
		plus_counter = plus_per_circle * 2;//for timer interrupt 2times just one plus
}

void STEP_M_set_actual_position(uint32_t pos){
	STEP_M_set_target_position(pos/plus_per_circle, pos%plus_per_circle);
}

void STEP_M_run_step(void){//call in timer interrupt 

//		TEST_CLK();
//		STEP_CLK_TOGGLE();

	if((current_circle > 0)&&(plus_counter > 0)){
			TEST_CLK();
			STEP_CLK_TOGGLE();
			plus_counter--;
			if(plus_counter <= 0){
				current_circle--;
				if(current_circle > 0){
						plus_counter = plus_per_circle *2;// //for timer interrupt 2times just one plus
						//STM_EVAL_LEDOn(0);
					}
					else{
						plus_counter = target_phase *2;//for timer interrupt 2times just one plus
						//STM_EVAL_LEDOn(1);
					}		
				}
	}
	else{
					if((current_circle <= 0)&&((plus_counter > 0))){
								TEST_CLK();
								STEP_CLK_TOGGLE();
								plus_counter--;
					}
					else{
							//arrive target position , do nothing
							STEP_M_stop_run();
							run_status = M_ARRIVED_STOP;
							//STM_EVAL_LEDOff(0);
							//STM_EVAL_LEDOn(1);
					}
	}	
}

uint16_t STEP_M_get_uncompleted_circle(void){
	return current_circle;
}

//debug only , it change quickly
//uint16_t STEP_M_get_uncompleted_phase(void){
//	return plus_counter;
//}
