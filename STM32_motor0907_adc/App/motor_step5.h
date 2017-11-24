/*
* for STEP  5 phase motor header 
timer 1 for pwm a an b bn c cn
A: 	PA8  	IN
AN:	PB13	OUT
B:	PA9		IN	  rx1
BN:	PB14	OUT
C:	PA10	IN	tx1
CN:	PB15	OUT
#if TIM8_PWM
D:	PC7		IN
DN:	PB0	OUT
E:	PC8		IN
EN:	PB1		OUT

pc6 & PA7 
#else
timer 4 for pwm d dn e en
D:	PB6		IN
DN:	PB7		OUT
E:	PB8		IN
EN:	PB9		OUT
#endif
timer 6 for step 
*/

#ifndef MOTOR_STEP5_H_
#define MOTOR_STEP5_H_ 

#define STEP5_DEBUG 1
#define IO_MODE	0//
#define ZHONG_TEST 	1
#define DEAD_AREA (!ZHONG_TEST)

#define TIM8_PWM 1

#define A_PHASE_H_ON 		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable)
#define A_PHASE_H_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable)
#define AN_PHASE_L_ON 		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable)
#define AN_PHASE_L_OFF 	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable)
#define A_PHASE_RISING	 	TIM_SetCompare1(TIM1,pwm_duty_gap*(DUTY_STEP*2 -duty_count))
#define A_PHASE_FALLING 	TIM_SetCompare1(TIM1,pwm_duty_gap*(duty_count-DUTY_STEP*5))
#define A_PHASE_OUT_H		TIM_SetCompare1(TIM1,0)
#define A_PHASE_OUT_L		TIM_SetCompare1(TIM1,pwm_fre+100)
#define A_PHASE_OUT_PWM	TIM_SetCompare1(TIM1,motor_current)
#define A_PHASE_OUT_PWM2	TIM_SetCompare1(TIM1,motor_current/2)
#define A_PORT		GPIOA
#define A_PIN		GPIO_Pin_8 
#define A_IO_H()	GPIO_SetBits(A_PORT, A_PIN)
#define A_IO_L()	GPIO_ResetBits(A_PORT, A_PIN)
#define AN_PORT		GPIOB
#define AN_PIN		GPIO_Pin_13 
#define AN_IO_H()	GPIO_SetBits(AN_PORT, AN_PIN)
#define AN_IO_L()	GPIO_ResetBits(AN_PORT, AN_PIN)

#if 0
#define B_PHASE_H_ON 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable)
#define B_PHASE_H_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable)
#define BN_PHASE_L_ON 		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable)
#define BN_PHASE_L_OFF 	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable)
#define B_PHASE_RISING	 	TIM_SetCompare2(TIM1,pwm_duty_gap*(DUTY_STEP*8 -duty_count))
#define B_PHASE_FALLING 	TIM_SetCompare2(TIM1,pwm_duty_gap*(duty_count-DUTY_STEP))
#define B_PHASE_OUT_H		TIM_SetCompare2(TIM1,0)
#define B_PHASE_OUT_L		TIM_SetCompare2(TIM1,pwm_fre+100)

