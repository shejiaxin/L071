#include "stm32l0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "lora.h"	          //������Ҫ��ͷ�ļ�
#include "include.h"
#include <stdlib.h>
#include <stdio.h>

volatile uint8_t Connect_State=0;

LoRaParameter LoRaNet = {                //����ģ�鹤��ģʽ
	0x00,                                //ģ���ַ���ֽ�
	0x01,                                //ģ���ַ���ֽ�
	LoRa_8N1,                            //���ڹ���ģʽ 8����λ ��У�� 1ֹͣλ
	LoRa_9600,                           //ģ�鴮�� ������ 9600
	LoRa_19_2,                           //ģ��������� 19.2K
	LoRa_CH23,                           //ģ���ŵ�23 0x17 ��ӦƵ��433MHz
	LoRa_TRANS,                          //ģ��͸������
	LoRa_UP,                             //ģ��TX AUX����
	LoRa_250MS,                          //ģ�黽��ʱ��  250����   ��ģʽ1��2��Ч
	LoRa_FEC_ON,                         //����FEC
	LoRa_FEC_33DBM,                      //���书��33dbm
};

/*-------------------------------------------------*/
/*����������ʼ��ģ���IO                           */
/*��  ����timeout�� ��ʱʱ�䣨10ms�ı�����         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LoRa_GPIO_Init(void)
{
	LoRa_PowerOFF;
	LoRa_MODE2;                                   //ģ��ģʽ3
	HAL_Delay(200);                               //�ʵ���ʱ
}
/*-------------------------------------------------*/
/*��������LoRa�ϵ縴λ                             */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
//uint8_t LoRa_Reset(int16_t timeout)
//{
//    LoRa_PowerOFF;                  //��OFF
//	HAL_Delay(200);                  //��ʱ
//	LoRa_PowerON;                   //��ON
//	
//	while(timeout--)                //�ȴ���λ�ɹ�
//	{                           
//		HAL_Delay(100);              //��ʱ100ms
//		if(LoRa_AUX==1)             //��Ҫ�ȵ��ߵ�ƽ1�������Ƴ�while����ʾ��λ���
//			break;       			//��������whileѭ��
//		printf("%d ",timeout);   //����������ڵĳ�ʱʱ��		
//	} 
//	printf("\r\n");              //���������Ϣ
//	if(timeout<=0)return 1;         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû�ܵȵ�LoRa_AUX�ߵ�ƽ������1
//	return 0;                       //��ȷ������0
//}
/*-------------------------------------------------*/
/*����������ʼ��ģ��                               */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
void LoRa_Init(void)
{
	LoRa_GPIO_Init();                                 //Loraģ�����ų�ʼ��
//	printf("׼����λLoraģ��\r\n");   	          //���������Ϣ
//	if(LoRa_Reset(100)){                              //��λLoraģ�飬����ֵ������㣬��ʾ����
//		printf("��λLoraģ��ʧ�ܣ�����\r\n");   	  //���������Ϣ
//		NVIC_SystemReset();                           //����
//	}else printf("Loraģ�鸴λ�ɹ�\r\n");   	      //���������Ϣ	
	printf("׼������Loraģ��\r\n");   	          //���������Ϣ
	HAL_Delay(200);                                   //��ʱ
	LoRa_Set();                                       //����Loraģ��
	HAL_Delay(200);
	LoRa_MODE0;	
	HAL_Delay(200);
	RTC_Time();
	
//	PWR12V_ON
//	HAL_Delay(10);
	read_SW();
	memset(main_buff1,0,sizeof(main_buff1));
	main_len=sprintf(main_buff1,"B5,%d,,%d,%d,%d,%d,%d,%d,%d,%d/%d/%d-%d:%d:%d,%d",LoRaNet.LoRa_AddrL,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
		User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.RW_motor_Type,User_Data.Wake_time,
		GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.lora_rssi);
	HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	
	
//	PWR12V_OFF
//	HAL_Delay(10);
}
/*-------------------------------------------------*/
/*��������LoRa���ù�������                         */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
void LoRa_Set(void)
{
	uint8_t cmd[10];                                          //�������ò��������黺�����������6�ֽ�
	 
	cmd[0] = 0xC0;                                           //���������1���ֽ�  �̶���0xC0
	cmd[1] = 0x00;																					//���������2���ֽ�  �̶���0x00
	cmd[2] = 0x07;																					//���������3���ֽ�  �̶���0x06
	cmd[3] = LoRaNet.LoRa_AddrH;                             //���������4���ֽ�  ģ���ַ���ֽ�
	cmd[4] = LoRaNet.LoRa_AddrL;                             //���������5���ֽ�  ģ���ַ���ֽ�
	cmd[5] = 0x00;																						//���������6���ֽ�  �����ַ�̶���0x05
	cmd[6] = LoRaNet.LoRa_UartMode | LoRaNet.LoRa_Baudrate | LoRaNet.LoRa_airvelocity;  //���������7���ֽ�  ����ģ�鴮��ģʽ �����ʺͿ�������
	cmd[7] = 0x00;																						//�ְ��趨 0 240�ֽڣ�Ĭ�ϣ� RSSI��������ʹ�� 0���ã�Ĭ�ϣ� ���书��33dBm��Ĭ��)
	cmd[8] = LoRaNet.LoRa_CH;                                //���������6���ֽ�  ����ģ�鴮��ģʽ �����ʺͿ�������
	cmd[9] = 0x83;
	//cmd[9] = LoRaNet.LoRa_DateMode | LoRaNet.LoRa_IOMode | LoRaNet.LoRa_WakeTime | LoRaNet.LoRa_FECMode | LoRaNet.LoRa_TxPower; //���������6���ֽ�  ����ģ�����ݴ���ģʽ TX��AUX��ģʽ ����ʱ�� FEC�Ƿ��� �Լ����书��                                 
	HAL_UART_Transmit(&huart2, cmd,10, 0xFF);//��������	
}
/*-------------------------------------------------*/
/*��������LoRa��ѯ��������                         */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
void LoRa_Get(uint8_t *data)
{	         			           	                                
	printf("ģ���ַ:0x%02X%02X\r\n",data[3],data[4]);                //���������Ϣ
	switch(data[6]&0x18){                                                //�жϴ��ڲ���
		case LoRa_8N1 :	 printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8O1 :	 printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8E1 :	 printf("8����λ żУ�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		default:
			break;
	}
	switch(data[6]&0xe0){                                                //�жϴ��ڲ�����
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
	
	switch(data[6]&0x07){                                                //�жϿ�������                                      //����
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
	switch(data[7]&0x03){                                                //�жϷ��书��
		case LoRa_FEC_33DBM :  printf("���书�� 33dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_30DBM :  printf("���书�� 30dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_27DBM :  printf("���书�� 27dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_24DBM :  printf("���书�� 24dbm\r\n");          //���������Ϣ
							   break;                                    //����
	}
	printf("�ŵ�:0x%02X  ��ӦƵ��%dMHz\r\n",data[8]&0x1F,410+(data[8]&0x1F));   //���������Ϣ
		
	switch(data[8]&0x20){                                                //�жϴ���ģʽ
		case LoRa_TRANS : printf("͸������ģʽ\r\n");                 //���������Ϣ
						  break;                                         //����
		case LoRa_POINT : printf("���㴫��ģʽ\r\n");                 //���������Ϣ
						  break;                                         //����
	}
//	
//	switch(data[8]&0x40){                                                //�ж�ģ��TX AUXģʽ
//		case LoRa_UP   : printf("ģ��TX AUX����\r\n");                //���������Ϣ
//						  break;                                         //����
//		case LoRa_OD   : printf("ģ��TX AUX��©\r\n");                //���������Ϣ
//						  break;                                         //����
//	}
//	
//	switch(data[8]&0x38){                                                //�жϻ���ʱ��
//		case LoRa_250MS :  printf("����ʱ�� 250����\r\n");            //���������Ϣ
//						   break;                                        //����
//		case LoRa_500MS :  printf("����ʱ�� 500����\r\n");            //���������Ϣ
//						   break;                                        //����
//		case LoRa_750MS :  printf("����ʱ�� 750����\r\n");            //���������Ϣ
//						   break;                                        //����
//		case LoRa_1000MS : printf("����ʱ�� 1000����\r\n");           //���������Ϣ
//						   break;                                        //����
//		case LoRa_1250MS : printf("����ʱ�� 1250����\r\n");           //���������Ϣ
//						   break;                                        //����
//		case LoRa_1500MS : printf("����ʱ�� 1500����\r\n");           //���������Ϣ
//						   break;                                        //����
//		case LoRa_1750MS : printf("����ʱ�� 1750����\r\n");           //���������Ϣ
//						   break;                                        //����
//		case LoRa_2000MS:  printf("����ʱ�� 2000����\r\n");           //���������Ϣ
//						   break;                                        //����
//	}
//	
//	switch(data[5]&0x04){                                                //�ж�FEC�������ǹر�
//		case LoRa_FEC_OFF : printf("�ر�FEC\r\n");                    //���������Ϣ
//							break;                                       //����
//		case LoRa_FEC_ON  : printf("����FEC\r\n");                    //���������Ϣ
//							break;                                       //����
//	}
	
	
}



/*-------------------------------------------------*/
/*������������2�����¼�                            */
/*��  ����data ������                              */
/*��  ����datalen �����ݳ���                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
	/*----------------------------------------------*/
	/*           ��������LoRaģ�鷵������           */
	/*----------------------------------------------*/
  printf("LoRa���� %s\r\n",data);
	char buff[50]={0};
	char *token;
	uint8_t i=0;
	strcpy(buff,(char *)data);
    // ��ȡ��һ�����ݶ�
    token = strtok((char *)buff, ",");
    while (token != NULL) {
			printf("token %d:%s",i,token);
				if(i==1){
					User_Data.lora_id = atoi(token);
				}
				i++;
        token = strtok(NULL, ",");
    }
	User_Data.lora_rssi = data[datalen-1];
	if(strstr((char *)data,"A5")&& (LoRaNet.LoRa_AddrL   == User_Data.lora_id)){
		RTC_Time();
		
//		PWR12V_ON
//		HAL_Delay(10);
		read_SW();
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B5,%d,%d,%d,%d,%d,%d,%d,%d,%d/%d/%d-%d:%d:%d,%d",LoRaNet.LoRa_AddrL,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
		User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.RW_motor_Type,User_Data.Wake_time,
		GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	
		
//		PWR12V_OFF
//		HAL_Delay(10);
	}
	else if(strstr((char *)data,"A1")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A1_data((char *)data);
		Control(&User_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B1,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	
	}
	else if(strstr((char *)data,"A2")&&(LoRaNet.LoRa_AddrL == User_Data.lora_id)){
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B2,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	
		lora_enter_stop_rtc_mode(User_Data.Wake_time);
	}
	else if(strstr((char *)data,"A3")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A3_data((char *)LPUART1_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B3,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	
	
	}
	else if(strstr((char *)data,"A4")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A4_data((char *)LPUART1_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B4,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//��������	

	}
//	else if(data[0]==0xC1){                   
//		printf("LoRaģ�����\r\n");           //���������Ϣ
//		LoRa_Get(data);                                      //�������õĲ���
//		LoRa_MODE0;                                          //�л���ģʽ0
//		HAL_Delay(200);                                      //��ʱ	
//	}
	else;
	
}




static void lora_get_A1_data(char *data)
{
	char *token;
	uint8_t i=0,userdata;
	// ʹ�ö�����Ϊ�ָ�������ȡ����
	token = strtok((char *)data, ",");
	while (token != NULL) {
			
			//printf("token%d=%s\r\n",i,token);
			switch(i)
			{
				case 2:
					userdata=atoi(token);
					User_Data.Switch_Type1=userdata;
					break;
				case 3:
					userdata=atoi(token);				
					User_Data.Switch_Type1=userdata;
					break;	
				case 4:
					userdata=atoi(token);			
					User_Data.Double_Type=userdata;
					break;
				case 5:
					userdata=atoi(token);					
					User_Data.Butterfly_Type=userdata;
					break;
				case 6:
					userdata=atoi(token);					
					User_Data.motor_Type=userdata;
					break;
				default:
					break;
			
			}
			i++;
			token = strtok(NULL, ",");
	}
}

static void lora_get_A3_data(char *data)
{
	char *token;
	uint8_t i=0;
	// �ҵ����ĸ�˫���ź�����ݲ���

	// ʹ�ö�����Ϊ�ָ�������ȡ����
	token = strtok(data, ",");
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

static void lora_get_A4_data(char *data)
{
	char *token;
	char *date_token ;
	char *dataString;
	int year, month, day, hour, minute, second;
	 char date[20];

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
	
