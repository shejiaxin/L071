#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "stm32l0xx_hal.h"   
#include "include.h"

//=========================数据类型宏定义
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define __IO    volatile 
typedef __IO uint16_t vu16;

struct BatteryMapping {
    float voltage;
    float percentage;
};
//=========================用户根据自己的需要设置
#define EEPROM_BASE_ADDR	0x08080000	
#define EEPROM_BYTE_SIZE	0x0FFF

#define EN_INT      	__enable_irq();		//系统开全局中断
#define DIS_INT     	__disable_irq();	//系统关全局中断

extern volatile uint8_t Motor_BD_State;

void mcu_eeprom_write(uint32_t address, uint8_t *data, uint16_t len);
void mcu_eeprom_read(uint32_t address, uint8_t *buffer, uint16_t len);
void read_SW(void);
void Control(DATA data);
void Motor_Control(void);
void Motor_BD(void);

#endif/*__CONTROL_H__*/


