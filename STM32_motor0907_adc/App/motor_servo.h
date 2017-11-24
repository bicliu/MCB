/*
* for one motor header 
*/

#ifndef SERVO_M_SERVO_H_
#define SERVO_M_SERVO_H_ 

#define SERVO_M_ID 0 //for each motor it should be different

//output
#define 	CH1_PORT		GPIOC
#define 	CH1_PIN		GPIO_Pin_6
#define  CH1_ON()   	GPIO_ResetBits(CH1_PORT, CH1_PIN)
#define  CH1_OFF()   	GPIO_SetBits(CH1_PORT, CH1_PIN)

#define 	CH1N_PORT		GPIOA
#define 	CH1N_PIN		GPIO_Pin_7
#define  CH1N_ON()   	GPIO_ResetBits(CH1N_PORT, CH1N_PIN)
#define  CH1N_OFF()   	GPIO_SetBits(CH1N_PORT, CH1N_PIN)

#define 	CH2_PORT		GPIOC
#define 	CH2_PIN		GPIO_Pin_7
#define  CH2_ON()   	GPIO_ResetBits(CH2_PORT, CH2_PIN)
#define  CH2_OFF()   	GPIO_SetBits(CH2_PORT, CH2_PIN)

#define 	CH2N_PORT		GPIOB
#define 	CH2N_PIN		GPIO_Pin_0
#define  CH2N_ON()   	GPIO_ResetBits(CH2N_PORT, CH2N_PIN)
#define  CH2N_OFF()   	GPIO_SetBits(CH2N_PORT, CH2N_PIN)

#define 	CH3_PORT		GPIOC
#define 	CH3_PIN		GPIO_Pin_8
#define  CH3_ON()   	GPIO_ResetBits(CH3_PORT, CH3_PIN)
#define  CH3_OFF()   	GPIO_SetBits(CH3_PORT, CH3_PIN)

#define 	CH3N_PORT		GPIOB
#define 	CH3N_PIN		GPIO_Pin_1
#define  CH3N_ON()   	GPIO_ResetBits(CH3N_PORT, CH3N_PIN)
#define  CH3N_OFF()   	GPIO_SetBits(CH3N_PORT, CH3N_PIN)

#define 	STOP_PORT		GPIOB
#define 	STOP_PIN		GPIO_Pin_8
#define  SERVO_M_STOP()   	GPIO_ResetBits(STOP_PORT, STOP_PIN) // LOW for stop motor power
#define  SERVO_M_ENABLE()   	GPIO_SetBits(STOP_PORT, STOP_PIN)

//VOA PB11
//VOB PB10
//VOC PB12

//AD1_MCLK PC0
//AD1_MDAT PC1
//AD2_MCLK PC2
//AD2_MDAT PC3

//input Fault out 

void SERVO_M_init(void);
void SERVO_M_start(void);
void MOTOT_stop(void);

void SERVO_M_run_step(void);
void SERVO_M_set_clockwise(bool wise);
void SERVO_M_set_step_interval(int us);
void SERVO_M_fault_out(void);
//void SERVO_M_clockwise_run_one_step(void);
//void SERVO_M_anti_clockwise_run_one_step(void);

#endif//SERVO_M_SERVO_H_
