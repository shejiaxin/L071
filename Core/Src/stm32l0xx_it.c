/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32l0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dma.h"
#include "usart.h"
#include "ec800.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim7;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable Interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	printf("HardFault_Handler");
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	if(User_Data.SysTick_10S_start == 1)
	User_Data.SysTick_10S++;
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RTC global interrupt through EXTI lines 17, 19 and 20 and LSE CSS interrupt through EXTI line 19.
  */
void RTC_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_IRQn 0 */

  /* USER CODE END RTC_IRQn 0 */
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_IRQn 1 */

  /* USER CODE END RTC_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 4, channel 5, channel 6 and channel 7 interrupts.
  */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 0 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 1 */

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 1 */
}

/**
  * @brief This function handles ADC, COMP1 and COMP2 interrupts (COMP interrupts through EXTI lines 21 and 22).
  */
void ADC1_COMP_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_COMP_IRQn 0 */

  /* USER CODE END ADC1_COMP_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc);
  /* USER CODE BEGIN ADC1_COMP_IRQn 1 */

  /* USER CODE END ADC1_COMP_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	if (__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)//�Ǹ����ж�
	{	 			   
		LPUART1_RX_STA|=1<<15;	//��ǽ������
		__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE  );  //���TIM7�����жϱ�־    
		__HAL_TIM_DISABLE(&htim7);  //�ر�TIM7 
	}	   
  /* USER CODE END TIM7_IRQn 0 */
  //HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET)        /* UART���չ��ش����ж� */
	{
			__HAL_UART_CLEAR_OREFLAG(&hlpuart1);                           /* ������չ��ش����жϱ�־ */
			(void)hlpuart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)hlpuart1.Instance->TDR;
	}
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) {
      __HAL_UART_CLEAR_FEFLAG(&huart1); //���FE֡�����־
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET) {
      __HAL_UART_CLEAR_PEFLAG(&huart1); //���PE��żУ������־ 
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) {
      __HAL_UART_CLEAR_NEFLAG(&huart1); //���NE���������־    
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  }
	USART1_DMAHandler();
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET)        /* UART���չ��ش����ж� */
	{
			__HAL_UART_CLEAR_OREFLAG(&hlpuart1);                           /* ������չ��ش����жϱ�־ */
			(void)hlpuart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)hlpuart1.Instance->TDR;
	}
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) {
      __HAL_UART_CLEAR_FEFLAG(&huart1); //���FE֡�����־
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET) {
      __HAL_UART_CLEAR_PEFLAG(&huart1); //���PE��żУ������־ 
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) {
      __HAL_UART_CLEAR_NEFLAG(&huart1); //���NE���������־    
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  }
	USART2_DMAHandler();
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles LPUART1 global interrupt / LPUART1 wake-up interrupt through EXTI line 28.
  */
void LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */
	
	//LPUART1_DMAHandler();
  /* USER CODE END LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&hlpuart1);
  /* USER CODE BEGIN LPUART1_IRQn 1 */
	uint8_t res;
	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET)        /* UART���չ��ش����ж� */
	{
			__HAL_UART_CLEAR_OREFLAG(&hlpuart1);                           /* ������չ��ش����жϱ�־ */
			(void)hlpuart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)hlpuart1.Instance->TDR;
	}
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) {
      __HAL_UART_CLEAR_FEFLAG(&huart1); //���FE֡�����־
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET) {
      __HAL_UART_CLEAR_PEFLAG(&huart1); //���PE��żУ������־ 
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) {
      __HAL_UART_CLEAR_NEFLAG(&huart1); //���NE���������־    
			(void)huart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
			(void)huart1.Instance->TDR;
  }
//	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE) != RESET)        /* UART���չ��ش����ж� */
//	{
//			__HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);                           /* ������չ��ش����жϱ�־ */
//			(void)hlpuart1.Instance->RDR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
//			(void)hlpuart1.Instance->TDR;
//	}  
//	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)//���յ�����
//	{	 
//		HAL_UART_Receive(&huart1, &res, 1, HAL_MAX_DELAY);		
//		if((LPUART1_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
//		{ 
//			if(LPUART1_RX_STA<LPUART1_MAX_RECV_LEN)	//�����Խ�������
//			{
//				__HAL_TIM_SET_COUNTER(&htim7,0);//���������          				//���������
//				if(LPUART1_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
//				{
//					__HAL_TIM_ENABLE(&htim7);//ʹ�ܶ�ʱ��7
//				}
//				LPUART1_Data[LPUART1_RX_STA++]=res;	//��¼���յ���ֵ	 
//			}else 
//			{
//				LPUART1_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
//			} 
//		}
//	}  
  /* USER CODE END LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

