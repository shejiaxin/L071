/*-------------------------------------------------*/
/*                                                 */
/*            操作LoRa模块功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LORA_H
#define __LORA_H

#include "stm32l0xx_hal.h"
#include "rtc.h"
typedef struct{  
	unsigned char LoRa_AddrH;         //模块地址高字节
	unsigned char LoRa_AddrL;         //模块地址低字节
	unsigned char LoRa_UartMode;      //模块串口工作模式
	unsigned char LoRa_Baudrate;      //模块串口波特率
	unsigned char LoRa_airvelocity;   //模块空中速率
	unsigned char LoRa_CH;            //模块信道
	unsigned char LoRa_DateMode;      //模块数据传输模式
	unsigned char LoRa_IOMode;        //模块TX AUX模式
	unsigned char LoRa_WakeTime;      //模块唤醒时间
	unsigned char LoRa_FECMode;       //模块FEC开关
	unsigned char LoRa_TxPower;       //模块发射功率
}LoRaParameter;

#define LoRa_MODE0  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET) //LoRa模块 模式0
#define LoRa_MODE1  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET)   //LoRa模块 模式1
#define LoRa_MODE2  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET)   //LoRa模块 模式2
#define LoRa_MODE3  HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET)     //LoRa模块 模式3

#define LoRa_AUX          HAL_GPIO_ReadPin(LORA_AUX_GPIO_Port,  LORA_AUX_Pin)                  //读取电平状态,判断模块状态
#define LoRa_PowerON      HAL_GPIO_WritePin(E22_ON_OFF_GPIO_Port, E22_ON_OFF_Pin, GPIO_PIN_SET)    //控制供电,高电平ON
#define LoRa_PowerOFF     HAL_GPIO_WritePin(E22_ON_OFF_GPIO_Port, E22_ON_OFF_Pin, GPIO_PIN_RESET)  //控制供电,低电平OFF
      
#define LoRa_TxData       u2_TxData  

#define LoRa_8N1          0x00   //模块串口参数 8数据位 无校验 1停止位
#define LoRa_8O1          0x40   //模块串口参数 8数据位 奇校验 1停止位
#define LoRa_8E1          0x80   //模块串口参数 8数据位 偶校验 1停止位

#define LoRa_1200         0x00   //模块串口 波特率 1200
#define LoRa_2400         0x20   //模块串口 波特率 2400
#define LoRa_4800         0x40   //模块串口 波特率 4800
#define LoRa_9600         0x60   //模块串口 波特率 9600
#define LoRa_19200        0x80   //模块串口 波特率 19200
#define LoRa_38400        0xa0   //模块串口 波特率 38400
#define LoRa_57600        0xc0   //模块串口 波特率 57600
#define LoRa_115200       0xe0   //模块串口 波特率 115200

#define LoRa_2_4          0x00   //模块空中速率 2.4K
#define LoRa_2_4_1        0x01   //模块空中速率 2.4K
#define LoRa_2_4_2        0x02   //模块空中速率 2.4K

#define LoRa_4_8          0x03   //模块空中速率 4.8K
#define LoRa_9_6          0x04   //模块空中速率 9.6K
#define LoRa_19_2         0x05   //模块空中速率 19.2K
#define LoRa_38_4         0x06   //模块空中速率 38.4K
#define LoRa_62_5         0x07   //模块空中速率 62.5K

#define LoRa_CH0          0x00   //模块信道 对应频率410MHz
#define LoRa_CH1          0x01   //模块信道 对应频率411MHz
#define LoRa_CH2          0x02   //模块信道 对应频率412MHz
#define LoRa_CH3          0x03   //模块信道 对应频率413MHz
#define LoRa_CH4          0x04   //模块信道 对应频率414MHz
#define LoRa_CH5          0x05   //模块信道 对应频率415MHz
#define LoRa_CH6          0x06   //模块信道 对应频率416MHz
#define LoRa_CH7          0x07   //模块信道 对应频率417MHz
#define LoRa_CH8          0x08   //模块信道 对应频率418MHz
#define LoRa_CH9          0x09   //模块信道 对应频率419MHz
#define LoRa_CH10         0x0A   //模块信道 对应频率420MHz
#define LoRa_CH11         0x0B   //模块信道 对应频率421MHz
#define LoRa_CH12         0x0C   //模块信道 对应频率422MHz
#define LoRa_CH13         0x0D   //模块信道 对应频率423MHz
#define LoRa_CH14         0x0E   //模块信道 对应频率424MHz
#define LoRa_CH15         0x0F   //模块信道 对应频率425MHz
#define LoRa_CH16         0x10   //模块信道 对应频率426MHz
#define LoRa_CH17         0x11   //模块信道 对应频率427MHz
#define LoRa_CH18         0x12   //模块信道 对应频率428MHz
#define LoRa_CH19         0x13   //模块信道 对应频率429MHz
#define LoRa_CH20         0x14   //模块信道 对应频率430MHz
#define LoRa_CH21         0x15   //模块信道 对应频率431MHz
#define LoRa_CH22         0x16   //模块信道 对应频率432MHz
#define LoRa_CH23         0x17   //模块信道 对应频率433MHz
#define LoRa_CH24         0x18   //模块信道 对应频率434MHz
#define LoRa_CH25         0x19   //模块信道 对应频率435MHz
#define LoRa_CH26         0x1A   //模块信道 对应频率436MHz
#define LoRa_CH27         0x1B   //模块信道 对应频率437MHz
#define LoRa_CH28         0x1C   //模块信道 对应频率438MHz
#define LoRa_CH29         0x1D   //模块信道 对应频率439MHz
#define LoRa_CH30         0x1E   //模块信道 对应频率440MHz
#define LoRa_CH31         0x1F   //模块信道 对应频率441MHz

#define LoRa_TRANS        0x00   //模块透明传输
#define LoRa_POINT        0x20   //模块定点传输

#define LoRa_OD           0x00   //模块TX AUX开漏
#define LoRa_UP           0x40   //模块TX AUX上拉

#define LoRa_250MS         0x00   //模块唤醒时间  250毫秒   仅模式1和2有效
#define LoRa_500MS         0x08   //模块唤醒时间  500毫秒   仅模式1和2有效
#define LoRa_750MS         0x10   //模块唤醒时间  750毫秒   仅模式1和2有效
#define LoRa_1000MS        0x18   //模块唤醒时间  1000毫秒  仅模式1和2有效
#define LoRa_1250MS        0x20   //模块唤醒时间  1250毫秒  仅模式1和2有效
#define LoRa_1500MS        0x28   //模块唤醒时间  1500毫秒  仅模式1和2有效
#define LoRa_1750MS        0x30   //模块唤醒时间  1750毫秒  仅模式1和2有效
#define LoRa_2000MS        0x38   //模块唤醒时间  2000毫秒  仅模式1和2有效

#define LoRa_FEC_OFF       0x00   //关闭FEC
#define LoRa_FEC_ON        0x04   //开启FEC

#define LoRa_FEC_33DBM     0x00   //发射功率33dbm
#define LoRa_FEC_30DBM     0x01   //发射功率30dbm
#define LoRa_FEC_27DBM     0x02   //发射功率27dbm
#define LoRa_FEC_24DBM     0x03   //发射功率24dbm


extern volatile uint8_t Connect_State;
extern LoRaParameter LoRaNet;
void LoRa_GPIO_Init(void);       				//函数声明，初始化模块的IO 
uint8_t LoRa_Reset(uint16_t timeout);     				//函数声明，LoRa复位命令
void LoRa_Init(void);         				    //函数声明，初始化模块
void LoRa_Set(void);                            //函数声明，LoRa设置工作参数
void LoRa_Get(uint8_t *data);                   //函数声明，LoRa查询工作参数    
void U2PassiveEvent(uint8_t *data, uint16_t datalen);       //函数声明，串口2被动事件   

static void lora_get_A1_data(char *data);
static void lora_get_A3_data(char *data);
static void lora_get_A4_data(char *data);

#endif


