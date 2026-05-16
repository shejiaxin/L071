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

 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ON_OFF_12V_Pin|EC_PWR_Pin|EC_ON_OFF_Pin|RUN_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SCL_Pin|SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IIC_NCA_RESET_GPIO_Port, IIC_NCA_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Motor_IN1_Pin|Motor_IN2_Pin|CW_ALRT_Pin|EC_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BD_LED_GPIO_Port, BD_LED_Pin, GPIO_PIN_RESET);
	
	
  GPIO_InitStruct.Pin = ON_OFF_12V_Pin|EC_PWR_Pin|EC_ON_OFF_Pin|BD_LED_Pin
                          |RUN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA3 PA5
                           PA6 PA7 PA9 PA10
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB12 PB3
                           PB4 PB5 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = SCL_Pin|SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IIC_NCA_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IIC_NCA_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ALM1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ALM1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = Motor_IN1_Pin|Motor_IN2_Pin|CW_ALRT_Pin|EC_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
		__HAL_RCC_GPIOD_CLK_DISABLE();
		
		HAL_UART_MspDeInit(&hlpuart1);
		HAL_UART_MspDeInit(&huart1);

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

static void gpioInit(void)
{
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_LPUART1_UART_Init();
	HAL_UART_MspInit(&hlpuart1);
  MX_USART1_UART_Init();	
	HAL_UART_MspInit(&huart1);

}

 void enter_stop_rtc_mode(uint32_t time)
{
		log_info("enter_stop\r\n");
		EC800_State = 0;
		ConfigModuleNoBlockFlage = 0;
    //1. 配置stop之前各外设
    system_config_before_stop();
		
    //2.设置rtc唤醒时间20s
    //stop_rtc_config(time);
    //3. 进入stop
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
		//HAL_PWR_EnterSTANDBYMode();
    //3.退出stop后恢复时钟
    clock_config_after_stop();

		//4.恢复各外设
    gpioInit();
		log_info("wake_up\r\n");
		//HAL_Delay(500);
		
		PWR12V_ON
		MCP3421_Init();
		EC20_Init();
}


