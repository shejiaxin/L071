#ifndef __USER_IIC_H__
#define __USER_IIC_H__

#include "stm32l0xx_hal.h"    //包含需要的头文件
#include "include.h"
#include "i2c.h"

#define DELAY_TIME	20
#define MCP3421ADDR 0xd0 //MCP3421 地址

#define TCA9548A_SLAVE_ADDR         0x70

#define TCA9548A_WRITE_BIT          0x00
#define TCA9548A_READ_BIT           0x01


void TCA9548A_SetChannel(uint8_t channel);

void Write_MCP3421(unsigned char WR_Data);
float READ_MCP3421(void);

void MCP3421_Init(void);

#endif /*__USER_IIC_H__*/


