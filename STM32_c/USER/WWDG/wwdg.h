
#ifndef WWDG_H
#define WWDG_H

#include "main.h"

// PC13 翻转函数宏定义
#define TOGGLE() {PCout(13)=!PCout(13);}

#define Print 		USART2_Printf

extern uint64_t wwdg_all_ticks;
extern const uint8_t pre_cnt;

void wwdgInit(void);
void WWDG_NVIC_Init(void);
void WWDG_IRQHandler(void);
void wDelay_ms(unsigned ms);
uint64_t getTime(void);
void IDLE();

double wChrono(void);

typedef void (*cb)(void);
extern cb Delay_callBackFunction;

#endif
