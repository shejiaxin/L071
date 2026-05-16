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
    .Mqtt_ip = "43.143.179.206",
    .Mqtt_port = 11883,
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
static at_t at = {
    UART_SendCmd,
    Usart_SendString
};

#define AT_CMD(a, b, c) at_cmd(&at, a, b, c)

char type[3], boardID[20];
void parseReceivedData(char* receivedData) {	
    // 去除开头的换行符\r\n
    if (strncmp(receivedData, "\r\n", 2) == 0) {
        receivedData += 2;
    }

    // 定位到第一个引号对内的内容，即命令类型
    char* firstQuote = strchr(receivedData, '\"');
    if (!firstQuote) {
        log_info("Debug: First quote not found\n");
        return;
    }
    firstQuote++;  // 跳过第一个引号

    char* secondQuote = strchr(firstQuote, '\"');
    if (!secondQuote) {
        log_info("Debug: Second quote not found\n");
        return;
    }
    *secondQuote = '\0';  // 将第二个引号替换为 null 字符

    // 第一个引号对之后是数据长度
    char* commaAfterSecondQuote = strchr(secondQuote + 1, ',');
    if (!commaAfterSecondQuote) {
        log_info("Debug: Comma after second quote not found\n");
        return;
    }
    commaAfterSecondQuote++;
    int dataLength = atoi(commaAfterSecondQuote);

    // 定位到第二个引号对内的内容，即命令数据
    char* thirdQuote = strchr(commaAfterSecondQuote, '\"');
    if (!thirdQuote) {
        log_info("Debug: Third quote not found\n");
        return;
    }
    thirdQuote++;  // 跳过第三个引号

    char* fourthQuote = strchr(thirdQuote, '\"');
    if (!fourthQuote) {
        log_info("Debug: Fourth quote not found\n");
        return;
    }
    *fourthQuote = '\0';  // 将第四个引号替换为 null 字符
		// 输出提取的数据
    log_info("Data length: %d\n", dataLength);
    log_info("Command data: %s\n", thirdQuote);
		sscanf(thirdQuote, "%[^,],%[^,]", type, boardID);
		if (strcmp(boardID, (char *)User_Data.imei) != 0) {
				log_info("Board ID does not match the specific ID %s. Received ID: %s\n", User_Data.imei, boardID); 
        return;  // 如果不匹配，直接返回
		}
		if (type[0] == 'A' && type[1] == '1') {
				get_A1_data((char *)thirdQuote);
				AT_CMD("AT+CSQ\r\n","OK",1000);
				read_SW();
				Control(&User_Data,1);
				memset(main_buff1,0,sizeof(main_buff1));
				main_len=sprintf(main_buff1,"B1,%s,OK,%d",User_Data.imei,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);
        
    } else if (type[0] == 'A' && type[1] == '2'){
				AT_CMD("AT+CSQ\r\n","OK",1000);
				memset(main_buff1,0,sizeof(main_buff1));
				main_len=sprintf(main_buff1,"B2,%s,OK,%d",User_Data.imei,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);
				enter_stop_rtc_mode(User_Data.Wake_time);
    } else if (type[0] == 'A' && type[1] == '3') {
				get_A3_data((char *)thirdQuote);
				AT_CMD("AT+CSQ\r\n","OK",1000);			
				memset(main_buff1,0,sizeof(main_buff1));	
				main_len=sprintf(main_buff1,"B3,%s,OK,%d",User_Data.imei,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);

    } else if (type[0] == 'A' && type[1] == '4'){
				get_A4_data((char *)thirdQuote);
				AT_CMD("AT+CSQ\r\n","OK",1000);
				memset(main_buff1,0,sizeof(main_buff1));
				main_len=sprintf(main_buff1,"B4,%s,OK,%d",User_Data.imei,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);
    } else if (type[0] == 'A' && type[1] == '5') {
				AT_CMD("AT+CSQ\r\n","OK",1000);
				RTC_Time();					
				read_SW();
				memset(main_buff1,0,sizeof(main_buff1));
				main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
				User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
				GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);	
    }
		else if(type[0] == 'A' && type[1] == '6'){				
				AT_CMD("AT+CSQ\r\n","OK",1000);
				memset(main_buff1,0,sizeof(main_buff1));		
				main_len=sprintf(main_buff1,"B6,%s,%.20s,OK,%d",User_Data.imei,User_Data.ICCID,User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);
		}
		else if (type[0] == 'A' && type[1] == '7') {
				get_A7_data((char *)thirdQuote);
				read_SW();
				Control(&User_Data, 7); // 调用 Control，传入控制类型 7
				// 发送响应 B7,<实际状态>,OK
				memset(main_buff1, 0, sizeof(main_buff1));
				main_len = sprintf(main_buff1, "B7,%s,OK,%d" ,User_Data.imei, User_Data.csq);
				MQTT_PublishQs0(main_buff1,main_len);
		}
		else if(type[0] == 'A' && type[1] == '8'){
			get_A8_data((char *)thirdQuote);
			read_SW();
			Control(&User_Data,8);
			memset(main_buff1, 0, sizeof(main_buff1));
			main_len = sprintf(main_buff1, "B8,%s,OK,%d", User_Data.imei, User_Data.csq);
			MQTT_PublishQs0(main_buff1,main_len);		
		}		
		else if(type[0] == 'A' && type[1] == '9') {
			get_A9_data((char *)thirdQuote);
			read_SW();			
			Control(&User_Data,9);
			memset(main_buff1, 0, sizeof(main_buff1));
			main_len = sprintf(main_buff1, "B9,%s,OK,%d", User_Data.imei, User_Data.csq);
			MQTT_PublishQs0(main_buff1,main_len);					
		}
		else if(type[0] == 'A' && type[1] == 'A'){
			get_AA_data((char *)thirdQuote);
			read_SW();
			Control(&User_Data,10);
			memset(main_buff1, 0, sizeof(main_buff1));
			main_len = sprintf(main_buff1, "BA,%s,OK,%d", User_Data.imei, User_Data.csq);
			MQTT_PublishQs0(main_buff1,main_len);
		}		
		else if(type[0] == 'D' && type[0] == '8'){
			AT_CMD("AT+CSQ\r\n","OK",1000);
			RTC_Time();					
			read_SW();
			memset(main_buff1,0,sizeof(main_buff1));
			main_len=sprintf(main_buff1,"C8,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
			User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
			GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
			MQTT_PublishQs0(main_buff1,main_len);	
		}







						else;			
    //parseCommand(commandStart);
}



















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
	static int err_cnt = 0;
 log_info("#####CMD:%s#####\r\n",cmd);
	while(timeOut--)
	{
		if(UART_WaitRecive() == REV_OK)							
		{
		
     log_info("*****nlog:%s*****\r\n",LPUART1_Data);	
			
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
				if(strstr(cmd,"+QMTSTAT: 0,1")){
					EC800_State = 0;			
				}
				
				UART_Clear();		
				err_cnt = 0;
				return 0;
			}
		}
		
		HAL_Delay(1);
	}	
  UART_Clear();
	if(err_cnt++ >= 30)	{
			err_cnt = 100;
			NVIC_SystemReset();   
	}
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


		
void MQTT_PublishQs0(char *data,uint16_t size)   
{
	User_Data.Heart_Cnt = 0;
	memset(main_buff,0,sizeof(main_buff));
	sprintf(main_buff," AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n",Mqtt_params.Sub_Topics,size);
	while(AT_CMD(main_buff,">",2000))
	HAL_Delay(1);
  HAL_UART_Transmit(&hlpuart1,(uint8_t *)data,size,1000);	
  if(!UART_SendCmd("publish_ACK","+QMTPUBEX: 0,0,0",200)){
   log_info("publish successful\r\n");
  }else{
   log_info("publish fail\r\n");
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
//		memset(Mqtt_params.Sub_Topics,0,strlen(Mqtt_params.Sub_Topics));
//		memset(Mqtt_params.Sub_Topics1,0,strlen(Mqtt_params.Sub_Topics1));
		if(Mqtt_params.Sub_Topics[0] == '\0' || Mqtt_params.Sub_Topics1[0] == '\0'){
			sprintf(Mqtt_params.Sub_Topics,"%s/report",User_Data.imei);
			sprintf(Mqtt_params.Sub_Topics1,"%s/command",User_Data.imei);	
			mcu_eeprom_write(46, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
		}	
		main_len=sprintf(main_buff,"AT+QMTSUB=0,1,\"%s\",0\r\n",Mqtt_params.Sub_Topics1);
		while(AT_CMD(main_buff,"+QMTSUB: 0,1,0,0",1000))
		HAL_Delay(100);
		EC800_State=1;
		log_info("EC20 Init Finish\r\n");	
		HAL_Delay(100);
		RTC_Time();
		Get_Adc_Value();
		
//		PWR12V_ON
		//HAL_Delay(5000);
		read_SW();
		
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
		User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
		GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
		MQTT_PublishQs0(main_buff1,main_len);	
  	UART_Clear();
		
//		PWR12V_OFF
//		HAL_Delay(100);
}




/*-------------------------------------------------*/
/*函数名：LPUART1被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U3PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
		log_info("4GSever_数据 %s 长度%d\r\n",data,datalen);
		if(strstr((char *)data,"+QMTRECV: 0,0")&&strstr((char *)data,(char *)User_Data.imei)){
//		processCommands((char *)data, commands, &command_count);
//		// 检查是否有符合条件的指令
//		if (command_count > 0) {
//				printf("开始处理符合条件的指令:\n");
//				for (int i = 0; i < command_count; i++) {
//						// 假设这里是处理指令的逻辑
//						printf("正在处理指令 %d: %s\n", i + 1, commands[i].raw_command);			
						if(strstr((char *)data,"A5")){
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							RTC_Time();					
//							PWR12V_ON
							//HAL_Delay(5000);
							read_SW();
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);	
//							PWR12V_OFF
//							HAL_Delay(100);
						}
						else if(strstr((char *)data,"D7")){
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							RTC_Time();					
//							PWR12V_ON
							//HAL_Delay(5000);
							read_SW();
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"C7,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);	
//							PWR12V_OFF
//							HAL_Delay(100);
						}
						else if(strstr((char *)data,"D8")){
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
							RTC_Time();					
//							PWR12V_ON
							//HAL_Delay(5000);
							read_SW();
							memset(main_buff1,0,sizeof(main_buff1));
							main_len=sprintf(main_buff1,"C8,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);	
//							PWR12V_OFF
//							HAL_Delay(100);
						}
						else if(strstr((char *)data,"A1")){
							get_A1_data((char *)data);
							memset(data,0,datalen);
							LPUART1_RX_STA = 0;
							AT_CMD("AT+CSQ\r\n","OK",1000);
//							PWR12V_ON
							//HAL_Delay(5000);
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
//							PWR12V_ON
							//HAL_Delay(5000);
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
//							PWR12V_ON
							//HAL_Delay(5000);
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
//							PWR12V_ON
							//HAL_Delay(5000);
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
//							PWR12V_ON
							//HAL_Delay(5000);
							read_SW();
							Control(&User_Data,10);
							memset(main_buff1, 0, sizeof(main_buff1));
							main_len = sprintf(main_buff1, "BA,%s,OK,%d", User_Data.imei, User_Data.csq);
							MQTT_PublishQs0(main_buff1,main_len);
						}
						else;			
//	
//						// 模拟处理完成后，删除该指令
//						// 将后续指令前移
//						for (int j = i; j < command_count - 1; j++) {
//								commands[j] = commands[j + 1];
//						}
//						
//						// 数组长度减一
//						command_count--;

//						// 调整i以重新检查当前位置
//						i--;
//				}
//				printf("所有指令已处理完毕。\n");
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	//dataString = token;
	// 使用逗号作为分隔符来提取数据
	
 if (token != NULL) {
			strncpy(dataString, token, sizeof(dataString) - 1);  // 复制字符串到本地数组
			dataString[sizeof(dataString) - 1] = '\0';          // 确保字符串以 '\0' 结束
	} else {
			log_info("Error: No valid data string found!\n");
			return;
	}
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			log_info("token%d=%s\r\n",i,token);
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			log_info("token%d=%s\r\n",i,token);
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	dataString = token;
	// 使用逗号作为分隔符来提取数据
  // 使用 strtok 函数拆分字符串
    token = strtok(dataString, ",");
    token = strtok(NULL, ",");
		token = strtok(NULL, ",");
		strcpy(User_Data.Time_Data, token);
		strcpy(date, token);
		log_info("User_Data.Time_Data=%s\r\n",User_Data.Time_Data);
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
		log_info("Year: %d\n", year);
   log_info("Month: %d\n", month);
   log_info("Day: %d\n", day);
   log_info("Hour: %d\n", hour);
   log_info("Minute: %d\n", minute);
   log_info("Second: %d\n", second);			
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			log_info("token%d=%s\r\n",i,token);
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			log_info("token%d=%s\r\n",i,token);
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
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
//	token = strtok(NULL, "\"");
	dataString = token;

	// 使用逗号作为分隔符来提取数据
	token = strtok(dataString, ",");
	while (token != NULL) {
			i++;
			log_info("token%d=%s\r\n",i,token);
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
    int field = 0;
    uint32_t num = 0;

    // 使用逗号作为分隔符
    token = strtok(data, ",");
    while (token != NULL) {
        field++;
        if (field == 3) {  // 第三个字段是我们需要的数字
            num = atoi(token);  // 将字符串转换为整数
						log_info("tokenAA: %d\n", num);
            if (num > 255) {
                log_info("Error: Number out of range for uint8_t.\n");
                User_Data.motor_Type=0;  // 返回一个明确的错误值或适当处理错误
            }
						else
								User_Data.motor_Type=(uint8_t)num;
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

   log_info("Year: %d\n", year);
   log_info("Month: %d\n", month);
   log_info("Day: %d\n", day);
   log_info("Hour: %d\n", hour);
   log_info("Minute: %d\n", minute);
   log_info("Second: %d\n", second);
    sDate.WeekDay = 0;  //这里必须要设置星期，否则读取年份不对
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
  
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
}

	