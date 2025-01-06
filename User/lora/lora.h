/*-------------------------------------------------*/
/*                                                 */
/*            ����LoRaģ�鹦�ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LORA_H
#define __LORA_H

#include "stm32l0xx_hal.h"
#include "rtc.h"
typedef struct{  
	unsigned char LoRa_AddrH;         //ģ���ַ���ֽ�
	unsigned char LoRa_AddrL;         //ģ���ַ���ֽ�
	unsigned char LoRa_UartMode;      //ģ�鴮�ڹ���ģʽ
	unsigned char LoRa_Baudrate;      //ģ�鴮�ڲ�����
	unsigned char LoRa_airvelocity;   //ģ���������
	unsigned char LoRa_CH;            //ģ���ŵ�
	unsigned char LoRa_DateMode;      //ģ�����ݴ���ģʽ
	unsigned char LoRa_IOMode;        //ģ��TX AUXģʽ
	unsigned char LoRa_WakeTime;      //ģ�黽��ʱ��
	unsigned char LoRa_FECMode;       //ģ��FEC����
	unsigned char LoRa_TxPower;       //ģ�鷢�书��
}LoRaParameter;

#define LoRa_MODE0  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET) //LoRaģ�� ģʽ0
#define LoRa_MODE1  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET)   //LoRaģ�� ģʽ1
#define LoRa_MODE2  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET)   //LoRaģ�� ģʽ2
#define LoRa_MODE3  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET)     //LoRaģ�� ģʽ3

#define LoRa_AUX          HAL_GPIO_ReadPin(LORA_AUX_GPIO_Port,  LORA_AUX_Pin)                  //��ȡ��ƽ״̬,�ж�ģ��״̬
#define LoRa_PowerON      HAL_GPIO_WritePin(E22_ON_OFF_GPIO_Port, E22_ON_OFF_Pin, GPIO_PIN_SET)    //���ƹ���,�ߵ�ƽON
#define LoRa_PowerOFF     HAL_GPIO_WritePin(E22_ON_OFF_GPIO_Port, E22_ON_OFF_Pin, GPIO_PIN_RESET)  //���ƹ���,�͵�ƽOFF
      
#define LoRa_TxData       u2_TxData  

#define LoRa_8N1          0x00   //ģ�鴮�ڲ��� 8����λ ��У�� 1ֹͣλ
#define LoRa_8O1          0x40   //ģ�鴮�ڲ��� 8����λ ��У�� 1ֹͣλ
#define LoRa_8E1          0x80   //ģ�鴮�ڲ��� 8����λ żУ�� 1ֹͣλ

#define LoRa_1200         0x00   //ģ�鴮�� ������ 1200
#define LoRa_2400         0x20   //ģ�鴮�� ������ 2400
#define LoRa_4800         0x40   //ģ�鴮�� ������ 4800
#define LoRa_9600         0x60   //ģ�鴮�� ������ 9600
#define LoRa_19200        0x80   //ģ�鴮�� ������ 19200
#define LoRa_38400        0xa0   //ģ�鴮�� ������ 38400
#define LoRa_57600        0xc0   //ģ�鴮�� ������ 57600
#define LoRa_115200       0xe0   //ģ�鴮�� ������ 115200

#define LoRa_2_4          0x00   //ģ��������� 2.4K
#define LoRa_2_4_1        0x01   //ģ��������� 2.4K
#define LoRa_2_4_2        0x02   //ģ��������� 2.4K

#define LoRa_4_8          0x03   //ģ��������� 4.8K
#define LoRa_9_6          0x04   //ģ��������� 9.6K
#define LoRa_19_2         0x05   //ģ��������� 19.2K
#define LoRa_38_4         0x06   //ģ��������� 38.4K
#define LoRa_62_5         0x07   //ģ��������� 62.5K

#define LoRa_CH0          0x00   //ģ���ŵ� ��ӦƵ��410MHz
#define LoRa_CH1          0x01   //ģ���ŵ� ��ӦƵ��411MHz
#define LoRa_CH2          0x02   //ģ���ŵ� ��ӦƵ��412MHz
#define LoRa_CH3          0x03   //ģ���ŵ� ��ӦƵ��413MHz
#define LoRa_CH4          0x04   //ģ���ŵ� ��ӦƵ��414MHz
#define LoRa_CH5          0x05   //ģ���ŵ� ��ӦƵ��415MHz
#define LoRa_CH6          0x06   //ģ���ŵ� ��ӦƵ��416MHz
#define LoRa_CH7          0x07   //ģ���ŵ� ��ӦƵ��417MHz
#define LoRa_CH8          0x08   //ģ���ŵ� ��ӦƵ��418MHz
#define LoRa_CH9          0x09   //ģ���ŵ� ��ӦƵ��419MHz
#define LoRa_CH10         0x0A   //ģ���ŵ� ��ӦƵ��420MHz
#define LoRa_CH11         0x0B   //ģ���ŵ� ��ӦƵ��421MHz
#define LoRa_CH12         0x0C   //ģ���ŵ� ��ӦƵ��422MHz
#define LoRa_CH13         0x0D   //ģ���ŵ� ��ӦƵ��423MHz
#define LoRa_CH14         0x0E   //ģ���ŵ� ��ӦƵ��424MHz
#define LoRa_CH15         0x0F   //ģ���ŵ� ��ӦƵ��425MHz
#define LoRa_CH16         0x10   //ģ���ŵ� ��ӦƵ��426MHz
#define LoRa_CH17         0x11   //ģ���ŵ� ��ӦƵ��427MHz
#define LoRa_CH18         0x12   //ģ���ŵ� ��ӦƵ��428MHz
#define LoRa_CH19         0x13   //ģ���ŵ� ��ӦƵ��429MHz
#define LoRa_CH20         0x14   //ģ���ŵ� ��ӦƵ��430MHz
#define LoRa_CH21         0x15   //ģ���ŵ� ��ӦƵ��431MHz
#define LoRa_CH22         0x16   //ģ���ŵ� ��ӦƵ��432MHz
#define LoRa_CH23         0x17   //ģ���ŵ� ��ӦƵ��433MHz
#define LoRa_CH24         0x18   //ģ���ŵ� ��ӦƵ��434MHz
#define LoRa_CH25         0x19   //ģ���ŵ� ��ӦƵ��435MHz
#define LoRa_CH26         0x1A   //ģ���ŵ� ��ӦƵ��436MHz
#define LoRa_CH27         0x1B   //ģ���ŵ� ��ӦƵ��437MHz
#define LoRa_CH28         0x1C   //ģ���ŵ� ��ӦƵ��438MHz
#define LoRa_CH29         0x1D   //ģ���ŵ� ��ӦƵ��439MHz
#define LoRa_CH30         0x1E   //ģ���ŵ� ��ӦƵ��440MHz
#define LoRa_CH31         0x1F   //ģ���ŵ� ��ӦƵ��441MHz

#define LoRa_TRANS        0x00   //ģ��͸������
#define LoRa_POINT        0x20   //ģ�鶨�㴫��

#define LoRa_OD           0x00   //ģ��TX AUX��©
#define LoRa_UP           0x40   //ģ��TX AUX����

#define LoRa_250MS         0x00   //ģ�黽��ʱ��  250����   ��ģʽ1��2��Ч
#define LoRa_500MS         0x08   //ģ�黽��ʱ��  500����   ��ģʽ1��2��Ч
#define LoRa_750MS         0x10   //ģ�黽��ʱ��  750����   ��ģʽ1��2��Ч
#define LoRa_1000MS        0x18   //ģ�黽��ʱ��  1000����  ��ģʽ1��2��Ч
#define LoRa_1250MS        0x20   //ģ�黽��ʱ��  1250����  ��ģʽ1��2��Ч
#define LoRa_1500MS        0x28   //ģ�黽��ʱ��  1500����  ��ģʽ1��2��Ч
#define LoRa_1750MS        0x30   //ģ�黽��ʱ��  1750����  ��ģʽ1��2��Ч
#define LoRa_2000MS        0x38   //ģ�黽��ʱ��  2000����  ��ģʽ1��2��Ч

#define LoRa_FEC_OFF       0x00   //�ر�FEC
#define LoRa_FEC_ON        0x04   //����FEC

#define LoRa_FEC_33DBM     0x00   //���书��33dbm
#define LoRa_FEC_30DBM     0x01   //���书��30dbm
#define LoRa_FEC_27DBM     0x02   //���书��27dbm
#define LoRa_FEC_24DBM     0x03   //���书��24dbm


extern volatile uint8_t Connect_State;
extern LoRaParameter LoRaNet;
void LoRa_GPIO_Init(void);       				//������������ʼ��ģ���IO 
uint8_t LoRa_Reset(uint16_t timeout);     				//����������LoRa��λ����
void LoRa_Init(void);         				    //������������ʼ��ģ��
void LoRa_Set(void);                            //����������LoRa���ù�������
void LoRa_Get(uint8_t *data);                   //����������LoRa��ѯ��������    
void U2PassiveEvent(uint8_t *data, uint16_t datalen);       //��������������2�����¼�   

static void lora_get_A1_data(char *data);
static void lora_get_A3_data(char *data);
static void lora_get_A4_data(char *data);

#endif


