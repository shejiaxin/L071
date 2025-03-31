/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.h
  * @brief   This file contains all the function prototypes for
  *          the dma.c file
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
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* DMA memory to memory transfer handles -------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define UART1_RX_BUFFSIZE  200  
extern volatile uint8_t Uart1Flag ;
extern uint8_t Uart1_RxBuff[UART1_RX_BUFFSIZE] ;
extern uint8_t Uart1_Data[UART1_RX_BUFFSIZE] ;
extern volatile uint16_t uart1Cnt ;

#define LPUART1_RX_BUFFSIZE     1000
#define LPUART1_MAX_RECV_LEN 1000
extern uint16_t res[1];
extern uint16_t LPUART1_RX_STA ;
extern volatile uint8_t LPUART1Flag ;
extern uint8_t LPUART1_RxBuff[LPUART1_RX_BUFFSIZE] ;
extern uint8_t LPUART1_Data[LPUART1_RX_BUFFSIZE] ;
extern volatile uint16_t LPUART1Cnt ;


#define UART2_RX_BUFFSIZE       1000
extern volatile uint8_t Uart2Flag ;
extern uint8_t Uart2_RxBuff[UART2_RX_BUFFSIZE] ;
extern uint8_t Uart2_Data[UART2_RX_BUFFSIZE] ;
extern volatile uint16_t uart2Cnt ;
/* USER CODE END Private defines */

void MX_DMA_Init(void);

/* USER CODE BEGIN Prototypes */
//void LPUART1_DMAHandler(void);
void USART1_DMAHandler(void);
void USART2_DMAHandler(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

