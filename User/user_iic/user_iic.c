#include "user_iic.h"

/**
  * @brief SDA线输入模式配置
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
  * @brief SDA线输出模式配置
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
  * @brief SDA线输出一个位
  * @param val 输出的数据
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
  * @brief SCL线输出一个位
  * @param val 输出的数据
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
  * @brief SDA输入一位
  * @param None
  * @retval GPIO读入一位
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
  * @brief I2C的短暂延时
  * @param None
  * @retval None
  */
static void delay1(unsigned int n)
{
    uint32_t i;
    for ( i = 0; i < n; ++i);
}
 
/**
  * @brief I2C起始信号
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
  * @brief I2C结束信号
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
  * @brief I2C等待确认信号
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
  * @brief I2C发送确认信号
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
  * @brief I2C发送非确认信号
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
  * @brief I2C发送一个字节
  * @param cSendByte 需要发送的字节
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
  * @brief I2C接收一个字节
  * @param None
  * @retval 接收到的字节
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
 



//MCP3421写配置
void Write_MCP3421(unsigned char WR_Data)
{
  I2CStart();
   //写入地址   及 最后位为0，是写操作配置   最后位为1，是读操作配置
  I2CSendByte(0xd0);   // 1101 a2 a1 a0 0  发送给第一个字节数据 MCP3421地址字节+R/W命令
                                 // 1101 0000 0xd0 写模式
  I2CWaitAck(); 
  I2CSendByte(WR_Data); //RDY O/C C1 C0 S1 S0 G1 G0    
                       
  I2CWaitAck();
  I2CStop();  
}

//MCP3421读数据 
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
    I2CSendByte(0xd1);                    //0xd1=0b11010001, 最后一位1表示单片机接收数据
    I2CWaitAck();                      //MCP3421发出应答ACK信号
    //读取第二个字节数据 Upper Data Byte
    elech=(long int)I2CReceiveByte();       //NULL NULL NULL NULL NULL NULL D17 D16
    I2CSendAck();                             //主器件发出应答信号
    //读取第三个字节数据 Lower Data Byte
    elecm=(long int)I2CReceiveByte();       //D15 D14 D13 D12 D11 D10 D9 D8
    
    I2CSendAck();                            //  主器件发出应答信号
    
    elecl=(long int)I2CReceiveByte();       //D7 D6 D5 D4 D3 D2 D1 D0
    I2CSendAck();
    config = (long int)(I2CReceiveByte());  //RDY C1 C0 O/C S1 S0 G1 G0
    I2CSendNotAck();// 停止接收
    I2CStop();

  //开始进行数据转化
    AD_B_Result=(elech<<16)|(elecm<<8)|(elecl); //将三个数据进行整合，整合成一个24位的数据

	
//    AD_B_Result=AD_B_Result&0x0001ffff;          //由于数据elech的前6个字节没有用，所以将其清零

//		LSB = (float)(2.0 * 2.048) / 262144;  //公式 (2*Vref(2.048))/2^n n为分辨率 12,14,16,18 
//										 //18为计算为15.625uv
    PGA = 1; //增益为1
    AD_F_Result = (float)(15.625 * AD_B_Result/1000)/PGA; //输出范围 -2.048v-0v-2.048v
//		printf("AD_B_Result=%f\r\n",AD_F_Result);
    return AD_F_Result;
 }


/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief 选择通道打开
 @param channel -[in] 通道号
 @return 无
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
	uint8_t parameter = 0x9c; //系统参数配置为18位精度，自动转换模式，无PGA增益。配置字见数据手册。
	IIC_NCA_RESET_ON
	TCA9548A_SetChannel(2);
	Write_MCP3421(parameter);
}

