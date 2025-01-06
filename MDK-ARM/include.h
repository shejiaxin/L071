#ifndef __INCLUDE__H_
#define __INCLUDE__H_

#include <stdio.h>
#include <string.h>
#include "lora.h"	 
#include "tim.h"
#include "user_iic.h"
#include "dma.h"
#include "ec800.h"
#include "ConfigModuleNoBlock.h"
#include "control.h"
#include "modbus_crc.h"
#include "stop.h"
#include "user_usart.h"

#define E22_ON 	 HAL_GPIO_WritePin(EC_ON_OFF_GPIO_Port, EC_ON_OFF_Pin, GPIO_PIN_SET)
#define E22_OFF  HAL_GPIO_WritePin(EC_ON_OFF_GPIO_Port, EC_ON_OFF_Pin, GPIO_PIN_RESET)
#define E22_PWR_ON 	 HAL_GPIO_WritePin(EC_PWR_GPIO_Port, EC_PWR_Pin, GPIO_PIN_SET)
#define E22_PWR_OFF  HAL_GPIO_WritePin(EC_PWR_GPIO_Port, EC_PWR_Pin, GPIO_PIN_RESET)
#define E22_RST_ON 	 HAL_GPIO_WritePin(EC_RST_GPIO_Port, EC_RST_Pin, GPIO_PIN_SET)
#define E22_RST_OFF  HAL_GPIO_WritePin(EC_RST_GPIO_Port, EC_RST_Pin, GPIO_PIN_RESET)


#define Battery_EN_ON   HAL_GPIO_WritePin(Battery_EN_GPIO_Port, Battery_EN_Pin, GPIO_PIN_RESET);
#define Battery_EN_OFF  HAL_GPIO_WritePin(Battery_EN_GPIO_Port, Battery_EN_Pin, GPIO_PIN_SET);

#define IO1_ON   HAL_GPIO_WritePin(IO_OUT1_GPIO_Port, IO_OUT1_Pin, GPIO_PIN_RESET);
#define IO1_OFF  HAL_GPIO_WritePin(IO_OUT1_GPIO_Port, IO_OUT1_Pin, GPIO_PIN_SET);

#define RELAY1_ON   HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
#define RELAY1_OFF  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);


#define RELAY1_ON_OFF_ON   HAL_GPIO_WritePin(RELAY1_ON_OFF_GPIO_Port, RELAY1_ON_OFF_Pin, GPIO_PIN_SET);
#define RELAY1_ON_OFF_OFF  HAL_GPIO_WritePin(RELAY1_ON_OFF_GPIO_Port, RELAY1_ON_OFF_Pin, GPIO_PIN_RESET);


#define IO1_IN   HAL_GPIO_ReadPin(IO_IN1_GPIO_Port, IO_IN1_Pin)
#define IO2_IN   HAL_GPIO_ReadPin(IO_IN2_GPIO_Port, IO_IN2_Pin)


#define Motor_ON   HAL_GPIO_WritePin(Motor_IN1_GPIO_Port, Motor_IN1_Pin, GPIO_PIN_SET); HAL_GPIO_WritePin(Motor_IN2_GPIO_Port, Motor_IN2_Pin, GPIO_PIN_RESET);  //正转
#define Motor_OFF  HAL_GPIO_WritePin(Motor_IN1_GPIO_Port, Motor_IN1_Pin, GPIO_PIN_RESET);HAL_GPIO_WritePin(Motor_IN2_GPIO_Port, Motor_IN2_Pin, GPIO_PIN_SET); //反转
#define Motor_sleep  HAL_GPIO_WritePin(Motor_IN1_GPIO_Port, Motor_IN1_Pin, GPIO_PIN_RESET); HAL_GPIO_WritePin(Motor_IN2_GPIO_Port, Motor_IN2_Pin, GPIO_PIN_RESET);  //正转
#define Motor_brake  HAL_GPIO_WritePin(Motor_IN1_GPIO_Port, Motor_IN1_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(Motor_IN2_GPIO_Port, Motor_IN2_Pin, GPIO_PIN_SET); //反转


#define Set_Pwm1(x) __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,x);
#define Set_Pwm2(x) __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,x);
#define ALM1_IN   HAL_GPIO_ReadPin(ALM1_GPIO_Port, ALM1_Pin);

#define IIC_NCA_RESET_ON   HAL_GPIO_WritePin(IIC_NCA_RESET_GPIO_Port, IIC_NCA_RESET_Pin, GPIO_PIN_SET);
#define IIC_NCA_RESET_OFF  HAL_GPIO_WritePin(IIC_NCA_RESET_GPIO_Port, IIC_NCA_RESET_Pin, GPIO_PIN_RESET);

#define PWR18V_ON   HAL_GPIO_WritePin(ON_OFF_18V_GPIO_Port, ON_OFF_18V_Pin, GPIO_PIN_SET);
#define PWR18V_OFF  HAL_GPIO_WritePin(ON_OFF_18V_GPIO_Port, ON_OFF_18V_Pin, GPIO_PIN_RESET);
#define PWR12V_ON   HAL_GPIO_WritePin(ON_OFF_12V_GPIO_Port, ON_OFF_12V_Pin, GPIO_PIN_SET);
#define PWR12V_OFF  HAL_GPIO_WritePin(ON_OFF_12V_GPIO_Port, ON_OFF_12V_Pin, GPIO_PIN_RESET);

//#define FM_ON_OFF1_ON   HAL_GPIO_WritePin(FM_ON_OFF1_GPIO_Port, FM_ON_OFF1_Pin, GPIO_PIN_SET);
//#define FM_ON_OFF1_OFF  HAL_GPIO_WritePin(FM_ON_OFF1_GPIO_Port, FM_ON_OFF1_Pin, GPIO_PIN_RESET);
#define KEY_PB9_IN   HAL_GPIO_ReadPin(KEY_PB9_GPIO_Port, KEY_PB9_Pin)


#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE__H_ */
