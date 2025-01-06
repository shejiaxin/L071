 #include "usart.h"            
#include "ec800.h"	         
#include "include.h"
#include "ec800.h"
#include "rtc.h"
#include "adc.h"
#include<stdlib.h> 
#include "rtc.h"


volatile uint8_t EC800_State=0;


//������ƽ̨����
char Mqtt_ip[55]="81.70.28.130";					  //IP��ַ
int  Mqtt_port = 1883;											//(MOTT������)�˿ں�28
char Mqtt_client_id[20] = "stm32";	
char Mqtt_user_name[20] = "pub";						//�û���
char Mqtt_password[50] = "Publish123456";		//����
char Sub_Topics[50]="P000005/D000476/report";
char Sub_Topics1[50]="P000005/D000476/command";

char main_buff[main_buff_len];
char main_buff1[main_buff_len];
uint16_t main_len;

DATA User_Data;

void UART_Clear(void)
{
	memset(LPUART1_Data, 0, sizeof(LPUART1_Data));
	LPUART1Cnt = 0;
}

/**
 *@breief	��������
 *@param	cmd����
			res��Ҫ��鷵��ָ��
 *@reval	0-�ɹ�	1-ʧ��
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
	//if(LPUART1Flag == 0) 							//������ռ�����Ϊ0 ��˵�����ڽ�����
	if( LPUART1_RX_STA&0X8000){
		LPUART1_RX_STA = 0;
		return REV_OK;		
	}
	//if(LPUART1Flag == 1)					//���ϴ�ֵ��ͬ ��˵���������
	else
	{
		//LPUART1Flag=0;								//��0���ռ���			
		return REV_WAIT;								//���ؽ��ձ�־
	}	
						//���ؽ���δ���
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
	sprintf(main_buff," AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n",Sub_Topics,size);
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
		while(AT_CMD("AT+CGATT?\r\n","+CGATT: 1",1000))
		//while(AT_CMD("AT+CGATT?\r\n","TT: 1",1000))
    HAL_Delay(100);
		while(AT_CMD("AT+CREG?\r\n","0,1",2000)) 
		HAL_Delay(100);
		while(AT_CMD("AT+CSQ\r\n","OK",1000))  
	  HAL_Delay(100);
		while(AT_CMD("AT+QMTCFG=\"qmtping\",0,30\r\n","OK",1000))
		HAL_Delay(100);
		while(AT_CMD("AT+QMTCFG=\"recv/mode\",0,0,1\r\n","OK",1000)) 
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTOPEN=0,\"%s\",%d\r\n",Mqtt_ip,Mqtt_port);
		while(AT_CMD(main_buff,"+QMTOPEN: 0,0",1000))  
		HAL_Delay(100);
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",User_Data.imei,Mqtt_user_name,Mqtt_password);
		while(AT_CMD(main_buff,"+QMTCONN: 0,0,0",1000))  
		HAL_Delay(200);
		memset(main_buff,0,sizeof(main_buff));
		main_len=sprintf(main_buff,"AT+QMTSUB=0,1,\"%s\",0\r\n",Sub_Topics1);
		while(AT_CMD(main_buff,"+QMTSUB: 0,1,0,0",1000))
		HAL_Delay(100);
		EC800_State=1;
		printf("EC20 Init Finish\r\n");	
		HAL_Delay(100);
		RTC_Time();
		Get_Adc_Value();
		read_SW();
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%d/%d/%d-%d:%d:%d",User_Data.imei,User_Data.adc,User_Data.Switch_Type,
		User_Data.Double_Type,User_Data.Butterfly_Type,User_Data.motor_Type,User_Data.Wake_time,
		GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
		MQTT_PublishQs0(main_buff1,main_len);	
  	UART_Clear();
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
	// �ҵ����ĸ�˫���ź�����ݲ���
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	//dataString = token;
	// ʹ�ö�����Ϊ�ָ�������ȡ����
	
 if (token != NULL) {
			strncpy(dataString, token, sizeof(dataString) - 1);  // �����ַ�������������
			dataString[sizeof(dataString) - 1] = '\0';          // ȷ���ַ����� '\0' ����
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
					if(userdata!=User_Data.Switch_Type)
						User_Data.Switch_Type=userdata;
					break;
				case 4:
					userdata=atoi(token);
					if(userdata!=User_Data.Double_Type)
						User_Data.Double_Type=userdata;
					break;
				case 5:
					userdata=atoi(token);
					if(userdata!=User_Data.Butterfly_Type)
						User_Data.Butterfly_Type=userdata;
					break;
				case 6:
					userdata=atoi(token);
					if(userdata!=User_Data.motor_Type)
						User_Data.motor_Type=userdata;
					break;

				default:
					break;
			
			}
			token = strtok(NULL, ",");
	}
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
//	mcu_eeprom_write(0,&User_Data.Switch_Type1,1);
}

void get_A3_data(char *data)
{
	char *token;
	char *dataString;
	uint8_t i=0;
	// �ҵ����ĸ�˫���ź�����ݲ���
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;

	// ʹ�ö�����Ϊ�ָ�������ȡ����
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
	// �ҵ����ĸ�˫���ź�����ݲ���
	token = strtok(data, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	token = strtok(NULL, "\"");
	dataString = token;
	// ʹ�ö�����Ϊ�ָ�������ȡ����
  // ʹ�� strtok ��������ַ���
    token = strtok(dataString, ",");
    token = strtok(NULL, ",");
		token = strtok(NULL, ",");
		strcpy(User_Data.Time_Data, token);
		strcpy(date, token);
		printf("User_Data.Time_Data=%s\r\n",User_Data.Time_Data);
		// ��ȡ������	
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
    // ��ȡʱ����
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
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}


	