#define C_PHASE_H_ON 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable)
#define C_PHASE_H_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable)
#define CN_PHASE_L_ON 		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable)
#define CN_PHASE_L_OFF 		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable)
#define C_PHASE_RISING	 	TIM_SetCompare3(TIM1,pwm_duty_gap*(DUTY_STEP*10 -duty_count))
#define C_PHASE_FALLING 	TIM_SetCompare3(TIM1,pwm_duty_gap*(duty_count-DUTY_STEP*3))
#define C_PHASE_FALLING_END 	TIM_SetCompare3(TIM1,pwm_fre+100)//TIM_SetCompare3(TIM1,pwm_duty_gap*(duty_count))
#define C_PHASE_FALLING_START 	TIM_SetCompare3(TIM1,0)//TIM_SetCompare3(TIM1,pwm_duty_gap*( duty_count-DUTY_STEP*8))
#define C_PHASE_OUT_H		TIM_SetCompare3(TIM1,0)
#define C_PHASE_OUT_L		TIM_SetCompare3(TIM1,pwm_fre+100)
#else
#define B_PHASE_H_ON 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable)
#define B_PHASE_H_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable)
#define BN_PHASE_L_ON 		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable)
#define BN_PHASE_L_OFF 		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable)
#define B_PHASE_RISING	 	TIM_SetCompare2(TIM1,pwm_duty_gap*(DUTY_STEP*4 -duty_count))
#define B_PHASE_FALLING 	TIM_SetCompare2(TIM1,pwm_duty_gap*(duty_count-DUTY_STEP*7))
#define B_PHASE_OUT_H		TIM_SetCompare2(TIM1,0)
#define B_PHASE_OUT_L		TIM_SetCompare2(TIM1,pwm_fre+100)
#define B_PHASE_OUT_PWM		TIM_SetCompare2(TIM1,motor_current)
#define B_PHASE_OUT_PWM2		TIM_SetCompare2(TIM1,motor_current/2)
#define B_PORT		GPIOA
#define B_PIN		GPIO_Pin_9 
#define B_IO_H()	GPIO_SetBits(B_PORT, B_PIN)
#define B_IO_L()	GPIO_ResetBits(B_PORT, B_PIN)
#define BN_PORT		GPIOB
#define BN_PIN		GPIO_Pin_14 
#define BN_IO_H()	GPIO_SetBits(BN_PORT, BN_PIN)
#define BN_IO_L()	GPIO_ResetBits(BN_PORT, BN_PIN)


#define C_PHASE_H_ON 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable)
#define C_PHASE_H_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable)
#define CN_PHASE_L_ON 		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable)
#define CN_PHASE_L_OFF 		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable)
#define C_PHASE_RISING	 	TIM_SetCompare3(TIM1,pwm_duty_gap*(DUTY_STEP*6 -duty_count))
#define C_PHASE_FALLING_END 	TIM_SetCompare3(TIM1,pwm_duty_gap*(duty_count))
#define C_PHASE_FALLING_START 	TIM_SetCompare3(TIM1,pwm_duty_gap*( duty_count-DUTY_STEP*8))
#define C_PHASE_OUT_H		TIM_SetCompare3(TIM1,0)
#define C_PHASE_OUT_L		TIM_SetCompare3(TIM1,pwm_fre+100)
#define C_PHASE_OUT_PWM		TIM_SetCompare3(TIM1,motor_current)
#define C_PHASE_OUT_PWM2		TIM_SetCompare3(TIM1,motor_current/2)
#define C_PORT		GPIOA
#define C_PIN		GPIO_Pin_10 
#define C_IO_H()	GPIO_SetBits(C_PORT, C_PIN)
#define C_IO_L()	GPIO_ResetBits(C_PORT, C_PIN)
#define CN_PORT		GPIOB
#define CN_PIN		GPIO_Pin_15 
#define CN_IO_H()	GPIO_SetBits(CN_PORT, CN_PIN)
#define CN_IO_L()	GPIO_ResetBits(CN_PORT, CN_PIN)


#endif

#if  TIM8_PWM

