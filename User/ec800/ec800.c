 #include "usart.h"            
#include "ec800.h"	         
#include "include.h"
#include "ec800.h"
#include "rtc.h"
#include "adc.h"
#include<stdlib.h> 
#include "rtc.h"


volatile uint8_t EC800_State=0;


//物联网平台参数
//char Mqtt_ip[55]="81.70.28.130";					  //IP地址
//int  Mqtt_port = 1883;											//(MOTT服务器)端口号28
//char Mqtt_client_id[20] = "stm32";	
//char Mqtt_user_name[20] = "pub";						//用户名
//char Mqtt_password[50] = "Publish123456";		//密码
//char Sub_Topics[50]="P000005/D000476/report";
//char Sub_Topics1[50]="P000005/D000476/command";

MqttParams Mqtt_params = {
    .Mqtt_ip = "81.70.28.130",
    .Mqtt_port = 1883,
    .Mqtt_client_id = "stm32",
    .Mqtt_user_name = "pub",
    .Mqtt_password = "Publish123456",
    .Sub_Topics = "P000005/D000476/report",
    .Sub_Topics1 = "P000005/D000476/command"
};

char main_buff[main_buff_len];
char main_buff1[main_buff_len];
uint16_t main_len;

DATA User_Data;

#define COMMAND_PREFIX "+QMTRECV: 0,0,"    //接收的服务器数据头
#define MAX_COMMANDS 10          // 最多存储10条指令
Command commands[MAX_COMMANDS]; 
uint8_t command_count = 0;

void UART_Clear(void)
{
	memset(LPUART1_Data, 0, sizeof(LPUART1_Data));
	LPUART1Cnt = 0;
}

/**
 *@breief	发送命令
 *@param	cmd命令
			res需要检查返回指令
 *@reval	0-成功	1-失败
 *@notice	None
 */
_Bool UART_SendCmd(char *cmd, char *res, uint16_t timeout)
{	
	uint16_t timeOut = timeout;
  printf("#####CMD:%s#####\r\n",cmd);
	while(timeOut--)
	{
		if(UART_WaitRecive() == REV_OK)							
		{
		
      printf("*****nlog:%s*****\r\n",LPUART1_Data);	
			
			if(strstr((const char *)LPUART1_Data, res) != NULL)		
			{
				if(strstr(cmd,"CGSN")){
					strncpy(User_Data.imei,(char *)LPUART1_Data+10, 15);				
				}
				if(strstr(cmd,"CSQ")){
					char *csqPos = strstr((char *)LPUART1_Data, "+CSQ:");
						if(csqPos != NULL) {
								sscanf(csqPos, "+CSQ: %d", &User_Data.csq);
						}
				}
				if(strstr(cmd,"QCCID")){
					strncpy(User_Data.ICCID,(char *)LPUART1_Data+10, 20);					
				}
				if(strstr(cmd,"QLTS")){
					strncpy(User_Data.SIM_Time,(char *)LPUART1_Data+10, 20);				
				}
				UART_Clear();									
				return 0;
			}
		}
		
		HAL_Delay(1);
	}	
  UART_Clear();	
	return 1;
}

void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&hlpuart1,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0'); 

}
_Bool UART_WaitRecive(void)
{
	//if(LPUART1Flag == 0) 							//如果接收计数器为0 则说明处于接受中
	if( LPUART1_RX_STA&0X8000){
		LPUART1_RX_STA = 0;
		return REV_OK;		
	}
	//if(LPUART1Flag == 1)					//和上次值相同 则说明接收完成
	else
	{
		//LPUART1Flag=0;								//清0接收计数			
		return REV_WAIT;								//返回接收标志
	}	
						//返回接收未完成
}

at_result_t at_cmd(at_t *at,char *cmd, char *res, uint16_t time_out)
{
    at->write((uint8_t *)cmd);
//    if (res == NULL)
//        return AT_OK;
    if (at->check(cmd, res, time_out) == 0)
    {
        return AT_OK;
    }
    return AT_ERR;
}

