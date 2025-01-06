#include "user_iic.h"

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief ѡ��ͨ����
 @param channel -[in] ͨ����
 @return ��
*/
void TCA9548A_SetChannel(uint8_t channel)
{		
    uint8_t data;
    switch(channel)
    {
        case 0:
            data = TCA9548A_CHANNEL_0;
            break;
        case 1:
            data = TCA9548A_CHANNEL_1;
            break;
        case 2:
            data = TCA9548A_CHANNEL_2;
            break;
        case 3:
            data = TCA9548A_CHANNEL_3;
            break;

        default:
            break;        
    }
    HAL_I2C_Master_Transmit(&hi2c2, (TCA9548A_SLAVE_ADDR << 1) | TCA9548A_WRITE_BIT , &data, 1, 10);
}

void MCP3421_Init(void)
{
	uint8_t parameter = 0x9c; //ϵͳ��������Ϊ18λ���ȣ��Զ�ת��ģʽ����PGA���档�����ּ������ֲᡣ
	if(HAL_I2C_IsDeviceReady(&hi2c2,MCP3421ADDR,5,100)==HAL_OK)
	// if(!HAL_I2C_IsDeviceReady(&hi2c1,MCP3421ADDR,5,100))
	{
		//printf("MCP3421 is ok.\n\r");
	}
	else{
		//printf("MCP3421 is error.\n\r");
	}
	HAL_I2C_Master_Transmit(&hi2c2,MCP3421ADDR,&parameter,1,100);
	HAL_Delay(5);
	while(HAL_I2C_IsDeviceReady(&hi2c2,MCP3421ADDR,5,100))
	{
		HAL_Delay(5);
	}
	//printf ("MCP3421 RESET is OK.\n\r");
}

uint32_t MCP3421_Get_Value(void)
{
	uint32_t ADC_Value;
	uint8_t Adc_Temp[3]={0};
	if(HAL_I2C_IsDeviceReady(&hi2c2,MCP3421ADDR,5,100)==HAL_OK){
		HAL_I2C_Master_Receive(&hi2c2,MCP3421ADDR,Adc_Temp,3,100);
		ADC_Value = Adc_Temp[0];
		ADC_Value&=0x03;
		ADC_Value<<=8;
		ADC_Value+= Adc_Temp[1];
		ADC_Value<<=8;
		ADC_Value+= Adc_Temp[2];
	}
	else{}
	return  ADC_Value;
}


float Voltage_Convert(void)
{
	uint32_t Voltage;
	float Measure;
	Voltage = MCP3421_Get_Value();
	//printf("Voltage = %d\n\r",Voltage);
	if((Voltage&0x00020000)==0x00020000) //�ж����λ����λ�Ƿ�ΪΪ1��Ϊ��1����Ϊ����ѹ
	{
		Voltage&=0x0001ffff;
		Voltage=(~Voltage)+1;
		Voltage &=0x0001ffff;
		Measure = 15.625*Voltage; //15.625Ϊ18λ�ֱ��ʵľ��ȡ������ֱ��ʵľ��ȼ������ֲᡣ
		//Measure = Measure *0.001;
		Measure=0;
		//printf("Measure Result Voltage = -%fmv\n\r",Measure);
	}
	else {//Ϊ��0��������ѹֱ��ת��
		Voltage&=0x0001ffff;
		Measure = 15.625*Voltage;
		Measure = Measure *0.001;
		//printf("Measure Result Voltage = %fmv\n\r",Measure);
	}
	return Measure;
}

float Get_Valve_Voltage(uint8_t Num)
{
	IIC_NCA_RESET_ON
	TCA9548A_SetChannel(Num);
	MCP3421_Init();
	return Voltage_Convert();
}