#if STEP5_DEBUG
//#define F_PHASE_OUT_PWM		TIM_SetCompare1(TIM8,motor_current)
//#define F_PHASE_H_ON 		TIM_CCxCmd(TIM8, TIM_Channel_1, TIM_CCx_Enable)
//#define FN_PHASE_L_ON 		TIM_CCxNCmd(TIM8, TIM_Channel_1, TIM_CCxN_Enable)
#define F_PHASE_H_ON 		TIM_CCxCmd(TIM8, TIM_Channel_1, TIM_CCx_Enable)
#define F_PHASE_H_OFF 		TIM_CCxCmd(TIM8, TIM_Channel_1, TIM_CCx_Disable)
#define FN_PHASE_L_ON 		TIM_CCxNCmd(TIM8, TIM_Channel_1, TIM_CCxN_Enable)
#define FN_PHASE_L_OFF 		TIM_CCxNCmd(TIM8, TIM_Channel_1, TIM_CCxN_Disable)
//#define F_PHASE_RISING	 	TIM_SetCompare2(TIM8,pwm_duty_gap*(DUTY_STEP*8 -duty_count))
//#define F_PHASE_FALLING 	TIM_SetCompare2(TIM8,pwm_duty_gap*(duty_count-DUTY_STEP))
#define F_PHASE_OUT_H		TIM_SetCompare1(TIM8,0)
#define F_PHASE_OUT_L		TIM_SetCompare1(TIM8,pwm_fre+100)
#define F_PHASE_OUT_PWM		TIM_SetCompare1(TIM8,motor_current)
#define F_PHASE_OUT_PWM2		TIM_SetCompare1(TIM8,motor_current/2)


#define D_PHASE_H_ON 		F_PHASE_H_ON;TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable)
#define D_PHASE_H_OFF 		F_PHASE_H_OFF;TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable)
#define DN_PHASE_L_ON 		FN_PHASE_L_ON;TIM_CCxNCmd(TIM8, TIM_Channel_2, TIM_CCxN_Enable)
#define DN_PHASE_L_OFF 		FN_PHASE_L_OFF;TIM_CCxNCmd(TIM8, TIM_Channel_2, TIM_CCxN_Disable)
//#define D_PHASE_RISING	 	TIM_SetCompare2(TIM8,pwm_duty_gap*(DUTY_STEP*8 -duty_count))
//#define D_PHASE_FALLING 	TIM_SetCompare2(TIM8,pwm_duty_gap*(duty_count-DUTY_STEP))
#define D_PHASE_OUT_H		F_PHASE_OUT_H;TIM_SetCompare2(TIM8,0)
#define D_PHASE_OUT_L		F_PHASE_OUT_L;TIM_SetCompare2(TIM8,pwm_fre+100)
#define D_PHASE_OUT_PWM		F_PHASE_OUT_PWM;TIM_SetCompare2(TIM8,motor_current)
#define D_PHASE_OUT_PWM2		F_PHASE_OUT_PWM2;TIM_SetCompare2(TIM8,motor_current/2)
#define D_PORT		GPIOC
#define D_PIN		GPIO_Pin_7 
#define D_IO_H()	GPIO_SetBits(D_PORT, D_PIN)
#define D_IO_L()	GPIO_ResetBits(D_PORT, D_PIN)
#define DN_PORT		GPIOB
#define DN_PIN		GPIO_Pin_0 
#define DN_IO_H()	GPIO_SetBits(DN_PORT, DN_PIN)
#define DN_IO_L()	GPIO_ResetBits(DN_PORT, DN_PIN)

#else
#define D_PHASE_H_ON 		TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable)
#define D_PHASE_H_OFF 		TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable)
#define DN_PHASE_L_ON 		TIM_CCxNCmd(TIM8, TIM_Channel_2, TIM_CCxN_Enable)
#define DN_PHASE_L_OFF 	TIM_CCxNCmd(TIM8, TIM_Channel_2, TIM_CCxN_Disable)
#define D_PHASE_RISING	 	TIM_SetCompare2(TIM8,pwm_duty_gap*(DUTY_STEP*8 -duty_count))
#define D_PHASE_FALLING 	TIM_SetCompare2(TIM8,pwm_duty_gap*(duty_count-DUTY_STEP))
#define D_PHASE_OUT_H		TIM_SetCompare2(TIM8,0)
#define D_PHASE_OUT_L		TIM_SetCompare2(TIM8,pwm_fre+100)
#define D_PHASE_OUT_PWM		TIM_SetCompare2(TIM8,motor_current)
#define D_PHASE_OUT_PWM2		TIM_SetCompare2(TIM8,motor_current/2)
#define D_PORT		GPIOC
#define D_PIN		GPIO_Pin_7 
#define D_IO_H()	GPIO_SetBits(D_PORT, D_PIN)
#define D_IO_L()	GPIO_ResetBits(D_PORT, D_PIN)
#define DN_PORT		GPIOB
#define DN_PIN		GPIO_Pin_0 
#define DN_IO_H()	GPIO_SetBits(DN_PORT, DN_PIN)
#define DN_IO_L()	GPIO_ResetBits(DN_PORT, DN_PIN)

