#include "stm32l0xx_hal.h"    //包含需要的头文件
#include "include.h"
#include <stdlib.h>
#include <stdio.h>
#include "user_usart.h"

void LoRa_GetData(LoRaParameter LoRa)
{	         			           	                                
	printf("\r\n模块地址:0x%02X\r\n",LoRa.LoRa_AddrL + (LoRa.LoRa_AddrH << 8) );                //串口输出信息
	switch(LoRa.LoRa_UartMode){                                                //判断串口参数
		case LoRa_8N1 :	 printf("8数据位 无校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8O1 :	 printf("8数据位 奇校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8E1 :	 printf("8数据位 偶校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		default:
			break;
	}
	switch(LoRa.LoRa_Baudrate){                                                //判断串口波特率
		case LoRa_1200 : printf("波特率 1200\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_2400 : printf("波特率 2400\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_4800 : printf("波特率 4800\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_9600 : printf("波特率 9600\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_19200 : printf("波特率 19200\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_38400 : printf("波特率 38400\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_57600 : printf("波特率 57600\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_115200: printf("波特率 115200\r\n");                //串口输出信息
						 break;                                          //跳出
		default:
			break;
	}
	
	switch(LoRa.LoRa_airvelocity){                                                //判断空中速率                                      //跳出
		case LoRa_2_4_1:
		case LoRa_2_4_2:
		case LoRa_2_4 :  printf("空中速率 2.4K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_4_8 :  printf("空中速率 4.8K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_9_6 :  printf("空中速率 9.6K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_19_2 : printf("空中速率 19.2K\r\n");                //串口输出信息
						 break;                                          //跳出
		case LoRa_38_4 : printf("空中速率 19.2K\r\n");                //串口输出信息
				 break;                                          //跳出
		case LoRa_62_5 : printf("空中速率 19.2K\r\n");                //串口输出信息
				 break;                                          //跳出
		default:
			break;
	}
	switch(LoRa.LoRa_TxPower){                                                //判断发射功率
		case LoRa_FEC_33DBM :  printf("发射功率 33dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_30DBM :  printf("发射功率 30dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_27DBM :  printf("发射功率 27dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_24DBM :  printf("发射功率 24dbm\r\n");          //串口输出信息
							   break;                                    //跳出
	}
	printf("信道:0x%02X  对应频率%dMHz\r\n",LoRa.LoRa_CH,410+(LoRa.LoRa_CH));   //串口输出信息
		
	switch(LoRa.LoRa_DateMode){                                                //判断传输模式
		case LoRa_TRANS : printf("透明传输模式\r\n");                 //串口输出信息
						  break;                                         //跳出
		case LoRa_POINT : printf("定点传输模式\r\n");                 //串口输出信息
						  break;                                         //跳出
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
/*函数名：debug串口1被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U1PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
	/*----------------------------------------------*/
	/*           接收设置LoRa模块返回数据           */
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
				NVIC_SystemReset();                           //重启
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
				NVIC_SystemReset();                           //重启
				break;
			case 0xE2:
				LoRa_GetData(LoRaNet);
//				LoRa_MODE2; 
//				HAL_Delay(200); 	
//			  HAL_UART_Transmit(&huart2, cmd, 3, 0xFF);//发送数据
				break;
			case 'M':
				/*Mqtt_ip=81.70.28.130,Mqtt_port=1883,Mqtt_client_id=stm32,Mqtt_user_name=pub,Mqtt_password=Publish123456,Sub_Topics=P000005/D000476/report,Sub_Topics1=P000005/D000476/command*/
				if(parsecmd((char *)data))
						printf("\r\nMqtt数据设置成功\r\n")	;
				else
						printf("\r\nMqtt数据设置失败\r\n")	;
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

		
//		printf("接收设置LoRa模块返回数据\r\n");           //串口输出信息
//		LoRa_Get(data);                                      //分析设置的参数
//		LoRa_MODE0;                                          //切换到模式0
//		HAL_Delay(500);                                      //延时	
//		printf("\r\n\r\n按捕获按键可发送数据\r\n\r\n");   //串口输出提示信息	
	
}

