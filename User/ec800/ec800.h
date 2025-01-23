#ifndef __EC800_H
#define __EC800_H
#include "stm32l0xx_hal.h"  

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

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
	///////////�������·��Ŀ���ָ��////////////////
	uint8_t Switch_Type1;			//�����ͷ���״̬

	uint8_t Switch_Type2;			//�����ͷ���״̬
	
	uint8_t	Double_Type;			//˫ͨ����״̬

	uint8_t Butterfly_Type;	//���η�״̬

	uint8_t motor_Type;     //�������״̬
	
	/*-------------------------------------*/
	
	///////////ʵʱ��ȡ�ķ���״̬////////////////
	uint8_t RW_Switch_Type1;			//�����ͷ���״̬
	
	uint8_t RW_Switch_Type2;			//�����ͷ���״̬
	
	uint8_t	RW_Double_Type;			//˫ͨ����״̬

	uint8_t RW_Butterfly_Type;	//���η�״̬

	uint8_t RW_motor_Type;     //�������״̬	
	/*-------------------------------------*/
	
	uint8_t control_state; //���޿���ָ���·������·�Ϊ1��ִ����Ϊ0
	
	uint8_t control_report_state; //��������ϱ�
	
	uint32_t SysTick_10S;
	uint8_t SysTick_10S_start;
	
	uint16_t motor_0_val;   //��ʱ�� -90���ADC�궨��ѹ
	uint16_t motor_90_val_2t;  // 0���2tADC�궨��ѹ
	uint16_t motor_90_val_4t;  // 0���2tADC�궨��ѹ
	uint16_t motor_180_val; //˳ʱ�� -90���ADC�궨��ѹ
	
  uint16_t adc;
	uint32_t Wake_time;	
	uint32_t uuid[3];
	char imei[16];
	int csq;
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


void U3PassiveEvent(uint8_t *data, uint16_t datalen);
void Secrecy_GetUID(uint32_t * pBuf);
void get_A1_data(char *data);
void get_A3_data(char *data);
void get_A4_data(char *data);
#endif /*__EC800_H*/


