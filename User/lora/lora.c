#include "stm32l0xx_hal.h"    //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "lora.h"	          //包含需要的头文件
#include "include.h"
#include <stdlib.h>
#include <stdio.h>

volatile uint8_t Connect_State=0;

LoRaParameter LoRaNet = {                //网关模块工作模式
	0x00,                                //模块地址高字节
	0x01,                                //模块地址低字节
	LoRa_8N1,                            //串口工作模式 8数据位 无校验 1停止位
	LoRa_9600,                           //模块串口 波特率 9600
	LoRa_19_2,                           //模块空中速率 19.2K
	LoRa_CH23,                           //模块信道23 0x17 对应频率433MHz
	LoRa_TRANS,                          //模块透明传输
	LoRa_UP,                             //模块TX AUX上拉
	LoRa_250MS,                          //模块唤醒时间  250毫秒   仅模式1和2有效
	LoRa_FEC_ON,                         //开启FEC
	LoRa_FEC_33DBM,                      //发射功率33dbm
};

/*-------------------------------------------------*/
/*函数名：初始化模块的IO                           */
/*参  数：timeout： 超时时间（10ms的倍数）         */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LoRa_GPIO_Init(void)
{
	LoRa_PowerOFF;
	LoRa_MODE2;                                   //模块模式3
	HAL_Delay(200);                               //适当延时
}
/*-------------------------------------------------*/
/*函数名：LoRa上电复位                             */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
//uint8_t LoRa_Reset(int16_t timeout)
//{
//    LoRa_PowerOFF;                  //先OFF
//	HAL_Delay(200);                  //延时
//	LoRa_PowerON;                   //再ON
//	
//	while(timeout--)                //等待复位成功
//	{                           
//		HAL_Delay(100);              //延时100ms
//		if(LoRa_AUX==1)             //需要等到高电平1，才能推出while，表示复位完毕
//			break;       			//主动跳出while循环
//		printf("%d ",timeout);   //串口输出现在的超时时间		
//	} 
//	printf("\r\n");              //串口输出信息
//	if(timeout<=0)return 1;         //如果timeout<=0，说明超时时间到了，也没能等到LoRa_AUX高电平，返回1
//	return 0;                       //正确，返回0
//}
/*-------------------------------------------------*/
/*函数名：初始化模块                               */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
void LoRa_Init(void)
{
	LoRa_GPIO_Init();                                 //Lora模块引脚初始化
//	printf("准备复位Lora模块\r\n");   	          //串口输出信息
//	if(LoRa_Reset(100)){                              //复位Lora模块，返回值如果非零，表示错误
//		printf("复位Lora模块失败，重启\r\n");   	  //串口输出信息
//		NVIC_SystemReset();                           //重启
//	}else printf("Lora模块复位成功\r\n");   	      //串口输出信息	
	printf("准备设置Lora模块\r\n");   	          //串口输出信息
	HAL_Delay(200);                                   //延时
	LoRa_Set();                                       //设置Lora模块
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
	HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	
	
//	PWR12V_OFF
//	HAL_Delay(10);
}
/*-------------------------------------------------*/
/*函数名：LoRa设置工作参数                         */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
void LoRa_Set(void)
{
	uint8_t cmd[10];                                          //保存设置参数的数组缓冲区，命令共计6字节
	 
	cmd[0] = 0xC0;                                           //设置命令第1个字节  固定是0xC0
	cmd[1] = 0x00;																					//设置命令第2个字节  固定是0x00
	cmd[2] = 0x07;																					//设置命令第3个字节  固定是0x06
	cmd[3] = LoRaNet.LoRa_AddrH;                             //设置命令第4个字节  模块地址高字节
	cmd[4] = LoRaNet.LoRa_AddrL;                             //设置命令第5个字节  模块地址低字节
	cmd[5] = 0x00;																						//设置命令第6个字节  网络地址固定是0x05
	cmd[6] = LoRaNet.LoRa_UartMode | LoRaNet.LoRa_Baudrate | LoRaNet.LoRa_airvelocity;  //设置命令第7个字节  包括模块串口模式 波特率和空中速率
	cmd[7] = 0x00;																						//分包设定 0 240字节（默认） RSSI环境噪声使能 0禁用（默认） 发射功率33dBm（默认)
	cmd[8] = LoRaNet.LoRa_CH;                                //设置命令第6个字节  包括模块串口模式 波特率和空中速率
	cmd[9] = 0x83;
	//cmd[9] = LoRaNet.LoRa_DateMode | LoRaNet.LoRa_IOMode | LoRaNet.LoRa_WakeTime | LoRaNet.LoRa_FECMode | LoRaNet.LoRa_TxPower; //设置命令第6个字节  包括模块数据传输模式 TX和AUX的模式 唤醒时间 FEC是否开启 以及发射功率                                 
	HAL_UART_Transmit(&huart2, cmd,10, 0xFF);//发送数据	
}
/*-------------------------------------------------*/
/*函数名：LoRa查询工作参数                         */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
void LoRa_Get(uint8_t *data)
{	         			           	                                
	printf("模块地址:0x%02X%02X\r\n",data[3],data[4]);                //串口输出信息
	switch(data[6]&0x18){                                                //判断串口参数
		case LoRa_8N1 :	 printf("8数据位 无校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8O1 :	 printf("8数据位 奇校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8E1 :	 printf("8数据位 偶校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		default:
			break;
	}
	switch(data[6]&0xe0){                                                //判断串口波特率
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
	
	switch(data[6]&0x07){                                                //判断空中速率                                      //跳出
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
	switch(data[7]&0x03){                                                //判断发射功率
		case LoRa_FEC_33DBM :  printf("发射功率 33dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_30DBM :  printf("发射功率 30dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_27DBM :  printf("发射功率 27dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_24DBM :  printf("发射功率 24dbm\r\n");          //串口输出信息
							   break;                                    //跳出
	}
	printf("信道:0x%02X  对应频率%dMHz\r\n",data[8]&0x1F,410+(data[8]&0x1F));   //串口输出信息
		
	switch(data[8]&0x20){                                                //判断传输模式
		case LoRa_TRANS : printf("透明传输模式\r\n");                 //串口输出信息
						  break;                                         //跳出
		case LoRa_POINT : printf("定点传输模式\r\n");                 //串口输出信息
						  break;                                         //跳出
	}
//	
//	switch(data[8]&0x40){                                                //判断模块TX AUX模式
//		case LoRa_UP   : printf("模块TX AUX上拉\r\n");                //串口输出信息
//						  break;                                         //跳出
//		case LoRa_OD   : printf("模块TX AUX开漏\r\n");                //串口输出信息
//						  break;                                         //跳出
//	}
//	
//	switch(data[8]&0x38){                                                //判断唤醒时间
//		case LoRa_250MS :  printf("唤醒时间 250毫秒\r\n");            //串口输出信息
//						   break;                                        //跳出
//		case LoRa_500MS :  printf("唤醒时间 500毫秒\r\n");            //串口输出信息
//						   break;                                        //跳出
//		case LoRa_750MS :  printf("唤醒时间 750毫秒\r\n");            //串口输出信息
//						   break;                                        //跳出
//		case LoRa_1000MS : printf("唤醒时间 1000毫秒\r\n");           //串口输出信息
//						   break;                                        //跳出
//		case LoRa_1250MS : printf("唤醒时间 1250毫秒\r\n");           //串口输出信息
//						   break;                                        //跳出
//		case LoRa_1500MS : printf("唤醒时间 1500毫秒\r\n");           //串口输出信息
//						   break;                                        //跳出
//		case LoRa_1750MS : printf("唤醒时间 1750毫秒\r\n");           //串口输出信息
//						   break;                                        //跳出
//		case LoRa_2000MS:  printf("唤醒时间 2000毫秒\r\n");           //串口输出信息
//						   break;                                        //跳出
//	}
//	
//	switch(data[5]&0x04){                                                //判断FEC开启或是关闭
//		case LoRa_FEC_OFF : printf("关闭FEC\r\n");                    //串口输出信息
//							break;                                       //跳出
//		case LoRa_FEC_ON  : printf("开启FEC\r\n");                    //串口输出信息
//							break;                                       //跳出
//	}
	
	
}



/*-------------------------------------------------*/
/*函数名：串口2被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2PassiveEvent(uint8_t *data, uint16_t datalen)
{ 
	/*----------------------------------------------*/
	/*           接收设置LoRa模块返回数据           */
	/*----------------------------------------------*/
  printf("LoRa数据 %s\r\n",data);
	char buff[50]={0};
	char *token;
	uint8_t i=0;
	strcpy(buff,(char *)data);
    // 提取第一个数据段
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
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	
		
//		PWR12V_OFF
//		HAL_Delay(10);
	}
	else if(strstr((char *)data,"A1")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A1_data((char *)data);
		Control(&User_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B1,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	
	}
	else if(strstr((char *)data,"A2")&&(LoRaNet.LoRa_AddrL == User_Data.lora_id)){
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B2,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	
		lora_enter_stop_rtc_mode(User_Data.Wake_time);
	}
	else if(strstr((char *)data,"A3")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A3_data((char *)LPUART1_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B3,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	
	
	}
	else if(strstr((char *)data,"A4")&&(LoRaNet.LoRa_AddrL  == User_Data.lora_id)){
		lora_get_A4_data((char *)LPUART1_Data);
		memset(main_buff1,0,sizeof(main_buff1));
		main_len=sprintf(main_buff1,"B4,%d,OK,%d",LoRaNet.LoRa_AddrL,User_Data.lora_rssi);
		HAL_UART_Transmit(&huart2, (uint8_t *)main_buff1, main_len, 0xFF);//发送数据	

	}
//	else if(data[0]==0xC1){                   
//		printf("LoRa模块参数\r\n");           //串口输出信息
//		LoRa_Get(data);                                      //分析设置的参数
//		LoRa_MODE0;                                          //切换到模式0
//		HAL_Delay(200);                                      //延时	
//	}
	else;
	
}




static void lora_get_A1_data(char *data)
{
	char *token;
	uint8_t i=0,userdata;
	// 使用逗号作为分隔符来提取数据
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
	// 找到第四个双引号后的数据部分

	// 使用逗号作为分隔符来提取数据
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
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}
	
