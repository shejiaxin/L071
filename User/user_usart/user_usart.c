#include "stm32l0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "include.h"
#include <stdlib.h>
#include <stdio.h>
#include "user_usart.h"

void LoRa_GetData(LoRaParameter LoRa)
{	         			           	                                
	printf("\r\nģ���ַ:0x%02X\r\n",LoRa.LoRa_AddrL + (LoRa.LoRa_AddrH << 8) );                //���������Ϣ
	switch(LoRa.LoRa_UartMode){                                                //�жϴ��ڲ���
		case LoRa_8N1 :	 printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8O1 :	 printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8E1 :	 printf("8����λ żУ�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		default:
			break;
	}
	switch(LoRa.LoRa_Baudrate){                                                //�жϴ��ڲ�����
		case LoRa_1200 : printf("������ 1200\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_2400 : printf("������ 2400\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_4800 : printf("������ 4800\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_9600 : printf("������ 9600\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_19200 : printf("������ 19200\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_38400 : printf("������ 38400\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_57600 : printf("������ 57600\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_115200: printf("������ 115200\r\n");                //���������Ϣ
						 break;                                          //����
		default:
			break;
	}
	
	switch(LoRa.LoRa_airvelocity){                                                //�жϿ�������                                      //����
		case LoRa_2_4_1:
		case LoRa_2_4_2:
		case LoRa_2_4 :  printf("�������� 2.4K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_4_8 :  printf("�������� 4.8K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_9_6 :  printf("�������� 9.6K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_19_2 : printf("�������� 19.2K\r\n");                //���������Ϣ
						 break;                                          //����
		case LoRa_38_4 : printf("�������� 19.2K\r\n");                //���������Ϣ
				 break;                                          //����
		case LoRa_62_5 : printf("�������� 19.2K\r\n");                //���������Ϣ
				 break;                                          //����
		default:
			break;
	}
	switch(LoRa.LoRa_TxPower){                                                //�жϷ��书��
		case LoRa_FEC_33DBM :  printf("���书�� 33dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_30DBM :  printf("���书�� 30dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_27DBM :  printf("���书�� 27dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_24DBM :  printf("���书�� 24dbm\r\n");          //���������Ϣ
							   break;                                    //����
	}
	printf("�ŵ�:0x%02X  ��ӦƵ��%dMHz\r\n",LoRa.LoRa_CH,410+(LoRa.LoRa_CH));   //���������Ϣ
		
	switch(LoRa.LoRa_DateMode){                                                //�жϴ���ģʽ
		case LoRa_TRANS : printf("͸������ģʽ\r\n");                 //���������Ϣ
						  break;                                         //����
		case LoRa_POINT : printf("���㴫��ģʽ\r\n");                 //���������Ϣ
						  break;                                         //����
	}
}


#define MAX_PAIRS 20

void parse_pair(char* pair,MqttParams *params) {
    char *key = strtok(pair, "=");
    if (key == NULL) {
        printf("Error: Missing key in pair\n");
        return;
    }
    char *value = strtok(NULL, "=");
    if (value == NULL) {
        printf("Error: Missing value for key: %s\n", key);
        return;
    }
    printf("Key: %s, Value: %s\n", key, value);
    if (strcmp(key, "Mqtt_ip") == 0) {
		strncpy(params->Mqtt_ip, value, sizeof(params->Mqtt_ip) - 1);
    } else if (strcmp(key, "Mqtt_port") == 0) {
        params->Mqtt_port = atoi(value);
    } else if (strcmp(key, "Mqtt_client_id") == 0) {
        strncpy(params->Mqtt_client_id, value, sizeof(params->Mqtt_client_id) - 1);
    } else if (strcmp(key, "Mqtt_user_name") == 0) {
        strncpy(params->Mqtt_user_name, value, sizeof(params->Mqtt_user_name) - 1);
    } else if (strcmp(key, "Mqtt_password") == 0) {
        strncpy(params->Mqtt_password, value, sizeof(params->Mqtt_password) - 1);
    } else if (strcmp(key, "Sub_Topics") == 0) {
        strncpy(params->Sub_Topics, value, sizeof(params->Sub_Topics) - 1);
    } else if (strcmp(key, "Sub_Topics1") == 0) {
        strncpy(params->Sub_Topics1, value, sizeof(params->Sub_Topics1) - 1);
    }
	else;
	
}

int parsecmd(char *data) {
    char *pairs[MAX_PAIRS];
    int pair_count = 0;

    char *pair = strtok(data, ",");
    while (pair != NULL && pair_count < MAX_PAIRS) {
        pairs[pair_count++] = pair;
        pair = strtok(NULL, ",");
    }

    for (int i = 0; i < pair_count; i++) {
        parse_pair(pairs[i],&Mqtt_params);
    }
		if(		 Mqtt_params.Mqtt_ip[0] != '\0' &&
           Mqtt_params.Mqtt_port != 0 &&
           Mqtt_params.Mqtt_client_id[0] != '\0' &&
           Mqtt_params.Mqtt_user_name[0] != '\0' &&
           Mqtt_params.Mqtt_password[0] != '\0' &&
           Mqtt_params.Sub_Topics[0] != '\0' &&
           Mqtt_params.Sub_Topics1[0] != '\0'){
					mcu_eeprom_write(41, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
					return 1;
				}
		else
					return 0;
}

/*-------------------------------------------------*/
/*��������debug����1�����¼�                            */
/*��  ����data ������                              */
/*��  ����datalen �����ݳ���                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U1PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
	/*----------------------------------------------*/
	/*           ��������LoRaģ�鷵������           */
	/*----------------------------------------------*/
	//Modbus_CRC16(data, datalen);
		switch (data[0]){
			case 0xE0:
				Connect_State = data[1];
				mcu_eeprom_write(0,(uint8_t *)&Connect_State,1);
				if(Connect_State == 1)
					printf("Connect_State = 4G");	
				if(Connect_State == 2)				
					printf("Connect_State = Lora");		
				printf("NVIC_SystemReset\r\n");
				NVIC_SystemReset();                           //����
				break;
			case 0xE1:
				LoRaNet.LoRa_AddrH = data[1];
				LoRaNet.LoRa_AddrL = data[2];
				switch(data[3]){                                                
					case 0:
						LoRaNet.LoRa_airvelocity = LoRa_2_4;
						break;  
					case 1:
						LoRaNet.LoRa_airvelocity = LoRa_4_8;
						break;  
					case 2 :  
						LoRaNet.LoRa_airvelocity = LoRa_9_6;
					  break;                                          
					case 3 :  
						LoRaNet.LoRa_airvelocity = LoRa_19_2;
						break;                                          
					case 4 :  
						LoRaNet.LoRa_airvelocity = LoRa_38_4;
					  break;                                          
					case 5 :
						LoRaNet.LoRa_airvelocity = LoRa_62_5;
						break;                                                                                                               
					default:
						break;
				}	
				LoRaNet.LoRa_CH = data[4];
				
				LoRaNet.LoRa_Baudrate = LoRa_9600;
				mcu_eeprom_write(5,(uint8_t *)&LoRaNet,sizeof(LoRaNet));	
				LoRa_Init();
				LoRa_MODE0;
				HAL_Delay(200); 
				NVIC_SystemReset();                           //����
				break;
			case 0xE2:
				LoRa_GetData(LoRaNet);
//				LoRa_MODE2; 
//				HAL_Delay(200); 	
//			  HAL_UART_Transmit(&huart2, cmd, 3, 0xFF);//��������
				break;
			case 'M':
				/*Mqtt_ip=81.70.28.130,Mqtt_port=1883,Mqtt_client_id=stm32,Mqtt_user_name=pub,Mqtt_password=Publish123456,Sub_Topics=P000005/D000476/report,Sub_Topics1=P000005/D000476/command*/
				if(parsecmd((char *)data))
						printf("\r\nMqtt�������óɹ�\r\n")	;
				else
						printf("\r\nMqtt��������ʧ��\r\n")	;
				break;
			case 0xB1:
				Motor_OFF
				break;
			case 0xB2:
				Motor_brake
				break;
			case 0xB3:
				Motor_ON
				break;
			case 0xE6:
				break;
			case 0xE7:
				break;
			default:
				break;
		}
		if(strcmp((char *)data,"BD") ==0 || strstr((char *)data,"BD")){
			Motor_BD_State = 0;
			mcu_eeprom_write(20,(uint8_t *)&Motor_BD_State,1);
			Motor_BD();
		
		}

		
//		printf("��������LoRaģ�鷵������\r\n");           //���������Ϣ
//		LoRa_Get(data);                                      //�������õĲ���
//		LoRa_MODE0;                                          //�л���ģʽ0
//		HAL_Delay(500);                                      //��ʱ	
//		printf("\r\n\r\n�����񰴼��ɷ�������\r\n\r\n");   //���������ʾ��Ϣ	
	
}

