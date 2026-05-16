#ifndef __BSP_BOARD_H__
#define __BSP_BOARD_H__


#include "include.h"


void BSP_Board_Init(void);

static void _Timer_LedFlashCallback(void *user_data);
static uint8_t _Key0_GetLevel(void);
static void _Key_EventCallback(uint8_t id, KEY_EVENT  event);
#endif

