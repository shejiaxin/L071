#include "user_usart.h"
#include "include.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "adc.h"

#define FIRMWARE_UPDATE_VAR_ADDR            0x20000000      /* 一定要和 APP 保持一致 */

volatile uint64_t update_flag __attribute__((at(FIRMWARE_UPDATE_VAR_ADDR), zero_init));

__WEAK
uint64_t Bootloader_GetUpdateFlag(void)
{
    return update_flag;
}

/**
 * @brief  设置固件更新标志位
 * @note   
 * @param[in]  flag: 固件更新标志位
 * @retval None
 */
__WEAK
void Bootloader_SetUpdateFlag(uint64_t flag)
{
    update_flag = flag;
}


void process_json_command(const char* json_data) {
    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        log_info("JSON parse error\n");
        return;
    }

    cJSON *type = cJSON_GetObjectItem(root, "type");
    if (type == NULL) {   //|| type->type != cJSON_Object
        log_info("Missing or invalid 'type'\n");
        cJSON_Delete(root);
        return;
    }

    if (strcmp(type->valuestring, "upgrade") == 0) {
        log_info("Handling upgrade command...\n");
				Bootloader_SetUpdateFlag(0xA5A5A5A5);
				HAL_NVIC_SystemReset();
        // 实现升级逻辑
    } else if (strcmp(type->valuestring, "status_query") == 0) {
        log_info("Handling status query command...\n");
				respondToStatusQuery() ;	
				
    }else if (strcmp(type->valuestring, "set_mqtt") == 0) {
        log_info("Handling set_sever_data command...\n");
				cJSON *item;
				// 解析IP地址
				item = cJSON_GetObjectItem(root, "ip");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Mqtt_ip, item->valuestring);
				}

				// 解析端口号
				item = cJSON_GetObjectItem(root, "port");
				if (item->type == cJSON_Number) {
						Mqtt_params.Mqtt_port = (uint16_t)item->valuedouble;
				}

				// 解析客户端ID
				item = cJSON_GetObjectItem(root, "client_id");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Mqtt_client_id, item->valuestring);
				}

				// 解析用户名
				item = cJSON_GetObjectItem(root, "user_name");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Mqtt_user_name, item->valuestring);
				}

				// 解析密码
				item = cJSON_GetObjectItem(root, "password");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Mqtt_password, item->valuestring);
				}

				// 解析订阅主题
				item = cJSON_GetObjectItem(root, "Topics");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Sub_Topics, item->valuestring);
				}

				// 解析第二个订阅主题
				item = cJSON_GetObjectItem(root, "Topics1");
				if (item->type == cJSON_String && (item->valuestring != NULL)) {
						strcpy(Mqtt_params.Sub_Topics1, item->valuestring);
				}
				mcu_eeprom_write(46, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
    }
		else if (strcmp(type->valuestring, "sever_data") == 0) {
        log_info("Handling sever_data command...\n");
			  respondToSever_data() ;
    }	else if (strcmp(type->valuestring, "motor_control") == 0) {
        log_info("Handling motor_control command...\n");
				cJSON *command = cJSON_GetObjectItem(root, "command");
        if (command->type == cJSON_String) {
						User_Data.control_state = 12;  //0-11外的数字会使电机不受服务器控制
//						PWR12V_ON
						if (strcmp(command->valuestring, "forward") == 0) {
								Motor_ON
						} else if (strcmp(command->valuestring, "reverse") == 0) {
								Motor_OFF
						}  else if (strcmp(command->valuestring, "stop") == 0) {
                Motor_brake
//								PWR12V_OFF
            }
					}
    } else if (strcmp(type->valuestring, "set_Hreat") == 0) {
        log_info("Handling set pressure range command...\n");
				cJSON *type = cJSON_GetObjectItem(root, "type");
				cJSON *command = cJSON_GetObjectItem(root, "command");
				if (type->valuestring && (strcmp(type->valuestring, "set_Hreat") == 0)) {
						User_Data.Heart_Time = command->valueint;
						mcu_eeprom_write(300,(uint8_t *)&User_Data.Heart_Time,4);
				}

    }else if (strcmp(type->valuestring, "set_pressure") == 0) {
        log_info("Handling set_Hreat range command...\n");
				cJSON *type = cJSON_GetObjectItem(root, "type");
				cJSON *command = cJSON_GetObjectItem(root, "command");
				if (type->valuestring && (strcmp(type->valuestring, "set_pressure") == 0)) {
						switch (command->valueint) {
								case 1:
										// 执行命令1的操作
										User_Data.Pressure_Range = 0;
										break;
								case 2:
										// 执行命令2的操作
										User_Data.Pressure_Range = 1;
										break;
								case 3:
										// 执行命令3的操作
										User_Data.Pressure_Range = 2;
										break;
								default:
										// 未知命令
										break;
						}
						mcu_eeprom_write(41,(uint8_t *)&User_Data.Pressure_Range,4);
				}

    } else if (strcmp(type->valuestring, "set_motor_angle") == 0) {
        log_info("Handling set motor angle command...\n");
				cJSON *type = cJSON_GetObjectItem(root, "type");
				cJSON *command = cJSON_GetObjectItem(root, "command");
				if (type->valuestring && (strcmp(type->valuestring, "set_motor_angle") == 0)) {
						switch (command->valueint) {

								case 1:
										// 执行命令1的操作
										User_Data.motor_0_val =ADC_Values[0];
										break;
								case 2:
										// 执行命令2的操作
										User_Data.motor_90_val_2t =ADC_Values[0];
										User_Data.motor_90_val_4t =ADC_Values[1];		
										break;
								case 3:
										// 执行命令3的操作
										User_Data.motor_180_val =ADC_Values[1];
										break;
								default:
										// 未知命令
										break;
						}
							Motor_BD_State = 1;
							mcu_eeprom_write(20,(uint8_t *)&Motor_BD_State,1);
							mcu_eeprom_write(21,(uint8_t *)&User_Data.motor_0_val,4);
							mcu_eeprom_write(26,(uint8_t *)&User_Data.motor_90_val_2t,4);
							mcu_eeprom_write(31,(uint8_t *)&User_Data.motor_90_val_4t,4);
							mcu_eeprom_write(36,(uint8_t *)&User_Data.motor_180_val,4);
							Motor_brake	
//							PWR12V_OFF
				}
				
    } else if (strcmp(type->valuestring, "motor_angle") == 0) {
        log_info("Handling get motor_angle command...\n");
				respondToMotorQuery();
				
    }else {
        log_info("Unknown command type: %s\n", type->valuestring);
    }

    cJSON_Delete(root);
}
uint8_t jsonData[500]={0};
// 串口队列处理任务
void User_Usart_QueueTask(void)
{
		if(buff_manage_struct_usart1.ReadLen>0)//有数据
		{
			log_info("Usart1BufferMemoryCopy;%s    buff_manage_struct_usart1.ReadLen:%d\r\n",Usart1BufferMemoryCopy,buff_manage_struct_usart1.ReadLen);
			if (Usart1BufferMemoryCopy[0] == 0xAA && Usart1BufferMemoryCopy[1] == 0x55 \
					&& Usart1BufferMemoryCopy[buff_manage_struct_usart1.ReadLen - 2] == 0x55 \
					&& Usart1BufferMemoryCopy[buff_manage_struct_usart1.ReadLen - 1] == 0xAA
				)
			{
					uint16_t len = (Usart1BufferMemoryCopy[2] << 8) | Usart1BufferMemoryCopy[3];  // 解析数据长度

					if (len + 6 <= buff_manage_struct_usart1.ReadLen) // 验证数据长度是否合理
					{
							uint16_t crc = (Usart1BufferMemoryCopy[4 + len] << 8) | Usart1BufferMemoryCopy[4 + len + 1];  // 校验码

							// 创建一个数组来存储 JSON 数据，确保只包含 JSON 部分
							//uint8_t jsonData[len+1];
							memcpy(jsonData, &Usart1BufferMemoryCopy[4], len);  // 从 Usart1BufferMemoryCopy 复制 len 字节的 JSON 数据到 jsonData
							jsonData[len] = '\0'; // 确保字符串正常终止
							// 校验数据
							if (crc == Modbus_CRC16(jsonData, len))  // 校验码检查
							{
								log_info("Data correct"); // 数据正确
								// 这里可以进一步处理 jsonData
 								process_json_command((char*)jsonData); // 处理 JSON 数据	
							}
							else
							{
									log_info("CRC mismatch"); // CRC 不匹配
							}
					}
					else
					{
							log_info("Invalid data length"); // 数据长度不合理
					}
			}
			else
			{
					log_info("Invalid start or end marker"); // 起始或结束标志不正确
			}
		}
}
#define MAX_JSON_SIZE 512 // 定义最大 JSON 字符串长度
static uint8_t sendJsonData(cJSON *json) {
		uint8_t res = 0;
		uint8_t packet[MAX_JSON_SIZE] = {0};
    char *jsonString = cJSON_PrintUnformatted(json);
    if (!jsonString) {
        log_info("Failed to print JSON.\n");
        return -1;
    }

    uint16_t jsonLength = strlen(jsonString);
    uint16_t packetLength = jsonLength + 8; // Total length including header, length, CRC, and tail

		packet[0] = 0xAA;
		packet[1] = 0x55;
		packet[2] = (jsonLength >> 8) & 0xFF; // Length high byte
		packet[3] = jsonLength & 0xFF;        // Length low byte
		memcpy(&packet[4], jsonString, jsonLength); // Copy JSON string
		uint16_t crc = Modbus_CRC16(&packet[4], jsonLength); // Calculate CRC on JSON data
		packet[4 + jsonLength] = (crc >> 8) & 0xFF;   // CRC high byte
		packet[5 + jsonLength] = crc & 0xFF;          // CRC low byte
		packet[6 + jsonLength] = 0x55;
		packet[7 + jsonLength] = 0xAA;

		// Send packet here (replace USART_Send with your actual function)
		//USART_Send(packet, packetLength);
		res = HAL_UART_Transmit(&huart1, packet, packetLength,0xff);

		memset(packet,0,packetLength);
    free(jsonString);
		return res;
}


