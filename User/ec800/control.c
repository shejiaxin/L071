#include "rtc.h"
#include "control.h"
#include "adc.h"
#include <math.h>

volatile uint8_t Motor_BD_State=0;

#define SIZE 12

struct BatteryMapping Mp[SIZE] = {
				{6000, 0.0},
        {6900, 5.0},
        {7360, 10.0},
        {7480, 20.0},
        {7540, 30.0},
        {7580, 40.0},
        {7640, 50.0},
        {7740, 60.0},
        {7840, 70.0},
        {7960, 80.0},
        {8120, 90.0},
        {8400, 100.0}
 };
//EEPROM地址0x08080000 - 0x080817FF 6 * 1 KB//
//0x08080000 为Connect_State 标准存放地址
//0x08080000+5 为lora结构体 存放地址

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
    HAL_Delay(10);
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

static float calculateBatteryPercentage(float voltage, struct BatteryMapping* map, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (voltage >= map[i].voltage && voltage <= map[i + 1].voltage) {
            return ((voltage - map[i].voltage) / (map[i + 1].voltage - map[i].voltage)) 
                   * (map[i + 1].percentage - map[i].percentage) + map[i].percentage;
        }
    }
    return -1.0;  // Return -1 if the voltage is out of range
}
void read_SW(void)
{		
		User_Data.adc = calculateBatteryPercentage(ADC_Values[2]/0.364,Mp,SIZE);
		if(IO1_IN==1&&IO2_IN==0){
			User_Data.Switch_Type=1;
		}
	  if(IO1_IN==0&&IO2_IN==1){
			User_Data.Switch_Type=0;
		}

		User_Data.Butterfly_Type=(Get_Valve_Voltage(1)-400)/16+1;
		User_Data.Butterfly_Type=round(User_Data.Butterfly_Type/10)*10;
		
		User_Data.Double_Type=Get_Valve_Voltage(0);
		

		if(ADC_Values[0] >= 1665 && ADC_Values[0] < 1761){
			User_Data.motor_Type =0;
		}
		else if(ADC_Values[0] >= 1665 && ADC_Values[0] < 1761){
			User_Data.motor_Type =10;
		}
		else if(ADC_Values[0] >= 1761 && ADC_Values[0] < 1858){
					User_Data.motor_Type =20;
		}	
		else if(ADC_Values[0] >= 1858 && ADC_Values[0] < 1954){
					User_Data.motor_Type =30;
		}	
		else if(ADC_Values[0] >= 1954 && ADC_Values[0] < 2051){
					User_Data.motor_Type =40;
		}	
		else if(ADC_Values[0] >= 2051 && ADC_Values[0] < 2147){
					User_Data.motor_Type =50;
		}	
		else if(ADC_Values[0] >= 2147 && ADC_Values[0] < 2340){
					User_Data.motor_Type =60;
		}	
		else if(ADC_Values[0] >= 2340 && ADC_Values[0] < 2437){
					User_Data.motor_Type =70;
		}	
		else if(ADC_Values[0] >= 2437 && ADC_Values[0] < 2533){
					User_Data.motor_Type =80;
		}	
		else if(ADC_Values[0] >= 2533 && ADC_Values[0] < 2600){
					User_Data.motor_Type =90;
		}	
		
//		if(ADC_Values[3] >= 1665 && ADC_Values[0] < 1761){
//			User_Data.motor_Type =0;
//		}
//		else if(ADC_Values[3] >= 1665 && ADC_Values[0] < 1761){
//			User_Data.motor_Type =10;
//		}
//		else if(ADC_Values[3] >= 1761 && ADC_Values[0] < 1858){
//					User_Data.motor_Type =20;
//		}	
//		else if(ADC_Values[3] >= 1858 && ADC_Values[0] < 1954){
//					User_Data.motor_Type =30;
//		}	
//		else if(ADC_Values[3] >= 1954 && ADC_Values[0] < 2051){
//					User_Data.motor_Type =40;
//		}	
//		else if(ADC_Values[3] >= 2051 && ADC_Values[0] < 2147){
//					User_Data.motor_Type =50;
//		}	
//		else if(ADC_Values[3] >= 2147 && ADC_Values[0] < 2340){
//					User_Data.motor_Type =60;
//		}	
//		else if(ADC_Values[3] >= 2340 && ADC_Values[0] < 2437){
//					User_Data.motor_Type =70;
//		}	
//		else if(ADC_Values[3] >= 2437 && ADC_Values[0] < 2533){
//					User_Data.motor_Type =80;
//		}	
//		else if(ADC_Values[3] >= 2533 && ADC_Values[0] < 2600){
//					User_Data.motor_Type =90;
//		}			
		//(Get_Valve_Voltage(1)-400)/16+1;
		//User_Data.Butterfly_Type2=round(User_Data.Butterfly_Type2/10)*10;
}


