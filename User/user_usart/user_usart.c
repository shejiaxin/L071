#include "stm32l0xx_hal.h"    //包含需要的头文件
#include "include.h"
#include <stdlib.h>
#include <stdio.h>
#include "user_usart.h"


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
	uint8_t cmd[3]={0xc1,0x00,0x07};
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
				//LoRaNet.LoRa_AddrH = data[1];
				LoRaNet.LoRa_AddrL = data[1];
				switch(data[2]){                                                
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
				LoRaNet.LoRa_CH = data[3];
				
				LoRaNet.LoRa_Baudrate = LoRa_9600;
				mcu_eeprom_write(5,(uint8_t *)&LoRaNet,sizeof(LoRaNet));	
				LoRa_Init();
				LoRa_MODE0;
				HAL_Delay(200); 
				NVIC_SystemReset();                           //重启
				break;
			case 0xE2:
				LoRa_MODE2; 
				HAL_Delay(200); 	
			  HAL_UART_Transmit(&huart2, cmd, 3, 0xFF);//发送数据
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

