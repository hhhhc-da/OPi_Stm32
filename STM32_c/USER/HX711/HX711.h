#ifndef HX711_H
#define HX711_H

#include "main.h"

#define SCK_HIGH	PBout(1) = 1
#define SCK_LOW		PBout(1) = 0

#define SDA			PBin(0)

#define HX711_OFFSET	8488879
#define HX711_SCALE		1959

void HX711_Init(void);
unsigned HX711_Read(void);
uint8_t HX711_GetWeight(unsigned* pointLeft, unsigned* pointRight);

#endif
