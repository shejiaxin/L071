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
//    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(0);
//    delay1(DELAY_TIME);
}
 
/**
  * @brief I2C结束信号
  * @param None
  * @retval None
  */
void I2CStop(void)
{
    SCL_Output(0);
//    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(1);
//    delay1(DELAY_TIME);
    SDA_Output(1);
//    delay1(DELAY_TIME);
 
}
 
/**
  * @brief I2C等待确认信号
  * @param None
  * @retval None
  */
unsigned char I2CWaitAck(void)
{
	  SDA_Output(1);
    SCL_Output(1);
    delay1(DELAY_TIME);
		SDA_Input_Mode();
    uint8_t ack = SDA_Input();
    SCL_Output(0);
		SDA_Output_Mode();
    return ack;
//    unsigned short cErrTime = 5;
//    SDA_Input_Mode();
//    delay1(DELAY_TIME);
//    SCL_Output(1);
//    delay1(DELAY_TIME);
//    while(SDA_Input())
//    {
//        cErrTime--;
//        delay1(DELAY_TIME);
//        if (0 == cErrTime)
//        {
//            SDA_Output_Mode();
//            I2CStop();
//            return ERROR;
//        }
//    }
//    SDA_Output_Mode();
//    SCL_Output(0);
//    delay1(DELAY_TIME);
//    return SUCCESS;
}
 
/**
  * @brief I2C发送确认信号
  * @param None
  * @retval None
  */
void I2CSendAck(void)
{
	  SDA_Output(0);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
//    SDA_Output(0);
//    delay1(DELAY_TIME);
//    delay1(DELAY_TIME);
//    SCL_Output(1);
//    delay1(DELAY_TIME);
//    SCL_Output(0);
//    delay1(DELAY_TIME);
 
}
 
/**
  * @brief I2C发送非确认信号
  * @param None
  * @retval None
  */
void I2CSendNotAck(void)
{
	  SDA_Output(1);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
//    SDA_Output(1);
//    delay1(DELAY_TIME);
//    delay1(DELAY_TIME);
//    SCL_Output(1);
//    delay1(DELAY_TIME);
//    SCL_Output(0);
//    delay1(DELAY_TIME);
}
 
/**
  * @brief I2C发送一个字节
  * @param cSendByte 需要发送的字节
  * @retval None
  */
void I2CSendByte(unsigned char cSendByte)
{
	  for (uint8_t i = 0; i < 8; i++) {
			if (cSendByte & 0x80)
					SDA_Output(1);
			else
					SDA_Output(0);
			delay1(DELAY_TIME);
			SCL_Output(1);
			delay1(DELAY_TIME);
			SCL_Output(0);
			cSendByte <<= 1;
    }
//    unsigned char  i = 8;
//    while (i--)
//    {
//        SCL_Output(0);
//        delay1(DELAY_TIME);
//        SDA_Output(cSendByte & 0x80);
//        delay1(DELAY_TIME);
//        cSendByte += cSendByte;
//        delay1(DELAY_TIME);
//        SCL_Output(1);
//        delay1(DELAY_TIME);
//    }
//    SCL_Output(0);
//    delay1(DELAY_TIME);
}
 
/**
  * @brief I2C接收一个字节
  * @param None
  * @retval 接收到的字节
  */