void respondToStatusQuery(void) {
    // 假设以下函数是从硬件获取数据的实际实现
    int batteryLevel = User_Data.adc;//getBatteryLevel(); // 获取电量
    float pressure1 = User_Data.RW_Pressure1;//getPressureSensor1(); // 获取气压值1
    float pressure2 = User_Data.RW_Pressure2;//getPressureSensor2(); // 获取气压值2
    int valveAngle = User_Data.RW_motor_Type;//User_Data.Report_motor_Type;//getValveAngle(); // 获取阀门角度
    int adcValue1 = ADC_Values[0];//getAdcValue1(); // 获取ADC值1
    int adcValue2 = ADC_Values[1];//getAdcValue2(); // 获取ADC值2
    int networkStatus = EC800_State;//get4GNetworkStatus(); // 获取4G网络状态
		uint8_t state_12v = HAL_GPIO_ReadPin(ON_OFF_12V_GPIO_Port,ON_OFF_12V_Pin);
		uint8_t Pressure_Range = User_Data.Pressure_Range;
		uint8_t Heart = User_Data.Heart_Time;
	
    // 创建JSON对象
    cJSON *response = cJSON_CreateObject();
	  if (response == NULL) {
        // 错误处理：无法创建对象
        return;
    }
    cJSON_AddStringToObject(response, "type", "status_response");
    cJSON_AddNumberToObject(response, "battery", batteryLevel);
    cJSON_AddNumberToObject(response, "pressure1", pressure1);
    cJSON_AddNumberToObject(response, "pressure2", pressure2);
    cJSON_AddNumberToObject(response, "valve_angle", valveAngle);
    cJSON_AddNumberToObject(response, "adc1", adcValue1);
    cJSON_AddNumberToObject(response, "adc2", adcValue2);
		cJSON_AddNumberToObject(response, "12V", state_12v);
    cJSON_AddNumberToObject(response, "network_status", networkStatus);
		cJSON_AddNumberToObject(response, "Pressure_Range", Pressure_Range);
		cJSON_AddNumberToObject(response, "Heart", Heart);
    // 发送JSON数据
    sendJsonData(response);
    cJSON_Delete(response);
}

