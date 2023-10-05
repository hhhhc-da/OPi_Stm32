#ifndef NANOKA_UART_
#define NANOKA_UART_

#include <iostream>
#include "wiringPi.h"
#include <exception>
#include <string>
#include <vector>
#include <wiringSerial.h>
#include "libxl/include_cpp/libxl.h"
#include "picojson/picojson.h"

#define OK 0
#define ERR 1

enum Read_Type{
    READ_NONE = 0x00,
    READ_HUMI,
    READ_TEMP,
    READ_MQX,
    READ_LENGTH_HIGH,
    READ_LENGTH_LOW,
    READ_WEIGHT_HIGH,
    READ_WEIGHT_LOW
};

uint8_t GPIO_Init(void);
uint8_t outputModeSet(Read_Type type);

#endif