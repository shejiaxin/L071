/*-------------------------------------------------*/
/*                                                 */
/*            操作LoRa模块功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __USER_USART_H
#define __USER_USART_H

#include "stm32l0xx_hal.h"
#include "cJSON.h"

void User_Usart_QueueTask(void);
static uint8_t sendJsonData(cJSON *json);
void respondToStatusQuery(void) ;
void respondToSever_data(void) ;
void respondToMotorQuery(void) ;
#endif


