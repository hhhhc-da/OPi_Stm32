#ifndef SYSTICK_H
#define SYSTICK_H

#include <stm32f10x.h>
#include "main.h"

void SysTick_Handler(void);
void SystickInit(void);
void Delay_ms(uint32_t nms);
void iDelay_ms(uint32_t nms);
void Delay_us(uint32_t nus);
void iDelay_us(uint32_t nus);

uint32_t Chrono(void);

extern uint32_t fac_us, fac_ms;

#endif