static at_t at = {
    UART_SendCmd,
    Usart_SendString
};

#define AT_CMD(a, b, c) at_cmd(&at, a, b, c)
		
void MQTT_PublishQs0(char *data,uint16_t size)   
{
	memset(main_buff,0,sizeof(main_buff));
	sprintf(main_buff," AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n",Mqtt_params.Sub_Topics,size);
	while(AT_CMD(main_buff,">",2000))
	HAL_Delay(1);
  HAL_UART_Transmit(&hlpuart1,(uint8_t *)data,size,1000);	
  if(!UART_SendCmd("publish_ACK","+QMTPUBEX: 0,0,0",200)){
    printf("publish successful\r\n");
  }else{
    printf("publish fail\r\n");
  }
  UART_Clear();
}

void EC20_Init(void)
{		
		E22_OFF;
		E22_PWR_OFF;
		E22_RST_OFF;
		HAL_Delay(500);
		E22_PWR_ON;
		HAL_Delay(800);
		E22_PWR_OFF;
		HAL_Delay(1000);
		UART_Clear();
		while(AT_CMD("AT\r\n","OK",1000))
		HAL_Delay(100);
		while(AT_CMD("AT+CGSN\r\n","OK",1000))  
	  HAL_Delay(100);
		while(AT_CMD("ATE0\r\n","OK",1000))
	  HAL_Delay(1000);
		while(AT_CMD("AT+CPIN?\r\n","READY",1000))
		HAL_Delay(1000);
		while(AT_CMD("AT+QCCID\r\n","OK",1000))
		HAL_Delay(1000);
		while(AT_CMD("AT+CGATT?\r\n","+CGATT: 1",1000))
		//while(AT_CMD("AT+CGATT?\r\n","TT: 1",1000))
    HAL_Delay(100);
		while(AT_CMD("AT+CREG?\r\n","0,1",2000)) 
		HAL_Delay(100);
		while(AT_CMD("AT+QLTS=2\r\n","OK",2000)) 
		HAL_Delay(100);
		set_date_and_time(User_Data.SIM_Time);	
		while(AT_CMD("AT+CSQ\r\n","OK",1000))  
	  HAL_Delay(100);
		while(AT_CMD("AT+QMTCFG=\"qmtping\",0,30\r\n","OK",1000))
		HAL_Delay(100);
		while(AT_CMD("AT+QMTCFG=\"recv/mode\",0,0,1\r\n","OK",1000)) 
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTOPEN=0,\"%s\",%d\r\n",Mqtt_params.Mqtt_ip,Mqtt_params.Mqtt_port);
		while(AT_CMD(main_buff,"+QMTOPEN: 0,0",1000))  
		HAL_Delay(100);
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",User_Data.imei,Mqtt_params.Mqtt_user_name,Mqtt_params.Mqtt_password);
		while(AT_CMD(main_buff,"+QMTCONN: 0,0,0",1000))  
		HAL_Delay(200);
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTSUB=0,1,\"%s\",0\r\n",Mqtt_params.Sub_Topics1);
		while(AT_CMD(main_buff,"+QMTSUB: 0,1,0,0",1000))
		HAL_Delay(100);
		EC800_State=1;
		printf("EC20 Init Finish\r\n");	
		HAL_Delay(100);
		RTC_Time();
		Get_Adc_Value();
		
		PWR12V_ON
		HAL_Delay(5000);
		read_SW();
		
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
		User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.RW_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
		GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
		MQTT_PublishQs0(main_buff1,main_len);	
  	UART_Clear();
		
		PWR12V_OFF
		HAL_Delay(100);
}

