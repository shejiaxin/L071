/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "include.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


void GetSystemClocks(void) {
  // 获取系统时钟配置
  uint32_t SYSCLK_Frequency = HAL_RCC_GetSysClockFreq();/*!< returns SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency= HAL_RCC_GetHCLKFreq();    /*!< returns HCLK clock frequency expressed in Hz */
  uint32_t PCLK1_Frequency= HAL_RCC_GetPCLK1Freq();   /*!< returns PCLK1 clock frequency expressed in Hz */
  uint32_t PCLK2_Frequency= HAL_RCC_GetSysClockFreq(); 
 // 这里设置一个断点，用于调试时查看频率
 //    __NOP(); // 这是一个空操作，仅用于设置断点
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  __enable_irq();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	//注意  MX_DMA_Init  要放在 串口初始化前面
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	  MX_DMA_Init();
  MX_USART1_UART_Init();
	 MX_LPUART1_UART_Init();
  MX_ADC_Init();
  MX_RTC_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	BSP_Board_Init();

	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(HAL_GPIO_ReadPin(MCU_WKUP_GPIO_Port,MCU_WKUP_Pin) == SET){
			User_Data.Wake_Up_Flag++;
				
		}
		if(User_Data.Wake_Up_Flag >= 30000){
			User_Data.Wake_Up_Flag = 0;
			enter_stop_rtc_mode(User_Data.Wake_time);
		}
		Motor_Control();
		read_SW();
		
//		if((LPUART1_RX_STA&0X8000)&&EC800_State == 1)  //接收完成并且ec800初始化完成
//		{
//			LPUART1Cnt = LPUART1_RX_STA & 0x7FFF;
//			U3PassiveEvent(LPUART1_Data,LPUART1Cnt);
//			memset(LPUART1_Data,0,sizeof(LPUART1_Data));
//			LPUART1_RX_STA = 0;
//			LPUART1Cnt = 0;//清除接收结束标志位
//		}
//		else if(User_Data.Heart_Cnt >= 30000 &&EC800_State == 1 ){		
//			memset(main_buff1,0,sizeof(main_buff1));
//			main_len=sprintf(main_buff1,"BB,%s,%d,%d,%d",User_Data.imei,User_Data.adc,User_Data.Report_motor_Type,User_Data.csq);
//			MQTT_PublishQs0(main_buff1,main_len);		
//		}
		ConfigModuleNoBlock();//配置模块
		

		
		//串口接收数据出现空闲
		if(LPUART1_RX_STA&0X8000)
		{
			LPUART1Cnt = LPUART1_RX_STA & 0x7FFF;
			if(LPUART1Cnt >= sizeof(LPUART1_Data)){
				LPUART1_Data[sizeof(LPUART1_Data) - 1] = '\0';
			}else{
				LPUART1_Data[LPUART1Cnt] = '\0';
			}
			log_info("4GSever_数据 %s 长度%d\r\n",LPUART1_Data,LPUART1Cnt);
			
			//可以在这里判断模组返回的数据有没有问题
			//如果有问题就重新配置模组
			//下面是假设模组返回了 disconnect 就需要重新配置模组
			if(strstr((char*)LPUART1_Data,"+QMTSTAT: 0,1")){
				ConfigModuleNoBlockFlage=0;
				ConfigModuleNoBlockCaseValue=0;
				EC800_State = 0;
			}
			
			if(ConfigConnectDispose != NULL){
				ConfigConnectDispose((char*)LPUART1_Data,LPUART1Cnt);
			}

			if(strstr((char *)LPUART1_Data,"CGSN")){
				strncpy(User_Data.imei,(char *)LPUART1_Data+10, 15);				
			}
			if(strstr((char *)LPUART1_Data,"CSQ")){
				char *csqPos = strstr((char *)LPUART1_Data, "+CSQ:");
					if(csqPos != NULL) {
							sscanf(csqPos, "+CSQ: %d", &User_Data.csq);
					}
			}
			if(strstr((char *)LPUART1_Data,"QCCID")){
				strncpy(User_Data.ICCID,(char *)LPUART1_Data+10, 20);					
			}
			if(strstr((char *)LPUART1_Data,"QLTS")){
				strncpy(User_Data.SIM_Time,(char *)LPUART1_Data+10, 20);				
			}
			if(strstr((char *)LPUART1_Data,"+QMTRECV: 0,0")){
				parseReceivedData((char *)LPUART1_Data);
			}
			
			
			memset(LPUART1_Data,0,sizeof(LPUART1_Data));
			LPUART1_RX_STA = 0;
			LPUART1Cnt = 0;//清除接收结束标志位
		}		
		else if(User_Data.Heart_Cnt >= User_Data.Heart_Time*1000 &&ConfigModuleNoBlockFlage == 1 ){	
			RTC_Time();			
			memset(main_buff1,0,sizeof(main_buff1));
			main_len=sprintf(main_buff1,"B5,%s,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%d,%d/%d/%d-%d:%d:%d,%d",User_Data.imei,User_Data.adc,User_Data.RW_Switch_Type1,User_Data.RW_Switch_Type2,
			User_Data.RW_Double_Type,User_Data.RW_Butterfly_Type,User_Data.Report_motor_Type,User_Data.RW_Pressure1,User_Data.RW_Pressure2,User_Data.Wake_time,
			GetData.Year, GetData.Month, GetData.Date,GetTime.Hours, GetTime.Minutes, GetTime.Seconds,User_Data.csq);
			MQTT_PublishQs0(main_buff1,main_len);		
		}
		
		/*提取缓存的数据*/
		BufferManageRead(&buff_manage_struct_usart1,Usart1BufferMemoryCopy,&buff_manage_struct_usart1.ReadLen);
		User_Usart_QueueTask();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

