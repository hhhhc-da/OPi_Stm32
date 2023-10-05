#ifndef CS100A_H
#define CS100A_H

#include "main.h"

#define DHT11_OUT()	{GPIOA->CRL &= 0xf0ffffff;GPIOA->CRL |= 0x03000000;}
#define DHT11_IN()	{GPIOA->CRL &= 0xf0ffffff;GPIOA->CRL |= 0x08000000;}

void DHT11_Init(void);
uint8_t DHT11_Detect(void);
uint32_t DHT11_Read(uint8_t *flag);
uint8_t DHT11_GetData(unsigned *temp, unsigned *humi);

#endif
