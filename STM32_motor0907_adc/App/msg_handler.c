/*
* 
*/
#include "stm32f10x_it.h"
#include "msg_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "UART_INTERFACE.h"
#include "motor_step.h"

u8 BOARD_ID=1;
// bit 7~4: id/axis, bit 3~0: error id 
u8 ERR_REPORT_STA=0x21; 

#define BUF_SIZE 64
u8 USART_RX_BUF[BUF_SIZE];     //接收缓冲,最大64个字节.
char cmd_buf[BUF_SIZE]; //copy the cmd
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记

char TX_BUF[64];   
 const char *pERR_STR[ERR_MAX]={
	STR_ERR_STATUS,
	STR_ERR_SERVO_FAULT_OUT,
};

extern volatile int duty ;//ms
extern volatile int step_delay ;//ms
void cmd_process(void){
	char *p=NULL, *pfun=NULL,*ptmp;
	char id = 0, axis = 0;
	int value;
	
	p = strtok(cmd_buf, MSG_SPILT);
	pfun = p;
#if 1
	if(p){//
		DEBUG("\n get function : %s",p);
			if(0 == strcmp(p,"duty")){
					p = strtok(NULL, MSG_SPILT);
					value= strtol((p), &ptmp,10);
					DEBUG("\n get duty str =%s ,value=%d",(p),value);
					duty = value;
				}
			}
		 if(0 == strcmp(p,"speed")){
					p = strtok(NULL, MSG_SPILT);
					DEBUG("\n get speed str =%s ,value=%d",(p),value);
					value= strtol((p), &ptmp,10);
					DEBUG("\n get speed22 str =%s ,value=%d",(p),value);
					step_delay = value;
			}
#else
	if(p){//
		DEBUG("\n get function : %s",p);
		//stop should be high piority
			if(0 == strcmp(p,"SET_STOP")){//SET_STOP#ID@XX#AXIS@XX#STOP@XXX
						p = strtok(NULL, MSG_SPILT);
						if(p){//"ID@01"
							id = (char)strtol((p+3), &ptmp,10);
							DEBUG("\n get ID str =%s ,value=%d",(p+3),id);
							p = strtok(NULL, MSG_SPILT);
							if(p){//AXIS@2
								axis = (char)strtol((p+5), &ptmp,10);
								DEBUG("\n get axis str =%s,value =%d",(p+5),axis);
								p = strtok(NULL, MSG_SPILT);
								if(p){
									DEBUG("\n get stop command =%s",(p));//STOP
									if( 0 == strncmp(p, "STOP", 4)){
											DEBUG("\n get stop =%s",(p+5));//STOP@
											value = (int)strtol((p+5), &ptmp,10);//10:oct, 16:hex ...
											DEBUG("\n parse stop  value =%d",value);
										if(value == 0)
											STEP_M_stop_run();//stop quickly
										else
											STEP_M_stop_run();
										goto STEP_M_CMD_SUCC;
									}
								}
								else 
									goto STEP_M_CMD_ERR;//speed value 
							}
							else 
								goto STEP_M_CMD_ERR;//axis
						}
						else
						{
							goto STEP_M_CMD_ERR;//id
						}
		}
		else if(0 == strcmp(p,"SET_SPEED")){ //SET_SPEED#ID@XX#AXIS@XX#SPEED@XXX
			p = strtok(NULL, MSG_SPILT);
			if(p){//"ID@01"
				id = (char)strtol((p+3), &ptmp,10);
				DEBUG("\n get ID str =%s ,value=%d",(p+3),id);
				p = strtok(NULL, MSG_SPILT);
				if(p){//AXIS@2
					axis = (char)strtol((p+5), &ptmp,10);
					DEBUG("\n get axis str =%s,value =%d",(p+5),axis);
					p = strtok(NULL, MSG_SPILT);
					if(p){
						DEBUG("\n get SPEED =%s",(p+6));//SPEED@
						value = (int)strtol((p+6), &ptmp,10);//10:oct, 16:hex ...
						DEBUG("\n parse SPEED  value =%d",value);
						//STEP_M_stop_run(); no need to stop
						STEP_M_set_clock(value);						
						STEP_M_start_run();
						goto STEP_M_CMD_SUCC;
					}
					else 
						goto STEP_M_CMD_ERR;//speed value 
				}
				else 
					goto STEP_M_CMD_ERR;//axis
			}
			else
			{
				goto STEP_M_CMD_ERR;//id
			}
		}
		else if(0 == strcmp(p,"SET_LOGIC_POS")){//SET_LOGIC_POS#ID@XX#AXIS@XX#POS@XXX
						p = strtok(NULL, MSG_SPILT);
						if(p){//"ID@01"
							id = (char)strtol((p+3), &ptmp,10);
							DEBUG("\n get ID str =%s ,value=%d",(p+3),id);
							p = strtok(NULL, MSG_SPILT);
							if(p){//AXIS@2
								axis = (char)strtol((p+5), &ptmp,10);
								DEBUG("\n get axis str =%s,value =%d",(p+5),axis);
								p = strtok(NULL, MSG_SPILT);
								if(p){
									DEBUG("\n get POS =%s",(p+4));//POS@
									value = (int)strtol((p+4), &ptmp,10);//10:oct, 16:hex ...
									DEBUG("\n parse POS  value =%d, circle=%d, phase=%d",value,value/400,value%400);
									STEP_M_set_actual_position(value);						
									goto STEP_M_CMD_SUCC;
								}
								else 
									goto STEP_M_CMD_ERR;//speed value 
							}
							else 
								goto STEP_M_CMD_ERR;//axis
						}
						else
						{
							goto STEP_M_CMD_ERR;//id
						}
		}
		else if(0 == strcmp(p,"SET_CIRCLE")){//SET_CIRCLE#ID@XX#AXIS@XX#NUM@XXX
						p = strtok(NULL, MSG_SPILT);
						if(p){//"ID@01"
							id = (char)strtol((p+3), &ptmp,10);
							DEBUG("\n get ID str =%s ,value=%d",(p+3),id);
							p = strtok(NULL, MSG_SPILT);
							if(p){//AXIS@2
								axis = (char)strtol((p+5), &ptmp,10);
								DEBUG("\n get axis str =%s,value =%d",(p+5),axis);
								p = strtok(NULL, MSG_SPILT);
								if(p){
									DEBUG("\n get plus per circle  =%s",(p+4));//NUM@
									value = (int)strtol((p+4), &ptmp,10);//10:oct, 16:hex ...
									DEBUG("\n parse plus per circle value =%d, ",value);
									STEP_M_set_plus_num_per_circle(value);						
									goto STEP_M_CMD_SUCC;
								}
								else 
									goto STEP_M_CMD_ERR;//speed value 
							}
							else 
								goto STEP_M_CMD_ERR;//axis
						}
						else
						{
							goto STEP_M_CMD_ERR;//id
						}
		}
		else
			goto STEP_M_CMD_ERR;//function
	}
	else{
		goto STEP_M_CMD_ERR;
	}
	
	STEP_M_CMD_SUCC:
		printf("\nACK#%s#SUCCESS",pfun);
		return;
	STEP_M_CMD_ERR:
		printf("\nACK#%s#FAIL",pfun);
		return;
#endif
}