unsigned char I2CReceiveByte(void)
{
	  uint8_t byte = 0;
    SDA_Output(1);  // 释放 SDA
	  SDA_Input_Mode();
    for (uint8_t i = 0; i < 8; i++) {
        byte <<= 1;
        SCL_Output(1);
        delay1(DELAY_TIME);
        if (SDA_Input())
            byte |= 0x01;
        SCL_Output(0);
        delay1(DELAY_TIME);
    }
		SDA_Output_Mode();
    return byte;
//    unsigned char i = 8;
//    unsigned char cR_Byte = 0;
//    SDA_Input_Mode();
//    while (i--)
//    {
//        cR_Byte += cR_Byte;
//        SCL_Output(0);
//        delay1(DELAY_TIME);
//        delay1(DELAY_TIME);
//        SCL_Output(1);
//        delay1(DELAY_TIME);
//        cR_Byte |=  SDA_Input();
//    }
//    SCL_Output(0);
//    delay1(DELAY_TIME);
//    SDA_Output_Mode();
//    return cR_Byte;
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


 uint8_t CW2015_WriteReg(uint8_t reg, uint8_t data) {
    I2CStart();
    I2CSendByte(WRITE_CW2015); // 写入模式
		if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
    I2CSendByte(reg);
		if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
    I2CSendByte(data);
		if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
		I2CSendNotAck();
    I2CStop();
}
 
uint8_t CW2015_ReadReg(uint8_t reg) {
    uint8_t data = 0;
    I2CStart();
    I2CSendByte(WRITE_CW2015); // 写入模式
	  if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
    I2CSendByte(reg);
		if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
    I2CStart();                     // 重复起始条件
    I2CSendByte(READ_CW2015); // 读取模式		
		if (I2CWaitAck()) {
			I2CStop();
			return 0xFF;
    }
    data = I2CReceiveByte();          // 最后字节发送NACK
		I2CSendNotAck();
    I2CStop();
    return data;
}
 
 /*这个函数的作用是更新ic内的电池profile信息，一般只有在ic VDD掉电后再上电时才执行 */
unsigned char cw_update_config_info(void)
{
	uint8_t ret = 0;
	unsigned char i;
	unsigned char reset_val;
	unsigned char reg_val;
	/* make sure no in sleep mode */
//	reg_val = CW2015_ReadReg(REG_MODE);

//	if((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP)
//	{
//		return 2;
//	}
	/* update new battery info */
	for(i = 0; i < SIZE_BATINFO; i++)
	{
		reg_val = cw_bat_config_info[i];
		CW2015_WriteReg(REG_BATINFO+i, reg_val);
	}
 
	/* readback & check */
	for(i = 0; i < SIZE_BATINFO; i++)
	{
	  reg_val = CW2015_ReadReg(REG_BATINFO+i);
		/* debug_log("[cw_update]: REG[%02X] = %02X", REG_BATINFO +i, reg_val); */
		if(reg_val != cw_bat_config_info[i])
		{
			return 3;
		}
	}
	/* set cw2015/cw2013 to use new battery info */
	reg_val = CW2015_ReadReg(REG_CONFIG);

	reg_val |= CONFIG_UPDATE_FLG;   /* set UPDATE_FLAG */
	reg_val &= 0x07;                /* clear ATHD */
	reg_val |= ATHD;                /* set ATHD */
  CW2015_WriteReg(REG_CONFIG, reg_val);

	/* reset */
	reset_val = MODE_NORMAL;
	reg_val = MODE_RESTART;
	CW2015_WriteReg(REG_MODE, reg_val);

	HAL_Delay(10);  //delay 100us      
  CW2015_WriteReg(REG_MODE, reset_val);
	return 0;
}
 
/*电量计初始化函数 每次开机后要执行*/
unsigned char cw_init(void)
{
	unsigned ret;
	unsigned char i;
	unsigned char reg_val = MODE_NORMAL;
	
	/* wake up cw2015/13 from sleep mode */
  CW2015_WriteReg(REG_MODE, reg_val);
	/* check ATHD if not right */
	reg_val = CW2015_ReadReg(REG_CONFIG);

	if((reg_val & 0xf8) != ATHD)
	{
		//"the new ATHD need set"
		reg_val &= 0x07;    /* clear ATHD */
		reg_val |= ATHD;    /* set ATHD */
		CW2015_WriteReg(REG_CONFIG,reg_val);
	}
	
	/* check config_update_flag if not right */
	reg_val = CW2015_ReadReg(REG_CONFIG);

	if(!(reg_val & CONFIG_UPDATE_FLG))
	{
		//"update flag for new battery info need set"
		ret = cw_update_config_info();
		if(ret)
		{
			return ret;
		}
	}
	else
	{
		for(i = 0; i < SIZE_BATINFO; i++)
		{ 
			reg_val = CW2015_ReadReg(REG_BATINFO +i);
			 printf("[cw_init]:REG[%02X] = %02X", REG_BATINFO +i, reg_val); 
			if(cw_bat_config_info[i] != reg_val)
			{
				break;
			}
		}
		if(i != SIZE_BATINFO)
		{
			//"update flag for new battery info need set"
			ret = cw_update_config_info();
		}
	}
	HAL_Delay(1000);
	/* check SOC if not eqaul 255 */
	for (i = 0; i < 30; i++) {
		HAL_Delay(10);//delay 100ms
		reg_val = CW2015_ReadReg(REG_SOC);

		if (reg_val <= 100) 
			break;		
    }
	
    if (i >=30){
        reg_val = MODE_SLEEP;
        CW2015_WriteReg(REG_MODE, reg_val);
				printf("cw2015/cw2013 input unvalid power error_2\n");
        // "cw2015/cw2013 input unvalid power error_2\n";
        return 4;
    } 
	return 0;
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
	for(uint8_t i =0; i<3;i++){
				TCA9548A_SetChannel(i);
		Write_MCP3421(parameter);
		HAL_Delay(10);
	}
	TCA9548A_SetChannel(3);
	cw_init();
}

void MCP3421_Read(void)
{
	for(uint8_t i =0; i<4;i++){	
		TCA9548A_SetChannel(i);
		HAL_Delay(1);
    switch (i)
    {
    case  0:
      User_Data.Pressure1 = READ_MCP3421();
      break;
    case  1:
      User_Data.Pressure2 = READ_MCP3421();
      break;
    case  2:
      User_Data.Pressure3 = READ_MCP3421();
      break;
		case  3:
			User_Data.cw_capacity = CW2015_ReadReg(REG_SOC);
			User_Data.cw_capacity1 = CW2015_ReadReg(REG_SOC+1);
			User_Data.voltage = CW2015_ReadReg(REG_VCELL)<<8;
			User_Data.voltage += CW2015_ReadReg(REG_VCELL+1);
			User_Data.voltage = User_Data.voltage*304/1000;
    default:
      break;
    } 
	}
}