#endif 

#define E_PHASE_H_ON 		TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable)
#define E_PHASE_H_OFF 		TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable)
#define EN_PHASE_L_ON 		TIM_CCxNCmd(TIM8, TIM_Channel_3, TIM_CCxN_Enable)
#define EN_PHASE_L_OFF 		TIM_CCxNCmd(TIM8, TIM_Channel_3, TIM_CCxN_Disable)
#define E_PHASE_RISING	 	TIM_SetCompare3(TIM8,pwm_duty_gap*(DUTY_STEP*10 -duty_count))
#define E_PHASE_FALLING 	TIM_SetCompare3(TIM8,pwm_duty_gap*(duty_count-DUTY_STEP*3))
#define E_PHASE_OUT_H		TIM_SetCompare3(TIM8,0)
#define E_PHASE_OUT_L		TIM_SetCompare3(TIM8,pwm_fre+100)
#define E_PHASE_OUT_PWM		TIM_SetCompare3(TIM8,motor_current)
#define E_PHASE_OUT_PWM2		TIM_SetCompare3(TIM8,motor_current/2)
#define E_PORT		GPIOC
#define E_PIN		GPIO_Pin_8 
#define E_IO_H()	GPIO_SetBits(E_PORT, E_PIN)
#define E_IO_L()	GPIO_ResetBits(E_PORT, E_PIN)
#define EN_PHASE_PORT		GPIOB
#define EN_PHASE_PIN		GPIO_Pin_1 
#define EN_IO_H()	GPIO_SetBits(EN_PHASE_PORT, EN_PHASE_PIN)
#define EN_IO_L()	GPIO_ResetBits(EN_PHASE_PORT, EN_PHASE_PIN)



#else
#define D_PHASE_H_ON 		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable)
#define D_PHASE_H_OFF 		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable)
#define DN_PHASE_L_ON 		TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Enable)
#define DN_PHASE_L_OFF 	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Disable)

#define E_PHASE_H_ON 		TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Enable)
#define E_PHASE_H_OFF 		TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Disable)
#define EN_PHASE_L_ON 		TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable)
#define EN_PHASE_L_OFF 		TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Disable)
#endif

#if 0
#define A_PHASE_IN_ON 		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable)
#define A_PHASE_IN_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable)
#define AN_PHASE_OUT_ON 	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable)
#define AN_PHASE_OUT_OFF 	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable)

#define B_PHASE_IN_ON 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable)
#define B_PHASE_IN_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable)
#define BN_PHASE_OUT_ON 	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable)
#define BN_PHASE_OUT_OFF 	TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable)

#define C_PHASE_IN_ON 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable)
#define C_PHASE_IN_OFF 		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable)
#define CN_PHASE_OUT_ON 	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable)
#define CN_PHASE_OUT_OFF 	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable)

#define D_PHASE_IN_ON 		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable)
#define D_PHASE_IN_OFF 		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable)
#define DN_PHASE_OUT_ON 	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Enable)
#define DN_PHASE_OUT_OFF 	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Disable)

#define E_PHASE_IN_ON 		TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Enable)
#define E_PHASE_IN_OFF 		TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Disable)
#define EN_PHASE_OUT_ON 	TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable)
#define EN_PHASE_OUT_OFF 	TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Disable)
#endif

