#ifndef USART_H
#define USART_H

#include <stm32f10x.h>
#include <stdio.h>

#define BUFFER_LEN (uint8_t)256
#define CLEAR_RE   1

//接收函数为单次接收，第二次接收时会清空缓冲区

typedef struct buffer{
	char buf[BUFFER_LEN+1];
	uint8_t index;
} Buffer;

void USART1Init(unsigned int br);
void USART1_IRQHandler(void);
void ClearBuf(void);
unsigned isReadable(void);
char *readBuffer(void);
char *writeBuffer(char* buffer);

extern Buffer bufs;

#endif
