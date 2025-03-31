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
#include "i2c.h"
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	//注意  MX_DMA_Init  要放在 串口初始化前面
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_RTC_Init();

  MX_LPUART1_UART_Init();
  MX_USART1_UART_Init();	
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim7); // 开启定时器中断
	
	//GetSystemClocks();
	IO1_OFF
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(ALarm_LED_GPIO_Port,ALarm_LED_Pin);
//	PWR18V_ON
//	PWR12V_ON

	Battery_EN_ON
	Adc_start();
	
  Motor_BD();
	mcu_eeprom_read(21,(uint8_t *)&User_Data.motor_0_val,4);
	mcu_eeprom_read(26,(uint8_t *)&User_Data.motor_90_val_2t,4);
	mcu_eeprom_read(31,(uint8_t *)&User_Data.motor_90_val_4t,4);
	mcu_eeprom_read(36,(uint8_t *)&User_Data.motor_180_val,4);
	mcu_eeprom_read(0,(uint8_t *)&Connect_State,1);	
	
	User_Data.Wake_time=20;
	
	MCP3421_Init();
	
	if(Connect_State == 1)
	{
		mcu_eeprom_read(41, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
    if(Mqtt_params.Mqtt_ip[0] == '\0' ||
        Mqtt_params.Mqtt_port == 0 ||
        Mqtt_params.Mqtt_client_id[0] == '\0' ||
        Mqtt_params.Mqtt_user_name[0] == '\0' ||
        Mqtt_params.Mqtt_password[0] == '\0' ||
        Mqtt_params.Sub_Topics[0] == '\0' ||
        Mqtt_params.Sub_Topics1[0] == '\0'){
        static  MqttParams param = {
            .Mqtt_ip = "81.70.28.130",
            .Mqtt_port = 1883,
            .Mqtt_client_id = "stm32",
            .Mqtt_user_name = "pub",
            .Mqtt_password = "Publish123456",
            .Sub_Topics = "P000005/D000476/report",
            .Sub_Topics1 = "P000005/D000476/command"
        };
        memcpy(&Mqtt_params, &param, sizeof(MqttParams));
      }
		LoRa_PowerON;
		EC20_Init();
		
	}
	if(Connect_State == 2){
		E22_ON;
		mcu_eeprom_read(5,(uint8_t *)&LoRaNet,sizeof(LoRaNet));
		LoRa_Init();
	}	
//	printf("%d\r\n",sizeof(LoRaNet));

	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	printf("初始化完成\r\n");
	RTC_Time();
	

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		Get_Adc_Value();
		Motor_Control();
		read_SW();
		if(Uart1Flag == 1)  //接收完成标志
		{
			U1PassiveEvent(Uart1_Data,uart1Cnt);
			Uart1Flag = 0;//清除计数
			uart1Cnt = 0;//清除接收结束标志位
			memset(Uart1_Data,0,sizeof(Uart1_Data));
		}
		//if(LPUART1Flag == 1&&EC800_State == 1)  //接收完成并且ec800初始化完成
		if((LPUART1_RX_STA&0X8000)&&EC800_State == 1)  //接收完成并且ec800初始化完成
		{
			LPUART1Cnt = LPUART1_RX_STA & 0x7FFF;
			U3PassiveEvent(LPUART1_Data,LPUART1Cnt);
			memset(LPUART1_Data,0,sizeof(LPUART1_Data));
			LPUART1_RX_STA = 0;
			LPUART1Cnt = 0;//清除接收结束标志位
		}
		if(Uart2Flag == 1){
			U2PassiveEvent(Uart2_Data,uart2Cnt);

			Uart2Flag = 0;
			uart2Cnt =0;
			memset(Uart2_Data,0,UART2_RX_BUFFSIZE);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
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