void respondToSever_data(void) {

//    // 创建JSON对象
    cJSON *response = cJSON_CreateObject();
    if (response == NULL) {
        // 错误处理：无法创建对象
        return;
    }
    cJSON_AddStringToObject(response, "type", "sever_response");
    cJSON_AddStringToObject(response, "ip", Mqtt_params.Mqtt_ip);
	  cJSON_AddNumberToObject(response, "port", Mqtt_params.Mqtt_port);
    cJSON_AddStringToObject(response, "client_id", Mqtt_params.Mqtt_client_id);
    cJSON_AddStringToObject(response, "user_name", Mqtt_params.Mqtt_user_name);
    cJSON_AddStringToObject(response, "password", Mqtt_params.Mqtt_password);
    cJSON_AddStringToObject(response, "Sub_Topics", Mqtt_params.Sub_Topics);
    cJSON_AddStringToObject(response, "Sub_Topics1", Mqtt_params.Sub_Topics1);

    // 发送JSON数据
    sendJsonData(response);
		
    cJSON_Delete(response);
}
void respondToMotorQuery(void) {
    // 创建JSON对象
    cJSON *response = cJSON_CreateObject();
	  if (response == NULL) {
        // 错误处理：无法创建对象
        return;
    }
    cJSON_AddStringToObject(response, "type", "Motor_response");
    cJSON_AddNumberToObject(response, "motor_0_val", User_Data.motor_0_val);
    cJSON_AddNumberToObject(response, "motor_90_val_2t", User_Data.motor_90_val_2t);
    cJSON_AddNumberToObject(response, "motor_90_val_4t", User_Data.motor_90_val_4t);
    cJSON_AddNumberToObject(response, "motor_180_val", User_Data.motor_180_val);

    // 发送JSON数据
    sendJsonData(response);
    cJSON_Delete(response);
}
