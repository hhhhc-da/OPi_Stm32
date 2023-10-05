#ifndef USART_EVENT_H
#define USART_EVENT_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Event.h"

// Ϊ USART2 ��������������
#define RXBUFLEN					256U
#define TXBUFLEN					256U

#define USART2_MUTEX					0

typedef struct buffer2{
	char buf[RXBUFLEN+1];
	unsigned index;
} Buffer2;

// ��Ҫ����
void DMAInit(void);
void USART2Init(unsigned int br);
void USART2_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void DMA1_Channel6_IRQHandler(void);
void USART2_send(uint8_t *arr, unsigned len);
void USART2_Printf(char *fmt, ...);
void ClearRXBuf(void);
void ClearTXBuf(void);
unsigned isUsart2Readable(void);
char *readUsart2Buffer(void);
char *writeUsart2Buffer(char* buffer);

#if USART2_MUTEX
// �ٽ���Դ����
char getUSART2(void);
char giveUSART2(void);
uint8_t detectUSART2(void);
#endif

// �ⲿ������
extern Buffer2 bufTX2;
extern Buffer2 bufRX2;
extern unsigned usart2_status;

#endif
