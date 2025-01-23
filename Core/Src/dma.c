/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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

/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim7;
volatile uint8_t Uart1Flag = 0;
uint8_t Uart1_RxBuff[UART1_RX_BUFFSIZE] = {0};
uint8_t Uart1_Data[UART1_RX_BUFFSIZE] = {0};
volatile uint16_t uart1Cnt = 0;


uint16_t LPUART1_RX_STA = 0;
uint16_t res[1]={0};
volatile uint8_t LPUART1Flag = 0;
uint8_t LPUART1_RxBuff[LPUART1_RX_BUFFSIZE] = {0};
uint8_t LPUART1_Data[LPUART1_RX_BUFFSIZE] = {0};
volatile uint16_t LPUART1Cnt = 0;

volatile uint8_t Uart2Flag = 0;
uint8_t Uart2_RxBuff[UART2_RX_BUFFSIZE] = {0};
uint8_t Uart2_Data[UART2_RX_BUFFSIZE] = {0};
volatile uint16_t uart2Cnt = 0;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	 if (huart->Instance == LPUART1)
   {
		 printf("LPUART1-error\r\n");
	 }
	 if (huart->Instance == USART1)
   {
		 printf("USART1-error\r\n");
	 }
	 if (huart->Instance == USART2)
   {
		 printf("USART2-error\r\n");
	 }
	 
	 

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
    if (huart->Instance == LPUART1)
    {
				 HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)res, 1);    // ʹ�ܴ��ڽ����ж�
       if((LPUART1_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
				{ 
					if(LPUART1_RX_STA<LPUART1_MAX_RECV_LEN)	//�����Խ�������
					{
						__HAL_TIM_SET_COUNTER(&htim7,0);//���������          				//���������
						if(LPUART1_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
						{
							__HAL_TIM_ENABLE(&htim7);//ʹ�ܶ�ʱ��7
						}
						LPUART1_Data[LPUART1_RX_STA++]=res[0];	//��¼���յ���ֵ	 
						
					}else 
					{
						LPUART1_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
					} 
        //HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)Rxbuf, 40);    // ʹ�ܴ��ڽ����ж�
				}	
    }
}

/* uart IDLE DMAHandler interrupt */
//void LPUART1_DMAHandler(void)
//{
//    if (RESET != __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
//    {
//        __HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);                                     // ��������жϱ�־
//        HAL_UART_DMAStop(&hlpuart1);                                              // ֹͣ����DMA����
//        LPUART1Cnt = LPUART1_RX_BUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_lpuart1_rx); // ������յ������ݳ���
//        memcpy(LPUART1_Data, LPUART1_RxBuff, LPUART1Cnt);
//        LPUART1Flag = 1;
//        HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)LPUART1_RxBuff, LPUART1_RX_BUFFSIZE); // ������ʼDMA����
//    }
//}

void USART1_DMAHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);                                     // ��������жϱ�־
        HAL_UART_DMAStop(&huart1);                                              // ֹͣ����DMA����
        uart1Cnt = UART1_RX_BUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); // ������յ������ݳ���
        memcpy(Uart1_Data, Uart1_RxBuff, uart1Cnt);
        Uart1Flag = 1;
        HAL_UART_Receive_DMA(&huart1, (uint8_t *)Uart1_RxBuff, UART1_RX_BUFFSIZE); // ������ʼDMA����
    }
}

void USART2_DMAHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);                                     // ��������жϱ�־
        HAL_UART_DMAStop(&huart2);                                              // ֹͣ����DMA����
        uart2Cnt = UART2_RX_BUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx); // ������յ������ݳ���
        memcpy(Uart2_Data, Uart2_RxBuff, uart2Cnt);
        Uart2Flag = 1;
        HAL_UART_Receive_DMA(&huart2, (uint8_t *)Uart2_RxBuff, UART2_RX_BUFFSIZE); // ������ʼDMA����
    }
}

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Channel4_5_6_7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