#if 0//STEP5_DEBUG
//test only
#define CLK5_PORT	GPIOA
#define CLK5_PIN		GPIO_Pin_6 //12->7
#define STEP5_CLK_H()	GPIO_SetBits(CLK5_PORT, CLK5_PIN)
#define STEP5_CLK_L()	GPIO_ResetBits(CLK5_PORT, CLK5_PIN)
#define STEP5_CLK_TOGGLE() CLK5_PORT->ODR ^= CLK5_PIN
#define TEST5_CLK_TOGGLE()  GPIOA->ODR ^= GPIO_Pin_12
#else
#define CLK5_PORT	GPIOA
#define CLK5_PIN		GPIO_Pin_6 //12->7
#define STEP5_CLK_H()	//GPIO_SetBits(CLK5_PORT, CLK5_PIN)
#define STEP5_CLK_L()	//GPIO_ResetBits(CLK5_PORT, CLK5_PIN)
#define STEP5_CLK_TOGGLE() //CLK5_PORT->ODR ^= CLK5_PIN
#define TEST5_CLK_TOGGLE()  //GPIOA->ODR ^= GPIO_Pin_12
#endif

/*
#define CLK_CTRL	PB5	
#define DIR_CTRL	PB8	
#define LOCK_CTRL	PB9
#define RES_CTRL 	PB3

ADC_IN0 PA0
AMOTOR1 PA1--CURRENT FEEDBACK
AMOTOR2 PA4
AMOTOR3 PA5
LED1    PA6
AFAUT 	PA11 // ir2316 out
EN_A 	PA12 // CTRL ir2316
S3_CLK	PB5
INPUT PB10/PB11/PB12
EN_B	PC4
out P480 PC9/PC10/PC11/PC12
*/

#define CLK_PORT	GPIOB
#define CLK_PIN		GPIO_Pin_5 //12->7
#define CLK_STATU   GPIO_ReadInputDataBit(CLK_PORT,CLK_PIN)
#define CLK_INT_LINE EXTI_Line5
#define CLK_IRQ_CHN EXTI9_5_IRQn

#define DIR_PORT	GPIOB
#define DIR_PIN		GPIO_Pin_8 //12->7
#define DIR_STATU   GPIO_ReadInputDataBit(DIR_PORT,DIR_PIN)
#define DIR_INT_LINE EXTI_Line8
#define DIR_IRQ_CHN EXTI9_5_IRQn

#define STOP_PORT	GPIOB
#define STOP_PIN	GPIO_Pin_9 //12->7
#define STOP_STATU   GPIO_ReadInputDataBit(STOP_PORT,STOP_PIN)
#define STOP_INT_LINE EXTI_Line9
#define STOP_IRQ_CHN EXTI9_5_IRQn


#define EN1_PORT	GPIOA
#define EN1_PIN		GPIO_Pin_12 //12->7
#define EN1_ENABLE()	GPIO_SetBits(EN1_PORT, EN1_PIN)
#define EN1_DISABLE()	GPIO_ResetBits(EN1_PORT, EN1_PIN)

#define EN2_PORT	GPIOA
#define EN2_PIN		GPIO_Pin_11 //12->7
#define EN2_ENABLE()	GPIO_SetBits(EN2_PORT, EN2_PIN)
#define EN2_DISABLE()	GPIO_ResetBits(EN2_PORT, EN2_PIN)


#define 		M5_IDLE  			0 
#define		M5_INITED 			1
#define		M5_ARRIVED_STOP 	 2
#define		M5_DEC_STOP		3
#define		M5_LIMIT_STOP		4
#define		M5_SUDDEN_STOP	5
#define		M5_ACC_RUN 		6
#define		M5_RUN 			7

extern char step5_status;


extern void STEP5_motor_init(void);
extern void STEP5_motor_phase_INT(void);
extern void STEP5_pwm_current(char percent);
extern void STEP5_pwm_set_fre(int fre);
extern void STEP5_start_run(void);
extern void STEP5_stop_run(void);
extern void STEP5_set_speed(int speed);
extern void STEP5_set_clockwise(bool wise);
extern char STEP5_get_status(void);
extern void STEP5_loop();

void Step5_CLK_INT();
void Step5_DIR_INT();
void Step5_STOP_INT();
#endif//MOTOR_STEP5_H_
