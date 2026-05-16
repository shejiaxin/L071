#include "rtc.h"
#include "control.h"
#include "adc.h"
#include <math.h>

volatile uint8_t Motor_BD_State=0;

#define SIZE 12


//EEPROM地址0x08080000 - 0x080817FF 6 * 1 KB//
//0x08080000 为Connect_State 标准存放地址
//0x08080000+5 为lora结构体 存放地址
/*  0-4   Connect_State
	5-19  LoRaNet 结构体
	20    Motor_BD_State
	21-25 User_Data.motor_0_val
	26-30 User_Data.motor_90_val_2t
	31-35 User_Data.motor_90_val_4t  
	36-40 User_Data.motor_180_val
	41-45 User_Data.Pressure_Range  //气压检测量程范围
	
	46-291 MqttParams 结构体
	300-305 User_Data.Heart_Time
*/
void mcu_eeprom_write(uint32_t address, uint8_t *data, uint16_t len)
{
    DIS_INT
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASHEx_DATAEEPROM_Unlock();
    for(uint8_t i=0; i<len; i++)
    {
        status +=HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_BASE_ADDR+address+i, *data);
        data++;
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
		EN_INT
//    HAL_Delay(10);
		
//		// 禁用中断，防止写入过程中发生意外
//    DIS_INT;

//    // 解锁 EEPROM
//    if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK) {
//        EN_INT;
//        return;
//    }
//    HAL_StatusTypeDef status = HAL_OK;
//    for (uint16_t i = 0; i < len; i++) {
//        status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_BASE_ADDR + address + i, data[i]);
//        if (status != HAL_OK) {
//            break;  // 遇到错误就停止写入
//        }
//    }
//    // 锁定 EEPROM
//    HAL_FLASHEx_DATAEEPROM_Lock();
//    
//    // 重新启用中断
//    EN_INT;
}
/*
 * 读取EEPROM
 */
void mcu_eeprom_read(uint32_t address, uint8_t *buffer, uint16_t len)
{
    uint8_t *read_address = 0;
    read_address = (uint8_t *)(EEPROM_BASE_ADDR+address);
    while(len--)
    {
        *buffer++ = *read_address++;
    }
}


float getBatteryLevel(float vADC) {
    // 计算斜率和截距
    float m = (8400 - 6470) / (2900 - 2250);
    float b = 8400 - m * 2900;

    // 计算电池电压
    float vBattery = m * vADC + b;

    // 计算电量
    float level;
    if (vBattery >= 8400) {
        level = 100.0;
    } else if (vBattery <= 6470) {
        level = 0.0;
    } else {
        level = (vBattery - 6470) / (8400 - 6470) * 100.0;
    }

    return level;
}

struct motor_Data {
    int motor_0_val;
    int motor_90_val_2t;
		int motor_90_val_4t;
		int motor_180_val;
};

int calculateMotorAngle_2t(int adcValue, struct motor_Data* userData) {
		int temp = 0;
    // 计算每度的增量，放大倍数为 1000
    int adcIncrement = (userData->motor_90_val_2t - userData->motor_0_val) * 1000 / 90;
    // 检查 ADC 值是否在范围内
    int delta = adcValue - userData->motor_0_val;
    if (delta <= 0) {
        return 0;  // 低于 0°，返回 0
    }
    // 计算角度，放大倍数为 1000
    temp = delta * 10000  / adcIncrement;

    // 限制最大值为 89
    if (temp >= 900) {  // 90000 表示 90.0
        temp = 890;     // 89000 表示 89.0
				return temp/10;
    }
    // 返回整数角度，四舍五入
    //return (temp + 500) / 1000;  // 加 500 表示四舍五入
		return ((temp + 5) / 10) ;
}
int calculateMotorAngle_4t(int adcValue, struct motor_Data* userData) {
    int adcIncrement = (userData->motor_180_val - userData->motor_90_val_4t ) * 1000 / 90;
		int delta1 = adcValue - userData->motor_90_val_4t;
		if (delta1 <= 0) {
        return 0;  // 低于 0°，返回 0
    }
		int temp1 = delta1 *10000 /adcIncrement;
		// 限制最大值为 90
    if (temp1 >= 900) {  // 90000 表示 90.0
        temp1 = 900;     // 89000 表示 89.0
				return temp1/10;
    }
    return ((temp1 + 5) / 10) ;  
		//return ((adcValue - userData->motor_90_val_4t) / adcIncrement) * 10;

}

#define MIN_READING 423  // The reading corresponding to 0 degrees
#define MAX_READING 1998 // The reading corresponding to 90 degrees

