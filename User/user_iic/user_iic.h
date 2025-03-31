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

/*---------cw2015配置-----------------*/
#define	READ_CW2015				0xc5
#define	WRITE_CW2015			0xc4

#define REG_VERSION             0x0
#define REG_VCELL               0x2
#define REG_SOC                 0x4
#define REG_RRT_ALERT           0x6
#define REG_CONFIG              0x8
#define REG_MODE                0xA
#define REG_BATINFO             0x10

#define MODE_SLEEP_MASK         (0x3<<6)
#define MODE_SLEEP              (0x3<<6)
#define MODE_NORMAL             (0x0<<6)
#define MODE_QUICK_START        (0x3<<4)
#define MODE_RESTART            (0xf<<0)


#define CONFIG_UPDATE_FLG       (0x1<<1)
 
#define ATHD                    (0x0<<3)        //ATHD = 0%
#define UI_FULL             100

#define SIZE_BATINFO        64
static unsigned char cw_bat_config_info[SIZE_BATINFO] = {
    0x15, 0x39, 0x78, 0x3C, 0x2F, 0x4D, 0x62, 0x69, 0x64, 0x54,
    0x51, 0x59, 0x58, 0x5A, 0x61, 0x62, 0x63, 0x5C, 0x5A, 0x6A,
    0x6D, 0x72, 0x72, 0x7F, 0x83, 0x92, 0x06, 0x66, 0x38, 0x59,
    0x6F, 0x86, 0x8F, 0x9D, 0x9E, 0xA8, 0x48, 0x1F, 0xFF, 0x5B,
    0x09, 0x74, 0x18, 0x48, 0x7E, 0x93, 0x9D, 0x42, 0x4C, 0x7E,
    0x97, 0x97, 0x80, 0x64, 0x52, 0xCB, 0x2F, 0x00, 0x64, 0xA5,
    0xB5, 0xC1, 0x46, 0xAE
};

/*---------cw2015配置-----------------*/

void TCA9548A_SetChannel(uint8_t channel);

void Write_MCP3421(unsigned char WR_Data);
float READ_MCP3421(void);

void MCP3421_Init(void);
void MCP3421_Read(void);
#endif /*__USER_IIC_H__*/


