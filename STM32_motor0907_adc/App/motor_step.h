/*
* for STEP motor header 
STK682 CONTROLER
*/

#ifndef MOTOR_STEP_H_
#define MOTOR_STEP_H_ 

#define STEP_M_ID 1 //for each motor it should be different

#define STEP_M_ACC_STEP	50 //change speed 50 times ,and it arrive target speed
#define STEP_M_ACC_DELAY 100 //MS slowest:200-> 50ms-  > delay 100 ms,
#define STEP_M_DEFAULT_CIRCLE_PLUS 800*20 // 20:1 mode
#define STEP_M_DEFAULT_ACC	1000 // 100 plus/s

//STEP_EN PA10	//out put enable
#define EN_PORT		GPIOA
#define EN_PIN		GPIO_Pin_4///10->6 clay for uart rx ->4 NEW BOARD
#define STEP_EN_H()	GPIO_SetBits(EN_PORT, EN_PIN)
#define STEP_EN_L()	GPIO_ResetBits(EN_PORT, EN_PIN)

//STEP_CW PA11 //forward/reverse 
#define CW_PORT		GPIOA
#define CW_PIN		GPIO_Pin_5//11->5 NEW BOARD
#define STEP_CW_H()	GPIO_SetBits(CW_PORT, CW_PIN)
#define STEP_CW_L()	GPIO_ResetBits(CW_PORT, CW_PIN)

//STEP_CLK PA6 ---need tim3 ch1 for pwm
#define CLK_PORT	GPIOA
#define CLK_PIN		GPIO_Pin_6//12->6
#define STEP_CLK_H()	GPIO_SetBits(CLK_PORT, CLK_PIN)
#define STEP_CLK_L()	GPIO_ResetBits(CLK_PORT, CLK_PIN)
#define STEP_CLK_TOGGLE() CLK_PORT->ODR ^= CLK_PIN
#define TEST_CLK()  GPIOA->ODR ^= GPIO_Pin_12

//M1_FDT PC9	//decay mode select
#define FDT_PORT	GPIOC//C--> A demo boar ,C for test only
#define FDT_PIN		GPIO_Pin_9
#define STEP_FDT_H()	GPIO_SetBits(FDT_PORT, FDT_PIN)
#define STEP_FDT_L()	GPIO_ResetBits(FDT_PORT, FDT_PIN)

//M1_M1 	PC10 //excitation mode 	switching pin
#define M1_PORT		GPIOC
#define M1_PIN		GPIO_Pin_10
#define STEP_M1_H()	GPIO_SetBits(M1_PORT, M1_PIN)
#define STEP_M1_L()	GPIO_ResetBits(M1_PORT, M1_PIN)

//M1_M2		PC11
#define M2_PORT		GPIOC
#define M2_PIN		GPIO_Pin_11
#define STEP_M2_H()	GPIO_SetBits(M2_PORT, M2_PIN)
#define STEP_M2_L()	GPIO_ResetBits(M2_PORT, M2_PIN)

//M1_M3		PC12
#define M3_PORT		GPIOC
#define M3_PIN		GPIO_Pin_12
#define STEP_M3_H()	GPIO_SetBits(M3_PORT, M3_PIN)
#define STEP_M3_L()	GPIO_ResetBits(M3_PORT, M3_PIN)

typedef enum 
	{
		M_IDLE = 0,
		M_INITED = 1,
		M_ARRIVED_STOP = 2,
		M_DEC_STOP,
		M_SUDDEN_STOP,
		M_ACC_RUN ,
		M_UNIFORM_SPEED_RUN,

	} STEP_M_STATUS;
extern STEP_M_STATUS run_status;

void STEP_M_init(void);
//void STEP_M_CLK_toggle(void);
void STEP_MOT_set_clockwise(bool wise);
void STEP_M_set_enable(bool enable);
void STEP_M_DECAY(char fdt);
void STEP_M_set_excitation(char mode);

void STEP_M_set_clock(uint16_t us);
void STEP_M_set_peroid(int percent);
void STEP_M_start_run(void);
void STEP_M_stop_run(void);//sudden stop
void STEP_M_dec_stop(void);
void STEP_M_run_step(void);
void STEP_M_reset_counter(void);
void STEP_M_set_target_position(uint16_t circle, uint16_t phase);
	void STEP_M_set_actual_position(uint32_t pos);
void STEP_M_set_plus_num_per_circle(uint16_t num);
uint16_t STEP_M_get_uncompleted_circle(void);

	
#endif//MOTOR_STEP_H_
