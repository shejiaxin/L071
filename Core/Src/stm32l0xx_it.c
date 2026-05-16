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
#include "log.h"
#include "ConfigModuleNoBlock.h"
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
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
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

	  uint32_t *sp = (uint32_t *)__get_MSP(); // 获取主栈指针
    uint32_t r0 = sp[0];
    uint32_t r1 = sp[1];
    uint32_t r2 = sp[2];
    uint32_t r3 = sp[3];
    uint32_t r12 = sp[4];
    uint32_t lr = sp[5];  // 链接寄存器
    uint32_t pc = sp[6];  // 程序计数器
    uint32_t psr = sp[7]; // 程序状态寄存器
    log_info("HardFault:\n");
    log_info("R0 = %08lx\n", r0);
    log_info("R1 = %08lx\n", r1);
    log_info("R2 = %08lx\n", r2);
    log_info("R3 = %08lx\n", r3);
    log_info("R12 = %08lx\n", r12);
    log_info("LR = %08lx\n", lr);
    log_info("PC = %08lx\n", pc);
    log_info("PSR = %08lx\n", psr);
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
	extern void BSP_Timer_Handler(uint8_t ms);
	extern void BSP_Key_Handler(uint8_t ms);
  BSP_Timer_Handler(1);
	ConfigModuleNoBlockCnt ++;
//	BSP_Key_Handler(1);
	User_Data.Heart_Cnt ++;
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
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(MCU_WKUP_Pin);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
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
	if (__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)//是更新中断
	{	 			   
		LPUART1_RX_STA|=1<<15;	//标记接收完成
		__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE  );  //清除TIM7更新中断标志    
		__HAL_TIM_DISABLE(&htim7);  //关闭TIM7 
	}	   
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET)        /* UART接收过载错误中断 */
	{
			__HAL_UART_CLEAR_OREFLAG(&huart1);                           /* 清除接收过载错误中断标志 */
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
	}
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) {
      __HAL_UART_CLEAR_FEFLAG(&huart1); //清除FE帧错误标志
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET) {
      __HAL_UART_CLEAR_PEFLAG(&huart1); //清除PE奇偶校验错误标志 
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) {
      __HAL_UART_CLEAR_NEFLAG(&huart1); //清除NE噪声错误标志    
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  }
	USART1_DMAHandler();
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
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
	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET)        /* UART接收过载错误中断 */
	{
			__HAL_UART_CLEAR_OREFLAG(&hlpuart1);                           /* 清除接收过载错误中断标志 */
			(void)hlpuart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)hlpuart1.Instance->TDR;
	}
  if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) {
      __HAL_UART_CLEAR_FEFLAG(&huart1); //清除FE帧错误标志
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET) {
      __HAL_UART_CLEAR_PEFLAG(&huart1); //清除PE奇偶校验错误标志 
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  } 
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) {
      __HAL_UART_CLEAR_NEFLAG(&huart1); //清除NE噪声错误标志    
			(void)huart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
			(void)huart1.Instance->TDR;
  }
//	if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_IDLE) != RESET)        /* UART接收过载错误中断 */
//	{
//			__HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);                           /* 清除接收过载错误中断标志 */
//			(void)hlpuart1.Instance->RDR;                                   /* 先读SR寄存器，再读DR寄存器 */
//			(void)hlpuart1.Instance->TDR;
//	}  
//	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)//接收到数据
//	{	 
//		HAL_UART_Receive(&huart1, &res, 1, HAL_MAX_DELAY);		
//		if((LPUART1_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
//		{ 
//			if(LPUART1_RX_STA<LPUART1_MAX_RECV_LEN)	//还可以接收数据
//			{
//				__HAL_TIM_SET_COUNTER(&htim7,0);//计数器清空          				//计数器清空
//				if(LPUART1_RX_STA==0) 				//使能定时器7的中断 
//				{
//					__HAL_TIM_ENABLE(&htim7);//使能定时器7
//				}
//				LPUART1_Data[LPUART1_RX_STA++]=res;	//记录接收到的值	 
//			}else 
//			{
//				LPUART1_RX_STA|=1<<15;				//强制标记接收完成
//			} 
//		}
//	}  
  /* USER CODE END LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