void msg_receive(){
	u8 t;
	u8 len;	
	if(USART_RX_STA&0x80)
			{					   
				len=USART_RX_STA&0x3f;//得到此次接收到的数据长度
				memset(cmd_buf, 0, BUF_SIZE);
				//printf("\n input :\n");
				for(t=0;t<len;t++) //for USART_RX_BUF may not be string
				{
					//USART1->DR=USART_RX_BUF[t];
					//while((USART1->SR&0X40)==0);//等待发送结束
					//STM_EVAL_SendChar(USED_COM_NUMBER,(uint8_t)USART_RX_BUF[t]);
					cmd_buf[t] = (uint8_t)USART_RX_BUF[t];
				}
				//printf("\n\n");//插入换行
				DEBUG("\ninput : %s",cmd_buf);
				USART_RX_STA=0;
				cmd_process();
			}
}
void msg_send(){
	
	if(ERR_REPORT_STA){
		//memset(TX_BUF, 0, 64);
		sprintf(TX_BUF,"REPORT#ID@%d#AXIS@%d#STATUS@%s\n",BOARD_ID,ERR_REPORT_STA>>4,pERR_STR[ERR_REPORT_STA& ERR_MAX]);
		printf(TX_BUF);
		ERR_REPORT_STA=0;
	}
}

void msg_process(void){
	
	msg_receive();
//	msg_send();

}
