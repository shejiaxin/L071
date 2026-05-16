#ifndef _modbus_crc_h_
#define _modbus_crc_h_


#include "include.h"
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
u16  Modbus_CRC16( u8 *puchMsg, u16 usDataLen );


#endif

