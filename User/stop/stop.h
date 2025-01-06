#ifndef __STOP_H__
#define __STOP_H__

#include "stm32l0xx_hal.h"   
#include "usart.h"


void enter_stop_rtc_mode(uint32_t time);
void lora_enter_stop_rtc_mode(uint32_t time);

#endif/*__STOP_H__*/


