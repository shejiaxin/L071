#include "bsp_board.h"
#include "adc.h"
static struct BSP_KEY _key0;            /* 按键对象 */
static struct BSP_TIMER  _timer_led;    /* LED的闪烁timer */
static struct BSP_TIMER  _timer_adc;    /* adc采样的timer */
static struct BSP_TIMER  _timer_4G;    /* 4G模块的timer */
/**
 * @brief  板卡初始化
 * @note   用于从 APP 复位进入 bootloader 时需要对某些 GPIO 进行初始化的操作
 * @retval None
 */
void BSP_Board_Init(void)
{
		uint32_t hal_version = HAL_GetHalVersion();
    log_info("\r\n[APP] DinoHaw\r\n");
    log_info("UID: %.8X %.8X %.8X\r\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
    log_info("APP Version: V%d.%d\r\n", APP_VERSION_MAIN, APP_VERSION_SUB);
    log_info("HAL Version: V%d.%d.%d.%d\r\n", (hal_version >> 24), (uint8_t)(hal_version >> 16), (uint8_t)(hal_version >> 8), (uint8_t)hal_version);
	
		PWR12V_ON
    HAL_TIM_Base_Start_IT(&htim7); // 开启定时器中断
		SEGGER_RTT_Init();
		BSP_Timer_Init( &_timer_led, 
                    _Timer_LedFlashCallback, 
                    1000,
                    TIMER_RUN_FOREVER, 
                    TIMER_TYPE_HARDWARE);
    BSP_Timer_Start(&_timer_led);
		Adc_start();
		BSP_Timer_Init( &_timer_adc, 
                    Get_Adc_Value, 
                    1,
                    TIMER_RUN_FOREVER, 
                    TIMER_TYPE_HARDWARE);
    BSP_Timer_Start(&_timer_adc);
	
		MCP3421_Init();
		User_Data.Wake_time = 20;
		User_Data.Wake_Up_Flag = 0;
		mcu_eeprom_read(21,(uint8_t *)&User_Data.motor_0_val,4);
		mcu_eeprom_read(26,(uint8_t *)&User_Data.motor_90_val_2t,4);
		mcu_eeprom_read(31,(uint8_t *)&User_Data.motor_90_val_4t,4);
		mcu_eeprom_read(36,(uint8_t *)&User_Data.motor_180_val,4);
		mcu_eeprom_read(20,(uint8_t *)&Motor_BD_State,1);
		mcu_eeprom_read(41,(uint8_t *)&User_Data.Pressure_Range,4);
		mcu_eeprom_read(300,(uint8_t *)&User_Data.Heart_Time,4);
		if(User_Data.Heart_Time == 0){
				User_Data.Heart_Time = 10;
		}
		if(Motor_BD_State != 1){
			User_Data.motor_0_val = 1403;
			User_Data.motor_90_val_2t = 2158;
			User_Data.motor_90_val_4t = 675;
			User_Data.motor_180_val = 1474;		
		}
		
		mcu_eeprom_read(46, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
    if(Mqtt_params.Mqtt_ip[0] == '\0' ||
        Mqtt_params.Mqtt_port == 0 ||
        Mqtt_params.Mqtt_client_id[0] == '\0' ||
        Mqtt_params.Mqtt_user_name[0] == '\0' ||
        Mqtt_params.Mqtt_password[0] == '\0' ||
        Mqtt_params.Sub_Topics[0] == '\0' ||
        Mqtt_params.Sub_Topics1[0] == '\0'||
				Mqtt_params.Mqtt_port == 1883){		
        static  MqttParams param = {
            .Mqtt_ip = "43.143.179.206",
            .Mqtt_port = 11883,
            .Mqtt_client_id = "stm32",
            .Mqtt_user_name = "pub",
            .Mqtt_password = "Publish123456",
//            .Sub_Topics = "P000005/D000476/report",
//            .Sub_Topics1 = "P000005/D000476/command"
        };
        memcpy(&Mqtt_params, &param, sizeof(MqttParams));
				mcu_eeprom_write(46, (uint8_t*)&Mqtt_params, sizeof(Mqtt_params));
     }
//	  BSP_Key_Init(&_key0, (uint8_t )MCU_WKUP_Pin, _Key0_GetLevel, KEY_PRESS_HIGH);
//    BSP_Key_Register(&_key0, KEY_CLICK, _Key_EventCallback);
//    BSP_Key_Register(&_key0, KEY_LONG_PRESS, _Key_EventCallback);
//    BSP_Key_Start(&_key0);		
//		EC20_Init();
//		BSP_Timer_Init( &_timer_4G, 
//                    lte_process, 
//                    10,
//                    TIMER_RUN_FOREVER, 
//                    TIMER_TYPE_HARDWARE);
//    BSP_Timer_Start(&_timer_4G);
}


/**
 * @brief  LED闪烁周期任务
 * @note   
 * @param[in]  user_data: 用户数据
 * @retval None
 */
static void _Timer_LedFlashCallback(void *user_data)
{
    HAL_GPIO_TogglePin(BD_LED_GPIO_Port,BD_LED_Pin );
		MCP3421_Read();	
}

/**
 * @brief  读取 key0 的电平值
 * @note   
 * @retval 电平值
 */
static uint8_t _Key0_GetLevel(void)
{
    return HAL_GPIO_ReadPin(MCU_WKUP_GPIO_Port,MCU_WKUP_Pin);
}

/**
 * @brief  按键的事件处理
 * @note   
 * @param[in]  id: 按键的 ID
 * @param[in]  event: 按键的事件
 * @retval None
 */
static void _Key_EventCallback(uint8_t id, KEY_EVENT  event)
{
    log_info("[ key ] You just press the button[%d], event: %d\r\n\r\n", id, event);
    switch (event)
    {
        case KEY_CLICK:
        {
					log_info("KEY_CLICK \r\n");
//            update_flag = FIRMWARE_UPDATE_MAGIC_WORD;
//            _APP_Reset();
            break;
        }
        case KEY_LONG_PRESS:
        {
					log_info("KEY_LONG_PRESS \r\n");
//            update_flag = FIRMWARE_RECOVERY_MAGIC_WORD;
//            _APP_Reset();
					User_Data.Wake_Up_Flag = 1;						
            break;
        }
        default: break;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)		// 外部中断唤醒回调函数
{
	if(GPIO_Pin == MCU_WKUP_Pin)
	{
		/* Clear Wake Up Flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		log_info("=== EXTI_Callback ===\r\n");
	}
}