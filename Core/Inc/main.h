/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LORA_AUX_Pin GPIO_PIN_13
#define LORA_AUX_GPIO_Port GPIOC
#define Battery_V_Pin GPIO_PIN_2
#define Battery_V_GPIO_Port GPIOC
#define Battery_EN_Pin GPIO_PIN_3
#define Battery_EN_GPIO_Port GPIOC
#define ON_OFF_12V_Pin GPIO_PIN_0
#define ON_OFF_12V_GPIO_Port GPIOA
#define E22_ON_OFF_Pin GPIO_PIN_1
#define E22_ON_OFF_GPIO_Port GPIOA
#define EC_PWR_Pin GPIO_PIN_4
#define EC_PWR_GPIO_Port GPIOA
#define IO_IN1_Pin GPIO_PIN_5
#define IO_IN1_GPIO_Port GPIOA
#define IO_IN2_Pin GPIO_PIN_6
#define IO_IN2_GPIO_Port GPIOA
#define CHRG_Pin GPIO_PIN_7
#define CHRG_GPIO_Port GPIOA
#define RELAY1_ON_OFF_Pin GPIO_PIN_4
#define RELAY1_ON_OFF_GPIO_Port GPIOC
#define ALPS_2T_Pin GPIO_PIN_5
#define ALPS_2T_GPIO_Port GPIOC
#define ALPS_4T_Pin GPIO_PIN_0
#define ALPS_4T_GPIO_Port GPIOB
#define HALL_Pin GPIO_PIN_1
#define HALL_GPIO_Port GPIOB
#define ON_OFF_18V_Pin GPIO_PIN_2
#define ON_OFF_18V_GPIO_Port GPIOB
#define IIC_NCA_RESET_Pin GPIO_PIN_15
#define IIC_NCA_RESET_GPIO_Port GPIOB
#define ALM1_Pin GPIO_PIN_6
#define ALM1_GPIO_Port GPIOC
#define Motor_IN1_Pin GPIO_PIN_7
#define Motor_IN1_GPIO_Port GPIOC
#define Motor_IN2_Pin GPIO_PIN_8
#define Motor_IN2_GPIO_Port GPIOC
#define RELAY1_Pin GPIO_PIN_9
#define RELAY1_GPIO_Port GPIOC
#define EC_ON_OFF_Pin GPIO_PIN_8
#define EC_ON_OFF_GPIO_Port GPIOA
#define IO_OUT1_Pin GPIO_PIN_9
#define IO_OUT1_GPIO_Port GPIOA
#define CAT1_LoRa_Pin GPIO_PIN_11
#define CAT1_LoRa_GPIO_Port GPIOA
#define ALarm_LED_Pin GPIO_PIN_12
#define ALarm_LED_GPIO_Port GPIOA
#define LORA_M1_Pin GPIO_PIN_10
#define LORA_M1_GPIO_Port GPIOC
#define LORA_M0_Pin GPIO_PIN_11
#define LORA_M0_GPIO_Port GPIOC
#define EC_RST_Pin GPIO_PIN_12
#define EC_RST_GPIO_Port GPIOC

#define KEY_PB9_Pin  GPIO_PIN_9
#define KEY_PB9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
