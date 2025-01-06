#ifndef CONFIGMODULENOBLOCK_H_
#define CONFIGMODULENOBLOCK_H_
#include "include.h"
#include "stm32l0xx_hal.h"

#ifndef CONFIGMODULENOBLOCK_C_
#define CONFIGMODULENOBLOCK_Ex_  extern
#else 
#define CONFIGMODULENOBLOCK_Ex_
#endif


CONFIGMODULENOBLOCK_Ex_ uint32_t ConfigModuleNoBlockCnt;   //���ú�����ʱ����
CONFIGMODULENOBLOCK_Ex_ char ConfigModuleNoBlockFlage;     //1-������ 0-δ������
CONFIGMODULENOBLOCK_Ex_ int  ConfigModuleNoBlockCaseValue; //����ִ����һ��Case ���

CONFIGMODULENOBLOCK_Ex_ void (*ConfigConnectDispose)(char *data,int len);//����һ������ָ�����,��������ģ�鷵�ص�����

void ConfigModuleNoBlock(void);


void FunctionParseCompare(char *data,int len);//�Ա��ж��Ƿ񷵻���Ҫ������
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue);


#endif