// 解析从start到end的字符串，并提取ID和原始指令
Command parseCommand(const char *start, const char *end) {
    Command result = {0};
    const char *id_start;
    const char *id_end;

    // 查找ID开始的位置
   id_start = strchr(start, 'A'); // 假设ID总是跟在'A'之后
    if (id_start != NULL) {
        id_start += 3; // 跳过'A'
        // 查找ID结束的位置
        id_end = strchr(id_start, ',');
        if (id_end != NULL && id_end < end) {
            // 提取ID
            size_t id_len = id_end - id_start;
            if (id_len < sizeof(result.id)) {
                strncpy(result.id, id_start, id_len);
                result.id[id_len] = '\0'; // 确保字符串以null结束
            }
        }
    }

    // 提取整个原始指令字符串
    size_t command_len = end - start;
    if (command_len < sizeof(result.raw_command)) {
        strncpy(result.raw_command, start, command_len);
        result.raw_command[command_len] = '\0'; // 确保字符串以null结束
    }

    return result;
}

// 从buffer中解析所有指令并存储符合条件的指令
void processCommands(const char *buffer, Command commands[], uint8_t *command_count) {
    const char *command_start = buffer;
    const char *command_end;

    // 循环处理每个COMMAND_PREFIX指令
    while ((command_start = strstr(command_start, COMMAND_PREFIX)) != NULL) {
        // 移动指针到COMMAND_PREFIX之后
        command_start += strlen(COMMAND_PREFIX);
        // 查找指令的结束位置
        command_end = strstr(command_start, COMMAND_PREFIX);
        if (command_end == NULL) {
            // 如果没有找到下一个COMMAND_PREFIX，那么这是最后一个指令
            command_end = command_start + strlen(command_start);
        }

        // 解析指令并获取结果
        Command cmd = parseCommand(command_start, command_end);

        // 判断是否是自己的ID
        if (strcmp(cmd.id, (char *)User_Data.imei) == 0) {
            // 如果是自己的ID，将整个原始指令字符串存入commands数组
            if (*command_count < MAX_COMMANDS) {
                commands[*command_count] = cmd;
                (*command_count)++;
            }
        }
        // 移动到下一个指令的开始位置
        command_start = command_end;
    }
}