int calculateRange(uint32_t reading)
{
    // Check if the reading is in range
    if (reading < MIN_READING || reading > MAX_READING)
    {
        return -1;  // Return -1 to indicate an error
    }

    // Map the reading to the angle
    float angle = (float)(reading - MIN_READING + (MAX_READING - MIN_READING)/100) / (MAX_READING - MIN_READING) * 100.0;

    // Calculate the range
    int rounded_angle = round(angle / 10.0) * 10;

    return rounded_angle;
}



// 预计算好的参数表，避免运行时除法运算
typedef struct {
    float slope;
    float intercept;
} PressureCalibration;

// 使用原始参数表
static const PressureCalibration calibTable[] = {
    {0.0001875f, -0.075f}, // 0-0.30Kpa
    {0.000375f, -0.15f},   // 0-0.60Kpa
    {0.000625f, -0.25f}    // 0-1.00Kpa
};

float getPressure(float current_mA) {
    // 检查电流范围
    if (current_mA < 400.0f || current_mA > 2000.0f) {
        return 0.0f;
    }

    // 检查范围索引
    if (User_Data.Pressure_Range >= sizeof(calibTable)/sizeof(calibTable[0])) {
        return 0.0f;
    }

    // 直接查表计算
    const PressureCalibration* calib = &calibTable[User_Data.Pressure_Range];
    float pressure = calib->slope * current_mA + calib->intercept;
    
    // 限制最小值
    if (pressure < 0.0f) pressure = 0.0f;
    
    // 如果压力太小，返回0
    if (pressure < 0.0001f) {
        return 0.0f;
    }
    
    // 保留小数点后4位
    pressure = (float)((int)(pressure * 10000.0f + 0.5f)) / 10000.0f;
    
    return pressure;
}



//float getPressure(float current) {
//    float m = 0.0;  // 斜率
//    float b = 0.0;  // 截距

//    // 如果电流值不在范围内，返回0（或其他错误代码）
//    if (current < 400 || current > 2000) return 0;

//    // 根据Pressure_Range设置不同的压力范围公式
//    switch (User_Data.Pressure_Range) {
//        case 0:  // 0-300kPa
//            m = 0.000375;  // 斜率
//            b = -0.45;     // 截距
//            break;
//        case 1:  // 0-600kPa
//            m = 0.00075;   // 斜率
//            b = -0.6;      // 截距
//            break;
//        case 2:  // 0-1MPa
//            m = 0.00125;   // 斜率
//            b = -0.75;      // 截距
//            break;
//        default:
//            // 如果Pressure_Range不是1、2或3，返回0（或其他错误代码）
//            return 0;
//    }

//    // 计算压力
//    float pressure = m * current + b;
//    return pressure;
//}
void read_SW(void)
{		
		struct motor_Data userData = {
        .motor_0_val = User_Data.motor_0_val,
        .motor_90_val_2t = User_Data.motor_90_val_2t,
				.motor_90_val_4t = User_Data.motor_90_val_4t,
        .motor_180_val = User_Data.motor_180_val
    };
		
		//User_Data.adc =(int)getBatteryLevel(ADC_Values[2]);   //calculateBatteryPercentage(ADC_Values[2]/0.364,Mp,SIZE);
		User_Data.adc = User_Data.cw_capacity;                  //((User_Data.cw_capacity1 * 256 + User_Data.cw_capacity) * 100)/ (100*256);

		
		User_Data.RW_Butterfly_Type=calculateRange(User_Data.Pressure3);
		User_Data.RW_Pressure1=getPressure(User_Data.Pressure1);
		User_Data.RW_Pressure2=getPressure(User_Data.Pressure2);
		if(User_Data.RW_Butterfly_Type > 100)
			User_Data.RW_Butterfly_Type = 255;

		
		if(ADC_Values[0] <= User_Data.motor_90_val_2t && ADC_Values[0] >= User_Data.motor_0_val-100 &&
				(ADC_Values[1] >1500 || ADC_Values[1] <1300)
			)
			User_Data.RW_motor_Type = calculateMotorAngle_2t(ADC_Values[0], &userData);
		if(ADC_Values[1] >= User_Data.motor_90_val_4t && ADC_Values[1] <= User_Data.motor_180_val+100
			&&(ADC_Values[0] >= User_Data.motor_90_val_2t || (ADC_Values[0] >= 0 && ADC_Values[0] <= 1200)))  
			User_Data.RW_motor_Type = calculateMotorAngle_4t(ADC_Values[1], &userData)+90;

		User_Data.Report_motor_Type = ((User_Data.RW_motor_Type + 5) / 10) * 10;
		//(Get_Valve_Voltage(1)-400)/16+1;
		//User_Data.Butterfly_Type2=round(User_Data.Butterfly_Type2/10)*10;
}


