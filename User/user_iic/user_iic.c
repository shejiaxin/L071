#include "user_iic.h"

/**
  * @brief SDA������ģʽ����
  * @param None
  * @retval None
  */
void SDA_Input_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
 
    GPIO_InitStructure.Pin = SDA_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 
/**
  * @brief SDA�����ģʽ����
  * @param None
  * @retval None
  */
void SDA_Output_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
 
    GPIO_InitStructure.Pin = SDA_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 
/**
  * @brief SDA�����һ��λ
  * @param val ���������
  * @retval None
  */
void SDA_Output( uint16_t val )
{
    if ( val )
    {
        GPIOB->BSRR |= SDA_Pin;
    }
    else
    {
        GPIOB->BRR |= SDA_Pin;
    }
}
 
/**
  * @brief SCL�����һ��λ
  * @param val ���������
  * @retval None
  */
void SCL_Output( uint16_t val )
{
    if ( val )
    {
        GPIOB->BSRR |= SCL_Pin;
    }
    else
    {
        GPIOB->BRR |= SCL_Pin;
    }
}
 
/**
  * @brief SDA����һλ
  * @param None
  * @retval GPIO����һλ
  */
uint8_t SDA_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB, SDA_Pin) == GPIO_PIN_SET){
		return 1;
	}else{
		return 0;
	}
}
 
 
/**
  * @brief I2C�Ķ�����ʱ
  * @param None
  * @retval None
  */
static void delay1(unsigned int n)
{
    uint32_t i;
    for ( i = 0; i < n; ++i);
}
 
/**
  * @brief I2C��ʼ�ź�
  * @param None
  * @retval None
  */
void I2CStart(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);
}
 
/**
  * @brief I2C�����ź�
  * @param None
  * @retval None
  */
void I2CStop(void)
{
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(1);
    delay1(DELAY_TIME);
 
}
 
/**
  * @brief I2C�ȴ�ȷ���ź�
  * @param None
  * @retval None
  */
unsigned char I2CWaitAck(void)
{
    unsigned short cErrTime = 5;
    SDA_Input_Mode();
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    while(SDA_Input())
    {
        cErrTime--;
        delay1(DELAY_TIME);
        if (0 == cErrTime)
        {
            SDA_Output_Mode();
            I2CStop();
            return ERROR;
        }
    }
    SDA_Output_Mode();
    SCL_Output(0);
    delay1(DELAY_TIME);
    return SUCCESS;
}
 
/**
  * @brief I2C����ȷ���ź�
  * @param None
  * @retval None
  */
void I2CSendAck(void)
{
    SDA_Output(0);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);
 
}
 
/**
  * @brief I2C���ͷ�ȷ���ź�
  * @param None
  * @retval None
  */
void I2CSendNotAck(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);
 
}
 
/**
  * @brief I2C����һ���ֽ�
  * @param cSendByte ��Ҫ���͵��ֽ�
  * @retval None
  */
void I2CSendByte(unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        SCL_Output(0);
        delay1(DELAY_TIME);
        SDA_Output(cSendByte & 0x80);
        delay1(DELAY_TIME);
        cSendByte += cSendByte;
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
}
 
/**
  * @brief I2C����һ���ֽ�
  * @param None
  * @retval ���յ����ֽ�
  */
unsigned char I2CReceiveByte(void)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode();
    while (i--)
    {
        cR_Byte += cR_Byte;
        SCL_Output(0);
        delay1(DELAY_TIME);
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
        cR_Byte |=  SDA_Input();
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output_Mode();
    return cR_Byte;
}
 



//MCP3421д����
void Write_MCP3421(unsigned char WR_Data)
{
  I2CStart();
   //д���ַ   �� ���λΪ0����д��������   ���λΪ1���Ƕ���������
  I2CSendByte(0xd0);   // 1101 a2 a1 a0 0  ���͸���һ���ֽ����� MCP3421��ַ�ֽ�+R/W����
                                 // 1101 0000 0xd0 дģʽ
  I2CWaitAck(); 
  I2CSendByte(WR_Data); //RDY O/C C1 C0 S1 S0 G1 G0    
                       
  I2CWaitAck();
  I2CStop();  
}

//MCP3421������ 
float READ_MCP3421(void)
{
	unsigned long int  elech;
	unsigned long int elecm;
	unsigned long int elecl;
	unsigned long int config;
	unsigned long int AD_B_Result;
	float AD_F_Result=0.0;

    float LSB;
    int PGA;
    I2CStart();
    I2CSendByte(0xd1);                    //0xd1=0b11010001, ���һλ1��ʾ��Ƭ����������
    I2CWaitAck();                      //MCP3421����Ӧ��ACK�ź�
    //��ȡ�ڶ����ֽ����� Upper Data Byte
    elech=(long int)I2CReceiveByte();       //NULL NULL NULL NULL NULL NULL D17 D16
    I2CSendAck();                             //����������Ӧ���ź�
    //��ȡ�������ֽ����� Lower Data Byte
    elecm=(long int)I2CReceiveByte();       //D15 D14 D13 D12 D11 D10 D9 D8
    
    I2CSendAck();                            //  ����������Ӧ���ź�
    
    elecl=(long int)I2CReceiveByte();       //D7 D6 D5 D4 D3 D2 D1 D0
    I2CSendAck();
    config = (long int)(I2CReceiveByte());  //RDY C1 C0 O/C S1 S0 G1 G0
    I2CSendNotAck();// ֹͣ����
    I2CStop();

  //��ʼ��������ת��
    AD_B_Result=(elech<<16)|(elecm<<8)|(elecl); //���������ݽ������ϣ����ϳ�һ��24λ������

	
//    AD_B_Result=AD_B_Result&0x0001ffff;          //��������elech��ǰ6���ֽ�û���ã����Խ�������

//		LSB = (float)(2.0 * 2.048) / 262144;  //��ʽ (2*Vref(2.048))/2^n nΪ�ֱ��� 12,14,16,18 
//										 //18Ϊ����Ϊ15.625uv
    PGA = 1; //����Ϊ1
    AD_F_Result = (float)(15.625 * AD_B_Result/1000)/PGA; //�����Χ -2.048v-0v-2.048v
//		printf("AD_B_Result=%f\r\n",AD_F_Result);
    return AD_F_Result;
 }


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
		if (channel > 7) return; // Invalid channel.

		data = 1<<channel;
		I2CStart();
		I2CSendByte((TCA9548A_SLAVE_ADDR << 1) | TCA9548A_WRITE_BIT);
		I2CWaitAck();
		I2CSendByte(data);
		I2CWaitAck();
		I2CStop();
}

void MCP3421_Init(void)
{
	uint8_t parameter = 0x9c; //ϵͳ��������Ϊ18λ���ȣ��Զ�ת��ģʽ����PGA���档�����ּ������ֲᡣ
	IIC_NCA_RESET_ON
	TCA9548A_SetChannel(2);
	Write_MCP3421(parameter);
}

