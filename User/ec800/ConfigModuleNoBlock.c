/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   https://www.cnblogs.com/yangfengwu/p/11674814.html
  ******************************************************************************
	一,使用说明
		1,把以下程序放在1ms定时器中断中
			ConfigModuleNoBlockCnt++;
			
		2,把以下程序放到主函数
		while(1)
		{
			ConfigModuleNoBlock();//配置模块
			
			if(串口接收到一条完整的数据)
			{
				if(ConfigConnectDispose != NULL){
					ConfigConnectDispose(接收的数据地址,接收的数据个数);
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

//物联网平台参数

char Mqtt_ip[55]="81.70.28.130";					  //IP地址
int  Mqtt_port = 1883;											//(MOTT服务器)端口号28
char Mqtt_client_id[20] = "stm32";	
char Mqtt_user_name[20] = "pub";						//用户名
char Mqtt_password[50] = "Publish123456";		//密码
char Sub_Topics[50]="P000005/D000476/report";

#define main_buff_len 300
char main_buff[main_buff_len];
uint16_t main_len;

uint32_t ConfigModuleNoBlockCnt =0;   //配置函数延时变量,定时器内部累加
int ConfigModuleNoBlockCaseValue = 0; //控制执行哪一条Case 语句
char ConfigModuleNoBlockFlage  = 0;   //1-配置完连接 0-未配置完连接

uint32_t CompareValue=2000;                //每隔 Ms 发送一次数据
uint32_t SendNextDelay =0;                 //接收SendConfigFunction函数最后一个参数,最终传递给 ConfigModuleNoBlockCnt 控制写一条数据发送的时间
int Cnt = 0;                          //记录运行状态发送的次数
char DataReturnFlage  = 0;            //是否返回了预期的数据

char HopeReturnData1[20]="";//存储希望返回的数据
char HopeReturnData2[20]="";//存储希望返回的数据

void (*ConfigConnectDispose)(char *data,int len);//定义一个函数指针变量,用来处理模块返回的数据
void ConfigModuleRunNext(int delay);
/*********************************************************************************************************/




/**
* @brief  发送指令配置模块,非阻塞版
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
		if(DataReturnFlage == 1)//上一条指令是OK的
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//执行下一条
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//超过3次重新执行
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
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//这句必须加,清除所有的执行函数
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  执行下一条
* @param  delay 延时多少时间再执行下一条
* @param  
* @retval 
* @example 
延时500ms再执行下一条 case 语句,(注意,最大延时为 CompareValue)
ConfigModuleRunNext(CompareValue-500);
立即执行下一条 case语句
ConfigModuleRunNext(CompareValue);
**/
void ConfigModuleRunNext(int delay){
	SendNextDelay = delay;
	DataReturnFlage=1;
	ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
}


/**
* @brief  发送配置指令函数
* @param  order          发送数据(字符串)
* @param  FunctionSend   发送数据(运行发送数据函数)
* @param  HopeReturn1    接收数据(希望返回的字符串1)
* @param  HopeReturn2    接收数据(希望返回的字符串2)
* @param  FunctionParse  数据处理函数
* @param  ConfigFunctionValue  控制下一条数据的发送时间,取值为 ConfigFunctionCntCom:下一条立即发送
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//拷贝数据到数组 HopeReturn1,希望返回的数据1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//拷贝数据到数组 HopeReturn1,希望返回的数据2
	if(FunctionSend!=NULL) FunctionSend();//调用一个函数发送指令
	
	ConfigConnectDispose = FunctionParse;//传递处理函数指针
	SendNextDelay = ConfigFunctionValue;//如果上一条数据处理成功,控制发送下一条数据的时间
	
	if(order!=NULL){
		/*根据和模组通信的端口替换下面的发送数据函数*/
		 HAL_UART_Transmit(&hlpuart1, (unsigned char*)order,strlen(order), 0xFF);//发送的数据
	}
}


/**
* @brief  处理配置返回数据:判断是否返回想要的数据
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