void Control(DATA *data,uint8_t control_type)
{
	switch (control_type) {
        case 7: // A7 指令，只检查 Switch_Type1
            if (data->Switch_Type1 != data->RW_Switch_Type1) {
                data->control_state = 7;
            } else {
                data->control_state = 0;
            }
						data->control_report_state = 7;
            break;						
        case 8: // A8 指令，只检查 Switch_Type2
            if (data->Switch_Type2 != data->RW_Switch_Type2) {
                data->control_state = 8;
            } else {
                data->control_state = 0;
            }
						data->control_report_state = 8;						
            break;

        case 9: // A9 指令，只检查 Butterfly_Type
            if (data->Butterfly_Type != data->RW_Butterfly_Type) {
                data->control_state = 9;
            } else {
                data->control_state = 0;
            }
						data->control_report_state = 1;						
            break;
 
        case 10: // AA 指令，只检查 motor_Type
            if (data->motor_Type != data->RW_motor_Type) {
                data->control_state = 10;
            } else {
                data->control_state = 0;
            }
						data->control_report_state = 1;						
            break;

        default: // 默认检查所有字段
			if(data->Switch_Type1 != data->RW_Switch_Type1   		||
				data->Switch_Type2 != data->RW_Switch_Type2 		||
				data->Double_Type != data->RW_Double_Type 			||		
				data->Butterfly_Type != data->RW_Butterfly_Type ||
				data->motor_Type != data->RW_motor_Type 	
			)
			data->control_state =1;
			if(data->Switch_Type1 == data->RW_Switch_Type1   		&&
				data->Switch_Type2 == data->RW_Switch_Type2 		&&
				data->Double_Type == data->RW_Double_Type 			&&		
				data->Butterfly_Type == data->RW_Butterfly_Type &&
				data->motor_Type == data->RW_motor_Type 	
			)
			data->control_state =0;
            break;
    }
}

#define TIMER_MAX 1000 // Maximum value for the timer
#define MIN_ANGLE 0.0  // Minimum angle in degrees
#define MAX_ANGLE 100.0 // Maximum angle in degrees
#define MIN_CURRENT 4.0  // Minimum current in mA
#define MAX_CURRENT 20.0  // Maximum current in mA
#define MIN_PWM 200 // Minimum PWM value
#define MAX_PWM 1000 // Maximum PWM value