void Control(DATA data)
{
	PWR18V_OFF
	PWR12V_ON
	RELAY1_ON_OFF_ON
	
	//HAL_Delay(1000);
	if(data.Switch_Type){	
		RELAY1_ON		
	}
	else{
		RELAY1_OFF	
	}
	if(data.Double_Type){

	}
	else{		
	
	}
//	if(data.motor_Type){
//		User_Data.control_state =1;
//		User_Data.motor_Type = 1;
//	}
//	else{		
//		User_Data.control_state =1;
//		User_Data.motor_Type = 0;
//	}
	TIM2->CCR2 = data.Butterfly_Type*8+200;
	//TIM2->CCR1 = data.Butterfly_Type2*8+200;

}

void Motor_Control(void)
{
	if(User_Data.control_state ==1){
		if(User_Data.motor_Type == 1){
			Motor_ON
			if(ADC_Values[0] >= 3290 ){
				Motor_brake
				User_Data.control_state =0;
			}
		}
		if(User_Data.motor_Type == 0){
			Motor_OFF
			if(ADC_Values[0] <= 2475){
				Motor_brake
				User_Data.control_state =0;
			}
		}
	}
	else{
		Motor_sleep	
	}

}
 

uint8_t BD_State = 0;
void Motor_BD(void)
{
	mcu_eeprom_read(20,(uint8_t *)&Motor_BD_State,1);
	while(Motor_BD_State ==0 ){
			if(User_Data.SysTick_10S %1000 == 0)
				printf("未标定\r\n");
			User_Data.SysTick_10S_start = 1;
			
			if(strstr((char *)Uart1_Data,"BD"))//if(Uart1_Data[0] == 0xBD && KEY_PB9_IN == RESET)
			{
				User_Data.SysTick_10S = 0;
				User_Data.SysTick_10S_start = 0;
				while(1){
					switch(BD_State){
						case 0:
							PWR12V_ON
							Motor_OFF
							if(strstr((char *)Uart1_Data,"OK")){
								Motor_brake
								User_Data.motor_90_val =ADC_Values[0];
								BD_State = 1;
								memset(Uart1_Data,0,10);
							}		
							break;
						case 1:
							Motor_ON
							if(strstr((char *)Uart1_Data,"OK")){
								//Motor_brake
								User_Data.motor_0_val =ADC_Values[0];
								Motor_OFF
								if(ADC_Values[0]<=User_Data.motor_90_val){
									Motor_brake
									BD_State = 2;
									memset(Uart1_Data,0,10);
								}								
							}	
							break;
						case 2:
							Motor_OFF
							if(strstr((char *)Uart1_Data,"OK")){
								//Motor_brake
								User_Data.motor_180_val =ADC_Values[0];
								Motor_OFF
								if(ADC_Values[0]<=User_Data.motor_90_val){
									Motor_brake
									BD_State = 3;
									memset(Uart1_Data,0,10);
								}								
							}	
							break;
						default:
							break;
					
					}					
					Get_Adc_Value();
					if(BD_State == 3){
							BD_State = 0;
							Motor_BD_State = 1;
							mcu_eeprom_write(20,(uint8_t *)&Motor_BD_State,1);
							break;
					}
				}
			}
		
		if(User_Data.SysTick_10S >= 10000){
			User_Data.SysTick_10S = 0;
			User_Data.SysTick_10S_start = 0;
			break;
		}

	
	}
}