/*-------------------------------------------------*/
/*函数名：LPUART1被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U3PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
		printf("4GSever_数据 %s 长度%d\r\n",data,datalen);
//		if(strstr((char *)data,"+QMTRECV: 0,0")&&strstr((char *)data,(char *)User_Data.imei))
		processCommands((char *)data, commands, &command_count);
		// 检查是否有符合条件的指令
		if (command_count > 0) {
				printf("开始处理符合条件的指令:\n");
				for (int i = 0; i < command_count; i++) {
						// 假设这里是处理指令的逻辑
						printf("正在处理指令 %d: %s\n", i + 1, commands[i].raw_command);			
						if(strstr((char *)commands[i].raw_command,"A5")){
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							RTC_Time();					
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.RW_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);	
							PWR12V_OFF
							HAL_Delay(100);
						}
						else if(strstr((char *)data,"A1")){
							get_A1_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();
							Control(&User_Data,1);
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"B1,%s,OK,%d",User_Data.imei,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
						}
						else if(strstr((char *)data,"A2")){
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"B2,%s,OK,%d",User_Data.imei,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
							enter_stop_rtc_mode(User_Data.Wake_time);
						}
						else if(strstr((char *)data,"A3")){
							get_A3_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);			
							memset(main_buff1,0,sizeof(main_buff1));	
							main_len=sprintf(main_buff1,"B3,%s,OK,%d",User_Data.imei,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
						
						}
						else if(strstr((char *)data,"A4")){
							get_A4_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"B4,%s,OK,%d",User_Data.imei,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);

						}
						else if(strstr((char *)data,"A6")){				
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							memset(main_buff1,0,sizeof(main_buff1));		
							main_len=sprintf(main_buff1,"B6,%s,%.20s,OK,%d",User_Data.imei,User_Data.ICCID,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);

						}
						else if (strstr((char *)data, "A7") ) {
							get_A7_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();
							Control(&User_Data, 7); // 调用 Control，传入控制类型 7
							// 发送响应 B7,<实际状态>,OK
							memset(main_buff1, 0, sizeof(main_buff1));
							main_len = sprintf(main_buff1, "B7,%s,OK,%d" ,User_Data.imei, User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
						}
						else if(strstr((char *)data, "A8") ){
							get_A8_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();
							Control(&User_Data,8);
							memset(main_buff1, 0, sizeof(main_buff1));
							main_len = sprintf(main_buff1, "B8,%s,OK,%d", User_Data.imei, User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
							
						}
						else if(strstr((char *)data, "A9")) {
							get_A9_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;		
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();			
							Control(&User_Data,9);
							memset(main_buff1, 0, sizeof(main_buff1));
							main_len = sprintf(main_buff1, "B9,%s,OK,%d", User_Data.imei, User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);					
						}
						else if(strstr((char *)data, "AA")) {
							get_AA_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;		
							PWR12V_ON
							HAL_Delay(5000);
							read_SW();
							Control(&User_Data,10);
							memset(main_buff1, 0, sizeof(main_buff1));
							main_len = sprintf(main_buff1, "BA,%s,OK,%d", User_Data.imei, User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
						}
						else;			
	
						// 模拟处理完成后，删除该指令
						// 将后续指令前移
						for (int j = i; j < command_count - 1; j++) {
								commands[j] = commands[j + 1];
						}
						
						// 数组长度减一
						command_count--;

						// 调整i以重新检查当前位置
						i--;
				}
				printf("所有指令已处理完毕。\n");
		}
		
}

void Secrecy_GetUID(uint32_t * pBuf)
{
	
	pBuf[0] = HAL_GetUIDw0();;
	pBuf[1] = HAL_GetUIDw1();;
	pBuf[2] = HAL_GetUIDw2();;
	printf("pBuf[0]=%d\n",pBuf[0]);
	printf("pBuf[1]=%#x\n",pBuf[1]);
	printf("pBuf[2]=%#x\n",pBuf[2]);
}

void get_A1_data(char *data)
{
	char *token;
//	char *dataString;
	char dataString[100]; 
	uint8_t i=0,userdata=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	//dataString = token;
	// 使用逗号作为分隔符来提取数据
	
 if (token != NULL) {
			strncpy(dataString, token, sizeof(dataString) - 1);  // 复制字符串到本地数组
			dataString[sizeof(dataString) - 1] = '\0';          // 确保字符串以 '\0' 结束
	} else {
			printf("Error: No valid data string found!\n");
			return;
	}
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					userdata=atoi(token);
					User_Data.Switch_Type1=userdata;
					break;
				case 4:
					userdata=atoi(token);
					User_Data.Switch_Type2=userdata;
					break;
				case 5:
					userdata=atoi(token);			
					User_Data.Double_Type=userdata;
					break;
				case 6:
					userdata=atoi(token);					
					User_Data.Butterfly_Type=userdata;
					break;
				case 7:
					userdata=atoi(token);				
					User_Data.motor_Type=userdata;
					break;

				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}

void get_A3_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					User_Data.Wake_time=atoi(token);

					break;
			
				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}

void get_A4_data(char *data)
{
	char *token;
	char *date_token ;
	char *dataString;
	int year, month, day, hour, minute, second;
	 char date[20];
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;
	// 使用逗号作为分隔符来提取数据
  // 使用 strtok 函数拆分字符串
    token = strtok(dataString, ",");
    token = strtok(NULL, ",");
		token = strtok(NULL, ",");
		strcpy(User_Data.Time_Data, token);
		strcpy(date, token);
		printf("User_Data.Time_Data=%s\r\n",User_Data.Time_Data);
		// 提取年月日	
		date_token = strtok(date, "/-");
    year = atoi(date_token);
		sDate.Year=year;
    date_token = strtok(NULL, "/-");
    month = atoi(date_token);
		switch(month){
			case 1:
				sDate.Month = RTC_MONTH_JANUARY;
			break;	
			case 2:
				sDate.Month = RTC_MONTH_FEBRUARY;
			break;
			case 3:
				sDate.Month = RTC_MONTH_MARCH;
			break;
			case 4:
				sDate.Month = RTC_MONTH_APRIL;
			break;
			case 5:
				sDate.Month = RTC_MONTH_MAY;
			break;
			case 6:
				sDate.Month = RTC_MONTH_JUNE;
			break;
			case 7:
				sDate.Month = RTC_MONTH_JULY;
			break;
			case 8:
				sDate.Month = RTC_MONTH_AUGUST;
			break;
			case 9:
				sDate.Month = RTC_MONTH_SEPTEMBER;
			break;
			case 10:
				sDate.Month = RTC_MONTH_OCTOBER;
			break;
			case 11:
				sDate.Month = RTC_MONTH_NOVEMBER;
			break;
			case 12:
				sDate.Month = RTC_MONTH_DECEMBER;
			break;
			default:
			break;
		}
		date_token = strtok(NULL, "/-"); 
    day = atoi(date_token);
		sDate.Date=day;
    // 提取时分秒
    token = strtok(NULL, ":");
    hour = atoi(token);
		sTime.Hours=hour;
    token = strtok(NULL, ":");
    minute = atoi(token);
		sTime.Minutes = minute;
    token = strtok(NULL, ":");
    second = atoi(token);
		sTime.Seconds = second;
		 printf("Year: %d\n", year);
    printf("Month: %d\n", month);
    printf("Day: %d\n", day);
    printf("Hour: %d\n", hour);
    printf("Minute: %d\n", minute);
    printf("Second: %d\n", second);			
    sDate.WeekDay = 0;  //这里必须要设置星期，否则读取年份不对
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}
void get_A7_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					User_Data.Switch_Type1=atoi(token);

					break;
			
				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}

void get_A8_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					User_Data.Switch_Type2=atoi(token);

					break;
			
				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}
void get_A9_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					User_Data.Butterfly_Type=atoi(token);

					break;
			
				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}
void get_AA_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// 找到第四个双引号后的数据部分
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 3:
					User_Data.motor_Type=atoi(token);

					break;
			
				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
}

void set_date_and_time(char *data) {
      char *token;
    int year, month, day, hour, minute, second;
    RTC_DateTypeDef sDate;
    RTC_TimeTypeDef sTime;

    // 提取日期
    token = strtok(data, "/");
    year = atoi(token);
    sDate.Year = year-2000;

    token = strtok(NULL, "/");
    month = atoi(token);
    switch(month) {
        case 1: sDate.Month = RTC_MONTH_JANUARY; break;
        case 2: sDate.Month = RTC_MONTH_FEBRUARY; break;
        case 3: sDate.Month = RTC_MONTH_MARCH; break;
        case 4: sDate.Month = RTC_MONTH_APRIL; break;
        case 5: sDate.Month = RTC_MONTH_MAY; break;
        case 6: sDate.Month = RTC_MONTH_JUNE; break;
        case 7: sDate.Month = RTC_MONTH_JULY; break;
        case 8: sDate.Month = RTC_MONTH_AUGUST; break;
        case 9: sDate.Month = RTC_MONTH_SEPTEMBER; break;
        case 10: sDate.Month = RTC_MONTH_OCTOBER; break;
        case 11: sDate.Month = RTC_MONTH_NOVEMBER; break;
        case 12: sDate.Month = RTC_MONTH_DECEMBER; break;
        default: break;
    }

    token = strtok(NULL, ",");
    day = atoi(token);
    sDate.Date = day;

    // 提取时间
    token = strtok(NULL, ":");
    hour = atoi(token);
    sTime.Hours = hour;

    token = strtok(NULL, ":");
    minute = atoi(token);
    sTime.Minutes = minute;

    token = strtok(NULL, "");
    second = atoi(token);
    sTime.Seconds = second;

    printf("Year: %d\n", year);
    printf("Month: %d\n", month);
    printf("Day: %d\n", day);
    printf("Hour: %d\n", hour);
    printf("Minute: %d\n", minute);
    printf("Second: %d\n", second);
    sDate.WeekDay = 0;  //这里必须要设置星期，否则读取年份不对
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
  
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
}

	