int calculateCCR(float angle)
{
		if(angle <= 0){
		  return 200;
		}
		if(angle >= 100){
		  return 1000;
		}
    // Map the angle to the current
    float current = (angle - MIN_ANGLE) / (MAX_ANGLE - MIN_ANGLE) * (MAX_CURRENT - MIN_CURRENT) + MIN_CURRENT;

    // Map the current to the PWM
    int CCR = (int)((current - MIN_CURRENT) / (MAX_CURRENT - MIN_CURRENT) * (MAX_PWM - MIN_PWM) + MIN_PWM);

    return CCR;
}
void Motor_Control(void)
{
	if(User_Data.control_state == 1){
		
		User_Data.control_report_state = 1;

//		PWR12V_ON

//		HAL_Delay(100);
		read_SW();
		if(User_Data.Switch_Type1 != User_Data.RW_Switch_Type1){
			if(User_Data.Switch_Type1){	
				//FM1_Motor_ON
			}
			else{
				//FM1_Motor_OFF
			}
		}
		if(User_Data.Switch_Type2 != User_Data.RW_Switch_Type2){
			if(User_Data.Switch_Type2){	
				//FM2_Motor_ON
			}
			else{
				//FM2_Motor_OFF
			}
		}
		if(User_Data.Double_Type != User_Data.RW_Double_Type){
			if(User_Data.Double_Type){	
				
			}
			else{
				
			}
		}
		if(User_Data.Butterfly_Type != User_Data.RW_Butterfly_Type && User_Data.Butterfly_Type >=0  && User_Data.Butterfly_Type <=100){
			//TIM2->CCR2 = calculateCCR(User_Data.Butterfly_Type);
		}
		/*服务器下发电机状态不等于读取的电机角度，并且范围在0-180 度时 */
		if(User_Data.motor_Type != User_Data.RW_motor_Type && User_Data.motor_Type >=0  && User_Data.motor_Type <=180){
			if(User_Data.motor_Type < User_Data.RW_motor_Type){
					Motor_ON				
			}
			else if(User_Data.motor_Type > User_Data.RW_motor_Type){
					Motor_OFF				
			}
			else{
					Motor_sleep
			}		
		}
		else if(User_Data.motor_Type == User_Data.RW_motor_Type ){
					Motor_brake
		}
		else ;
		
		Control(&User_Data,1);
	}
	else if(User_Data.control_state == 7){
//		User_Data.control_report_state = 7;
//		PWR12V_ON
//		HAL_Delay(100);
		read_SW();
		if(User_Data.Switch_Type1 != User_Data.RW_Switch_Type1){
			if(User_Data.Switch_Type1){	
				//FM1_Motor_ON
			}
			else{
				//FM1_Motor_OFF
			}
		}
		Control(&User_Data,7);		
	}
	else if(User_Data.control_state == 8){
//		User_Data.control_report_state = 8;
//		PWR12V_ON
//		HAL_Delay(100);
		read_SW();
		if(User_Data.Switch_Type2 != User_Data.RW_Switch_Type2){
			if(User_Data.Switch_Type2){	
				//FM2_Motor_ON
			}
			else{
				//FM2_Motor_OFF
			}
		}
		Control(&User_Data,8);		
	}
	else if(User_Data.control_state == 9){
//		User_Data.control_report_state = 1;		
//		PWR12V_ON
//		HAL_Delay(100);
		read_SW();
		if(User_Data.Butterfly_Type != User_Data.RW_Butterfly_Type && User_Data.Butterfly_Type >=0  && User_Data.Butterfly_Type <=100){
			TIM2->CCR2 = calculateCCR(User_Data.Butterfly_Type);
		}
		Control(&User_Data,9);		
	}
	else if(User_Data.control_state == 10){
//		User_Data.control_report_state = 1;
//		PWR12V_ON
		//HAL_Delay(100);
		read_SW();
		/*服务器下发电机状态不等于读取的电机角度，并且范围在0-180 度时 */
		if(User_Data.motor_Type != User_Data.RW_motor_Type && User_Data.motor_Type >=0  && User_Data.motor_Type <=180){
			if(User_Data.motor_Type < User_Data.RW_motor_Type){
					Motor_ON				
			}
			else if(User_Data.motor_Type > User_Data.RW_motor_Type){
					Motor_OFF				
			}
			else{
					Motor_sleep
			}		
		}
		else if(User_Data.motor_Type == User_Data.RW_motor_Type ){
					Motor_brake
		}
		else ;
		Control(&User_Data,10);		
	}		
	else if(User_Data.control_state == 0){

		Motor_sleep	
//	FM1_Motor_sleep
//	FM2_Motor_sleep
		//HAL_Delay(10);
		
		if(User_Data.control_report_state ){			

				switch (User_Data.control_report_state){
					case 1:
						RTC_Time();	
						memset(main_buff1,0,sizeof(main_buff1));
						main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
						User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
						GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
						MQTT_PublishQs0(main_buff1,main_len);	
						break;
					case 7:
						RTC_Time();	
//						memset(main_buff1,0,sizeof(main_buff1));
//						main_len=sprintf(main_buff1,"C7,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
//							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
//							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
//						MQTT_PublishQs0(main_buff1,main_len);	
						break;
					case 8:
						RTC_Time();	
//						main_len=sprintf(main_buff1,"C8,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
//							User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
//							GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
//						MQTT_PublishQs0(main_buff1,main_len);	
						break;
					default:
						break;
				}
				
			
	
			User_Data.control_report_state = 0;
		}
		//HAL_Delay(100);
//		PWR12V_OFF
	}
	else;	

}
 
void Init_User_Data(DATA *data) 
{
    data->Switch_Type1 = 0;
    data->Switch_Type2 = 0;
    data->Butterfly_Type = 0;
    data->motor_Type = 0;
    data->RW_Switch_Type1 = 0;
    data->RW_Switch_Type2 = 0;
    data->RW_Butterfly_Type = 0;
    data->RW_motor_Type = 0;
    data->control_state = 0;
    data->control_report_state = 0;
}

