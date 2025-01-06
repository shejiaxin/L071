/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   https://www.cnblogs.com/yangfengwu/p/11674814.html
  ******************************************************************************
	һ,ʹ��˵��
		1,�����³������1ms��ʱ���ж���
			ConfigModuleNoBlockCnt++;
			
		2,�����³���ŵ�������
		while(1)
		{
			ConfigModuleNoBlock();//����ģ��
			
			if(���ڽ��յ�һ������������)
			{
				if(ConfigConnectDispose != NULL){
					ConfigConnectDispose(���յ����ݵ�ַ,���յ����ݸ���);
				}
			}
		}
			
  ******************************************************************************
  */
#define CONFIGMODULENOBLOCK_C_
#include <stdio.h>
#include <string.h>
#include "ConfigModuleNoBlock.h"
#include "include.h"
#include "usart.h"

//������ƽ̨����

char Mqtt_ip[55]="81.70.28.130";					  //IP��ַ
int  Mqtt_port = 1883;											//(MOTT������)�˿ں�28
char Mqtt_client_id[20] = "stm32";	
char Mqtt_user_name[20] = "pub";						//�û���
char Mqtt_password[50] = "Publish123456";		//����
char Sub_Topics[50]="P000005/D000476/report";

#define main_buff_len 300
char main_buff[main_buff_len];
uint16_t main_len;

uint32_t ConfigModuleNoBlockCnt =0;   //���ú�����ʱ����,��ʱ���ڲ��ۼ�
int ConfigModuleNoBlockCaseValue = 0; //����ִ����һ��Case ���
char ConfigModuleNoBlockFlage  = 0;   //1-���������� 0-δ����������

uint32_t CompareValue=2000;                //ÿ�� Ms ����һ������
uint32_t SendNextDelay =0;                 //����SendConfigFunction�������һ������,���մ��ݸ� ConfigModuleNoBlockCnt ����дһ�����ݷ��͵�ʱ��
int Cnt = 0;                          //��¼����״̬���͵Ĵ���
char DataReturnFlage  = 0;            //�Ƿ񷵻���Ԥ�ڵ�����

char HopeReturnData1[20]="";//�洢ϣ�����ص�����
char HopeReturnData2[20]="";//�洢ϣ�����ص�����

void (*ConfigConnectDispose)(char *data,int len);//����һ������ָ�����,��������ģ�鷵�ص�����
void ConfigModuleRunNext(int delay);
/*********************************************************************************************************/




/**
* @brief  ����ָ������ģ��,��������
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
**/
void ConfigModuleNoBlock(void)
{
	if(ConfigModuleNoBlockCnt>CompareValue && ConfigModuleNoBlockFlage==0)
	{
		ConfigModuleNoBlockCnt=0;
		if(DataReturnFlage == 1)//��һ��ָ����OK��
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//ִ����һ��
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//����3������ִ��
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
			}
		}
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0:
				SendConfigFunction("AT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 1:
				SendConfigFunction("ATE0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 2:
				SendConfigFunction("AT+CPIN?\r\n",NULL,"READY",NULL,FunctionParseCompare,CompareValue);
				break;
			case 3:
				SendConfigFunction("AT+CSQ\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 4:
				SendConfigFunction("AT+CREG?\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 5:
				SendConfigFunction("AT+QMTCFG=\"qmtping\",0,30\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 6:
				SendConfigFunction("AT+QMTCFG=\"recv/mode\",0,0,1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 7:
				memset(main_buff,0,sizeof(main_buff));
				main_len=sprintf(main_buff,"AT+QMTOPEN=0,\"%s\",%d\r\n",Mqtt_ip,Mqtt_port);
				SendConfigFunction(main_buff,NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 8:
				memset(main_buff,0,sizeof(main_buff));
				main_len=sprintf(main_buff,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",Mqtt_client_id,Mqtt_user_name,Mqtt_password);
				SendConfigFunction(main_buff,NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			case 9:
				memset(main_buff,0,sizeof(main_buff));
				main_len=sprintf(main_buff,"AT+QMTSUB=0,1,\"%s\",0\r\n",Sub_Topics);
				SendConfigFunction(main_buff,NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;		
			case 10:
				memset(main_buff,0,sizeof(main_buff));
				main_len=sprintf(main_buff," AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n","123",3);
				SendConfigFunction(main_buff,NULL,"OK",NULL,FunctionParseCompare,CompareValue);
				break;
			
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//�������,������е�ִ�к���
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  ִ����һ��
* @param  delay ��ʱ����ʱ����ִ����һ��
* @param  
* @retval 
* @example 
��ʱ500ms��ִ����һ�� case ���,(ע��,�����ʱΪ CompareValue)
ConfigModuleRunNext(CompareValue-500);
����ִ����һ�� case���
ConfigModuleRunNext(CompareValue);
**/
void ConfigModuleRunNext(int delay){
	SendNextDelay = delay;
	DataReturnFlage=1;
	ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
}


/**
* @brief  ��������ָ���
* @param  order          ��������(�ַ���)
* @param  FunctionSend   ��������(���з������ݺ���)
* @param  HopeReturn1    ��������(ϣ�����ص��ַ���1)
* @param  HopeReturn2    ��������(ϣ�����ص��ַ���2)
* @param  FunctionParse  ���ݴ�����
* @param  ConfigFunctionValue  ������һ�����ݵķ���ʱ��,ȡֵΪ ConfigFunctionCntCom:��һ����������
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//�������ݵ����� HopeReturn1,ϣ�����ص�����1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//�������ݵ����� HopeReturn1,ϣ�����ص�����2
	if(FunctionSend!=NULL) FunctionSend();//����һ����������ָ��
	
	ConfigConnectDispose = FunctionParse;//���ݴ�����ָ��
	SendNextDelay = ConfigFunctionValue;//�����һ�����ݴ���ɹ�,���Ʒ�����һ�����ݵ�ʱ��
	
	if(order!=NULL){
		/*���ݺ�ģ��ͨ�ŵĶ˿��滻����ķ������ݺ���*/
		 HAL_UART_Transmit(&hlpuart1, (unsigned char*)order,strlen(order), 0xFF);//���͵�����
	}
}


/**
* @brief  �������÷�������:�ж��Ƿ񷵻���Ҫ������
* @param  data
* @param  
* @retval 
* @example 
**/
void FunctionParseCompare(char *data,int len)
{
	if(strlen(HopeReturnData1) != 0 && strstr(data, HopeReturnData1))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
	if(strlen(HopeReturnData2) != 0 && strstr(data, HopeReturnData2))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
}


