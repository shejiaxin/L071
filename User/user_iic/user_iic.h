#ifndef __USER_IIC_H__
#define __USER_IIC_H__

#include "stm32l0xx_hal.h"    //包含需要的头文件
#include "include.h"
#include "i2c.h"

#define MCP3421ADDR 0xd0 //MCP3421 地址

#define TCA9548A_SLAVE_ADDR         0x70

#define TCA9548A_WRITE_BIT          0x00
#define TCA9548A_READ_BIT           0x01

#define TCA9548A_CHANNEL_0          0x01
#define TCA9548A_CHANNEL_1          0x02
#define TCA9548A_CHANNEL_2          0x04
#define TCA9548A_CHANNEL_3          0x08

void TCA9548A_SetChannel(uint8_t channel);
void MCP3421_Init(void); //MCP3421初始化
uint32_t MCP3421_Get_Value(void); //采集ADC数字值
float Voltage_Convert(void); //将数字量转换为电压值支持正负显示
float Get_Valve_Voltage(uint8_t Num);
#endif /*__USER_IIC_H__*/


