/*
* for parse UART command 
* send feedback 
*/ 
#ifndef MSG_HANDLER_H_
#define MSG_HANDLER_H_

extern u8 BOARD_ID;
// bit 7~4: id/axis, bit 3~0: error id 
extern u8 ERR_REPORT_STA; 

//MAPPING *pERR_STR[]
#define ERR_MAX	0x0F//16
#define ERR_STATUS	0
#define STR_ERR_STATUS	"ERROR"
#define ERR_SERVO_FAULT_OUT	1
#define STR_ERR_SERVO_FAULT_OUT "FAULTOUT"

#if 1//debug
	#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
	#define DEBUG(format, ...) //printf (format, ##__VA_ARGS__)
#endif //debug

//----------msg string---------------------
#define MSG_SPILT "#"
#define MSG_PARAMETER	"@"

//#define STR_SET_SPEED	"SET_SPEED"

extern u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
extern u8 USART_RX_STA;       //接收状态标记
extern char TX_BUF[64];

void msg_process(void);

#endif //MSG_HANDLER_H_
