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
#include "include.h"

extern TIM_HandleTypeDef htim7;

buff_manage_struct buff_manage_struct_usart1;//缓存管理变量
unsigned char Usart1ReadBuff[Usart1ReadBuffLen]={0};  //接收数据缓存

uint32_t  Usart1ReadCnt = 0;//串口接收到的数据个数
uint8_t   Usart1BufferMemory[Usart1BufferMemoryLen];//接收数据缓存区
uint32_t  Usart1BufferMemoryManage[Usart1BufferMemoryManageLen];//管理缓存区
uint8_t   Usart1BufferMemoryCopy[Usart1BufferMemoryCopyLen];//提取缓存区的数据

//接收环形队列
rb_t rb_t_usart1_read;

uint16_t LPUART1_RX_STA = 0;
uint16_t res[1]={0};
volatile uint8_t LPUART1Flag = 0;
uint8_t LPUART1_RxBuff[LPUART1_RX_BUFFSIZE] = {0};
uint8_t LPUART1_Data[LPUART1_RX_BUFFSIZE] = {0};
volatile uint16_t LPUART1Cnt = 0;



void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	 if (huart->Instance == LPUART1)
   {
		 log_info("LPUART1-error\r\n");
	 }
	 if (huart->Instance == USART1)
   {
		 log_info("USART1-error\r\n");
	 }
	 if (huart->Instance == USART2)
   {
		 log_info("USART2-error\r\n");
	 }
	 
	 

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
    if (huart->Instance == LPUART1)
    {
				 HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)res, 1);    // 使能串口接收中断
       if((LPUART1_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
				{ 
					if(LPUART1_RX_STA<(LPUART1_MAX_RECV_LEN - 1))	//还可以接收数据
					{
						__HAL_TIM_SET_COUNTER(&htim7,0);//计数器清空          				//计数器清空
						if(LPUART1_RX_STA==0) 				//使能定时器7的中断 
						{
							__HAL_TIM_ENABLE(&htim7);//使能定时器7
						}
						LPUART1_Data[LPUART1_RX_STA++]=res[0];	//记录接收到的值	 
						LPUART1_Data[LPUART1_RX_STA]='\0';
						
					}else 
					{
						LPUART1_Data[LPUART1_MAX_RECV_LEN - 1]='\0';
						LPUART1_RX_STA|=1<<15;				//强制标记接收完成
					} 
        //HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)Rxbuf, 40);    // 使能串口接收中断
				}	
    }
}

/* uart IDLE DMAHandler interrupt */
//void LPUART1_DMAHandler(void)
//{
//    if (RESET != __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE)) // 判断是否是空闲中断
//    {
//        __HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);                                     // 清除空闲中断标志
//        HAL_UART_DMAStop(&hlpuart1);                                              // 停止本次DMA传输
//        LPUART1Cnt = LPUART1_RX_BUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_lpuart1_rx); // 计算接收到的数据长度
//        memcpy(LPUART1_Data, LPUART1_RxBuff, LPUART1Cnt);
//        LPUART1Flag = 1;
//        HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)LPUART1_RxBuff, LPUART1_RX_BUFFSIZE); // 重启开始DMA传输
//    }
//}

void USART1_DMAHandler(void)
{
		int value;	
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) // 判断是否是空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);                                     // 清除空闲中断标志
        HAL_UART_DMAStop(&huart1);                                              // 停止本次DMA传输
        Usart1ReadCnt = Usart1ReadBuffLen - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); // 计算接收到的数据长度
				BufferManageWrite(&buff_manage_struct_usart1,Usart1ReadBuff,Usart1ReadCnt,&value);//把数据存入缓存
        HAL_UART_Receive_DMA(&huart1, (uint8_t *)Usart1ReadBuff, Usart1ReadBuffLen); // 重启开始DMA传输
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

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

