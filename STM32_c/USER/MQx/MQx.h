#ifndef MQX_H
#define MQX_H

#include "main.h"

uint8_t MQx_Init(void);
uint8_t MQx_GetStatus(uint8_t price);

extern uint8_t dangerManager;

#endif
