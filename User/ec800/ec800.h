#ifndef __EC800_H
#define __EC800_H
#include "stm32l0xx_hal.h"  

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

#define main_buff_len 300

typedef enum
{
    AT_OK = 0,
    AT_ERR,
} at_result_t;

typedef struct
{
    _Bool (*check)(char *cmd, char *res, uint16_t timeout);
    void (*write)( uint8_t *str);
} at_t;

typedef struct
{
	uint8_t Switch_Type;			//开关型阀门状态

	uint8_t	Double_Type;			//双通阀门状态

	uint8_t Butterfly_Type;	//蝶形阀状态

	uint8_t motor_Type;     //电机阀门状态
	
	uint8_t control_state; //有无控制指令下发，有下发为1，执行完为0
	
	uint32_t SysTick_10S;
	uint8_t SysTick_10S_start;
	
	uint16_t motor_0_val;   //逆时针 -90°的ADC标定电压
	uint16_t motor_90_val;  // 0°的ADC标定电压
	uint16_t motor_180_val; //顺时针 -90°的ADC标定电压
	
  uint16_t adc;
	uint32_t Wake_time;	
	uint32_t uuid[3];
	char imei[16];
	char Time_Data[20];
	uint16_t lora_id;
	uint16_t lora_rssi;
} DATA;

extern DATA User_Data;
extern char main_buff1[main_buff_len];
extern uint16_t main_len;
extern volatile uint8_t EC800_State;


void EC20_Init(void);
void UART_Clear(void);
_Bool UART_WaitRecive(void);
at_result_t at_cmd(at_t *at,char *cmd, char *res, uint16_t time_out);
_Bool UART_SendCmd(char *cmd, char *res, uint16_t timeout);
void Usart_SendString(uint8_t *str);
void MQTT_PublishQs0(char *data,uint16_t size);  



void Secrecy_GetUID(uint32_t * pBuf);
void get_A1_data(char *data);
void get_A3_data(char *data);
void get_A4_data(char *data);
#endif /*__EC800_H*/


