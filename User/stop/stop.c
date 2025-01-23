#include "include.h"
#include "stop.h"
#include "rtc.h"
#include "gpio.h"

static void system_config_before_stop(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();

    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();

    /* Select HSI as system clock source after Wake Up from Stop mode */
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

    __HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();

// /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOC, FM1_IN2_Pin|FM1_IN1_Pin|Battery_EN_Pin|RELAY_PW_ON_OFF_Pin
//                          |Motor_IN1_Pin|Motor_IN2_Pin|LORA_M1_Pin|LORA_M0_Pin
//                          |EC_RST_Pin, GPIO_PIN_RESET);

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOA, ON_OFF_12V_Pin|E22_ON_OFF_Pin|EC_PWR_Pin|EC_ON_OFF_Pin
//                          |IO_OUT1_Pin|ALarm_LED_Pin|FM2_IN1_Pin, GPIO_PIN_RESET);

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOB, ON_OFF_18V_Pin|IIC_NCA_RESET_Pin, GPIO_PIN_RESET);

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(FM2_IN2_GPIO_Port, FM2_IN2_Pin, GPIO_PIN_RESET);
	
	 
//		GPIO_InitStruct.Pin =  FM1_IN2_Pin|FM1_IN1_Pin|Battery_EN_Pin|RELAY_PW_ON_OFF_Pin
//                          |Motor_IN1_Pin|Motor_IN2_Pin|LORA_M1_Pin|LORA_M0_Pin
//                          |EC_RST_Pin;
			GPIO_InitStruct.Pin =  FM1_IN2_Pin|FM1_IN1_Pin|Battery_EN_Pin|RELAY_PW_ON_OFF_Pin
                          |Motor_IN1_Pin|Motor_IN2_Pin|EC_RST_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/*Configure GPIO PAPin  */
		GPIO_InitStruct.Pin = ON_OFF_12V_Pin|E22_ON_OFF_Pin|EC_PWR_Pin|EC_ON_OFF_Pin
                          |IO_OUT1_Pin|ALarm_LED_Pin|FM2_IN1_Pin |IO_IN1_Pin|IO_IN2_Pin|CHRG_Pin|CAT1_LoRa_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/*Configure GPIO PBPin :  */
		GPIO_InitStruct.Pin =  ON_OFF_18V_Pin|IIC_NCA_RESET_Pin|IO_in4_Pin|IO_in3_Pin|Key_S1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//LPUART1 GPIO Configuration
		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		//USART1 GPIO Configuration
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//USART2 GPIO Configuration
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		//I2C2 GPIO Configuration
		GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		/**TIM2 GPIO Configuration
		PB3     ------> TIM2_CH2
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		LoRa_MODE3;
    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
		__HAL_RCC_GPIOD_CLK_DISABLE();

}

static void stop_rtc_config(uint32_t sleep)
{
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    /*## Setting the Wake up time */
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, sleep, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}


static void clock_config_after_stop(void)
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

static void gpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//LPUART1 GPIO Configuration
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_LPUART1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//USART1 GPIO Configuration
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//USART2 GPIO Configuration
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//I2C2 GPIO Configuration
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_I2C2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/**TIM2 GPIO Configuration
	PB3     ------> TIM2_CH2
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	MX_GPIO_Init();

}

 void enter_stop_rtc_mode(uint32_t time)
{
		printf("进入休眠\r\n");
		EC800_State = 0;
    //1. 配置stop之前各外设
    system_config_before_stop();
    //2.设置rtc唤醒时间20s
    stop_rtc_config(time);
    //3. 进入stop
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
		//HAL_PWR_EnterSTANDBYMode();
    //3.退出stop后恢复时钟
    clock_config_after_stop();

		//4.恢复各外设
    gpioInit();
		printf("退出休眠\r\n");
		LoRa_PowerON;
		HAL_Delay(500);
		
		IO1_OFF
		PWR12V_ON
		EC20_Init();
}
void lora_enter_stop_rtc_mode(uint32_t time)
{
		printf("进入休眠\r\n");

   //1. 配置stop之前各外设
    system_config_before_stop();
     //2.设置rtc唤醒时间20s
    stop_rtc_config(time);
    //3. 进入stop
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
		//HAL_PWR_EnterSTANDBYMode();
    //3.退出stop后恢复时钟
    clock_config_after_stop();
		//4.恢复各外设
    gpioInit();
		printf("退出休眠\r\n");
		MX_USART1_UART_Init();
	  E22_ON;
		HAL_Delay(500);
		
		IO1_OFF
		PWR12V_ON
		LoRa_Init();
}

