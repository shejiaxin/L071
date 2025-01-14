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
//EEPROM��ַ0x08080000 - 0x080817FF 6 * 1 KB//
//0x08080000 ΪConnect_State ��׼��ŵ�ַ
//0x08080000+5 Ϊlora�ṹ�� ��ŵ�ַ

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
 * ��ȡEEPROM
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


struct motor_Data {
    int motor_0_val;
    int motor_90_val_2t;
		int motor_90_val_4t;
		int motor_180_val;
};

int calculateMotorAngle_2t(int adcValue, struct motor_Data* userData) {
    int adcIncrement = (userData->motor_90_val_2t - userData->motor_0_val) / 9;
//    if (adcValue < userData->motor_0_val || adcValue > userData->motor_180_val) {
//        return 200;  // Return -1 if the ADC value is out of range
//    }
    return ((adcValue - userData->motor_0_val) / adcIncrement) * 10;

}
int calculateMotorAngle_4t(int adcValue, struct motor_Data* userData) {
    int adcIncrement = (userData->motor_180_val - userData->motor_90_val_4t ) / 9;
//    if (adcValue < userData->motor_90_val_4t || adcValue > userData->motor_180_val) {
//        return 200;  // Return -1 if the ADC value is out of range
//    }
     return ((adcValue - userData->motor_90_val_4t) / adcIncrement) * 10;

}
void read_SW(void)
{		
		struct motor_Data userData = {
        .motor_0_val = User_Data.motor_0_val,
        .motor_90_val_2t = User_Data.motor_90_val_2t,
				.motor_90_val_4t = User_Data.motor_90_val_4t,
        .motor_180_val = User_Data.motor_180_val
    };
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
		
		if(ADC_Values[0] <= User_Data.motor_90_val_2t && ADC_Values[0] >= User_Data.motor_0_val)
			User_Data.motor_Type = calculateMotorAngle_2t(ADC_Values[0], &userData);
		if(ADC_Values[3] >= User_Data.motor_90_val_4t && ADC_Values[3] <= User_Data.motor_180_val)
			User_Data.motor_Type = calculateMotorAngle_4t(ADC_Values[3], &userData)+90;

//		if(ADC_Values[0] >= User_Data.motor_0_val && ADC_Values[0] < (User_Data.motor_0_val )){
//			User_Data.motor_Type =0;
//		}
//		else if(ADC_Values[0] >= 1665 && ADC_Values[0] < 1761){
//			User_Data.motor_Type =10;
//		}
//		else if(ADC_Values[0] >= 1761 && ADC_Values[0] < 1858){
//					User_Data.motor_Type =20;
//		}	
//		else if(ADC_Values[0] >= 1858 && ADC_Values[0] < 1954){
//					User_Data.motor_Type =30;
//		}	
//		else if(ADC_Values[0] >= 1954 && ADC_Values[0] < 2051){
//					User_Data.motor_Type =40;
//		}	
//		else if(ADC_Values[0] >= 2051 && ADC_Values[0] < 2147){
//					User_Data.motor_Type =50;
//		}	
//		else if(ADC_Values[0] >= 2147 && ADC_Values[0] < 2340){
//					User_Data.motor_Type =60;
//		}	
//		else if(ADC_Values[0] >= 2340 && ADC_Values[0] < 2437){
//					User_Data.motor_Type =70;
//		}	
//		else if(ADC_Values[0] >= 2437 && ADC_Values[0] < 2533){
//					User_Data.motor_Type =80;
//		}	
//		else if(ADC_Values[0] >= 2533 && ADC_Values[0] < 2600){
//					User_Data.motor_Type =90;
//		}	
//		
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
		if(User_Data.motor_Type >= 1){
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
			User_Data.SysTick_10S_start = 1;
			if(User_Data.SysTick_10S >= 10000){
				User_Data.SysTick_10S = 0;
				User_Data.SysTick_10S_start = 0;
				break;
			}
			if(User_Data.SysTick_10S %1000 == 0)
				printf("δ�궨,���롰BD����ʼ�궨,\r\n");
			
			
			if(strstr((char *)Uart1_Data,"BD"))//if(Uart1_Data[0] == 0xBD && KEY_PB9_IN == RESET)
			{

				while(1){
					switch(BD_State){
						case 0:
							Motor_OFF						
							BD_State = 1;
							printf("�����ת,��ʼ�궨90��λ��\r\n���롰OK���궨90��λ��");
							break;
						case 1:
							if(strstr((char *)Uart1_Data,"OK")){
								Motor_brake
								User_Data.motor_90_val_2t =ADC_Values[0];
								User_Data.motor_90_val_4t =ADC_Values[3];		
								printf("90��λ�ñ궨Ϊ2t_ADC��%d,4t_ADC��%d,\r\n",User_Data.motor_90_val_2t,User_Data.motor_90_val_4t);
								printf("���롰BD0����ʼ�궨0��λ��,\r\n");								
								memset(Uart1_Data,0,10);
							}
							if(strstr((char *)Uart1_Data,"BD0")){
								BD_State = 2;						
								memset(Uart1_Data,0,10);
							}
							break;
						case 2:
							Motor_ON
							BD_State = 3;
							printf("�����ת,��ʼ�궨0��λ��,\r\n���롰OK���궨0��λ��\r\n");
							
							break;
						case 3:
							if(strstr((char *)Uart1_Data,"OK")){
								User_Data.motor_0_val =ADC_Values[0];
								printf("0��λ�ñ궨ΪADC��%d,\r\n",User_Data.motor_0_val);																		
								Motor_OFF		
								memset(Uart1_Data,0,10);
								BD_State = 4;	
							}
							break;
						case 4:									
								if(User_Data.SysTick_10S %1000 == 0)
									printf("�����ת���ȴ���90��λ��\r\n");		
								if(ADC_Values[0] >= User_Data.motor_90_val_2t){
									Motor_brake	
									printf("����ѻ�90��λ��\r\n");
									BD_State = 5;		
								}		
							break;
						case 5:
								if(User_Data.SysTick_10S %1000 == 0)
									printf("���롰BD180����ʼ�궨0��λ��,\r\n");
								if(strstr((char *)Uart1_Data,"BD180")){		
									printf("�����ת,��ʼ�궨180��λ��,\r\n���롰OK���궨180��λ��\r\n");
									Motor_OFF		
									BD_State = 6;		
									memset(Uart1_Data,0,10);
								}		
							break;
						case 6:	
							if(strstr((char *)Uart1_Data,"OK")){
								User_Data.motor_180_val =ADC_Values[3];
								printf("180��λ�ñ궨ΪADC��%d,\r\n",User_Data.motor_180_val);		
								memset(Uart1_Data,0,10);
								Motor_ON
								BD_State = 7;		
							}	
							break;
						case 7:
							if(User_Data.SysTick_10S %1000 == 0)
								printf("�����ת���ȴ���90��λ��\r\n");		
							if(ADC_Values[3]<=User_Data.motor_90_val_4t){
								Motor_brake
								printf("����ѻ�90��λ��\r\n");
								printf("�궨���\r\n");
								BD_State = 8;
								memset(Uart1_Data,0,10);
							}						
							break;
						default:
							break;
					
					}					
					Get_Adc_Value();
					if(BD_State >= 8){
							BD_State = 0;
							Motor_BD_State = 1;
							User_Data.SysTick_10S = 0;
							User_Data.SysTick_10S_start = 0;
							mcu_eeprom_write(20,(uint8_t *)&Motor_BD_State,1);
							mcu_eeprom_write(21,(uint8_t *)&User_Data.motor_0_val,4);
							mcu_eeprom_write(26,(uint8_t *)&User_Data.motor_90_val_2t,4);
							mcu_eeprom_write(31,(uint8_t *)&User_Data.motor_90_val_4t,4);
							mcu_eeprom_write(36,(uint8_t *)&User_Data.motor_180_val,4);
							break;
					}
				}
			}
		
		

	
	}
}