uint8_t BD_State = 0;
//void Motor_BD(void)
//{
//	mcu_eeprom_read(20,(uint8_t *)&Motor_BD_State,1);
//	while(Motor_BD_State ==0 ){
//			User_Data.SysTick_10S_start = 1;
//			if(User_Data.SysTick_10S >= 10000){
//				User_Data.SysTick_10S = 0;
//				User_Data.SysTick_10S_start = 0;
//				break;
//			}
//			if(User_Data.SysTick_10S %1000 == 0)
//				printf("未标定,输入“BD”开始标定,\r\n");
//			
//			
//			if(strstr((char *)Uart1_Data,"BD"))//if(Uart1_Data[0] == 0xBD && KEY_PB9_IN == RESET)
//			{
//				PWR12V_ON
//				while(1){
//					switch(BD_State){
//						case 0:
//							Motor_OFF						
//							BD_State = 1;
//							printf("电机反转,开始标定90°位置\r\n输入“OK”标定90°位置");
//							break;
//						case 1:
//							if(strstr((char *)Uart1_Data,"OK")){
//								Motor_brake
//								User_Data.motor_90_val_2t =ADC_Values[0];
//								User_Data.motor_90_val_4t =ADC_Values[1];		
//								printf("90°位置标定为2t_ADC：%d,4t_ADC：%d,\r\n",User_Data.motor_90_val_2t,User_Data.motor_90_val_4t);
//								printf("输入“BD0”开始标定0°位置,\r\n");								
//								memset(Uart1_Data,0,10);
//							}
//							if(strstr((char *)Uart1_Data,"BD0")){
//								BD_State = 2;						
//								memset(Uart1_Data,0,10);
//							}
//							break;
//						case 2:
//							Motor_ON
//							BD_State = 3;
//							printf("电机正转,开始标定0°位置,\r\n输入“OK”标定0°位置\r\n");
//							
//							break;
//						case 3:
//							if(strstr((char *)Uart1_Data,"OK")){
//								User_Data.motor_0_val =ADC_Values[0];
//								printf("0°位置标定为ADC：%d,\r\n",User_Data.motor_0_val);																		
//								Motor_OFF		
//								memset(Uart1_Data,0,10);
//								BD_State = 4;	
//							}
//							break;
//						case 4:									
//								if(User_Data.SysTick_10S %1000 == 0)
//									printf("电机反转，等待回90°位置\r\n");		
//								if(ADC_Values[0] >= User_Data.motor_90_val_2t){
//									Motor_brake	
//									printf("电机已回90°位置\r\n");
//									BD_State = 5;		
//								}		
//							break;
//						case 5:
//								if(User_Data.SysTick_10S %1000 == 0)
//									printf("输入“BD180”开始标定0°位置,\r\n");
//								if(strstr((char *)Uart1_Data,"BD180")){		
//									printf("电机反转,开始标定180°位置,\r\n输入“OK”标定180°位置\r\n");
//									Motor_OFF		
//									BD_State = 6;		
//									memset(Uart1_Data,0,10);
//								}		
//							break;
//						case 6:	
//							if(strstr((char *)Uart1_Data,"OK")){
//								User_Data.motor_180_val =ADC_Values[1];
//								printf("180°位置标定为ADC：%d,\r\n",User_Data.motor_180_val);		
//								memset(Uart1_Data,0,10);
//								Motor_ON
//								BD_State = 7;		
//							}	
//							break;
//						case 7:
//							if(User_Data.SysTick_10S %1000 == 0)
//								printf("电机反转，等待回90°位置\r\n");		
//							if(ADC_Values[1]<=User_Data.motor_90_val_4t){
//								Motor_brake
//								printf("电机已回90°位置\r\n");
//								printf("标定完成\r\n");
//								BD_State = 8;
//								memset(Uart1_Data,0,10);
//							}						
//							break;
//						default:
//							break;
//					
//					}					
//					Get_Adc_Value();
//					if(User_Data.SysTick_10S %1000 == 0){
//							HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
//					}
//					if(BD_State >= 8){
//							BD_State = 0;
//							Motor_BD_State = 1;
//							User_Data.SysTick_10S = 0;
//							User_Data.SysTick_10S_start = 0;
////							PWR12V_OFF
//							HAL_GPIO_WritePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin,GPIO_PIN_RESET);
//							mcu_eeprom_write(20,(uint8_t *)&Motor_BD_State,1);
//							mcu_eeprom_write(21,(uint8_t *)&User_Data.motor_0_val,4);
//							mcu_eeprom_write(26,(uint8_t *)&User_Data.motor_90_val_2t,4);
//							mcu_eeprom_write(31,(uint8_t *)&User_Data.motor_90_val_4t,4);
//							mcu_eeprom_write(36,(uint8_t *)&User_Data.motor_180_val,4);
//							break;
//					}
//				}
//			}
//		
//		

//	
//	}
//}
