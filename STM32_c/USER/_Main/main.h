#ifndef MAIN_H
#define MAIN_H

//--------------------------------------------------------------------------------//
// 系统自带的库
						// 开发必备环境库
#include "stm32f10x.h"
						// string库
#include "string.h"
						// IO位带操作库
#include "sys.h"
						// 解决 uint1 的 bool 类型
#include <stdint.h>

//--------------------------------------------------------------------------------//
// 自定义的库函数
						// USART1库(无DMA)
#include "usart.h"
						// USART2库(有DMA)
#include "Usart_Event.h"
						// 定时器库(基于systick)
#include "Timer.h"
						// 系统滴答定时器库
#include "systick.h"
						// IIC
#include "iic.h"
						// AT24C256
// #include "AT24C256.h"
						// OLED
#include "OLED.h"
						// MPU6050
// #include "mpu6050.h"
						// DAC
// #include "DAC.h"
						// PWM
// #include "PWM.h"
						// SG90
// #include "sg90.h"
						// WWDG
#include "wwdg.h"
						// ESP8266
// #include "esp01s.h"
						// MQTT
// #include "myMQTT.h"
						// DHT11
#include "DHT11.h"
						// MQx
#include "MQx.h"
						// CS100A
#include "CS100A.h"
						// HX711
#include "HX711.h"

//--------------------------------------------------------------------------------//
// 借鉴的库函数
						// MQTT库函数
// #include "MqttKit.h"
						// MPU6050 官方 DMP 计算包
// #include "inv_mpu.h"
// #include "inv_mpu_dmp_motion_driver.h"

//--------------------------------------------------------------------------------//
// 结构体声明
typedef struct{
	uint8_t MQx_Flag;
	unsigned DHT11_Humi, DHT11_Temp;
	unsigned CS_LengthH, CS_LengthL;
	unsigned HX_WeightH, HX_WeightL;
} XgkData;

//--------------------------------------------------------------------------------//
// 函数声明

void periphInit(void);
void CS_Work(unsigned *dataHigh, unsigned *dataLow);
void DHT11_Work(unsigned *temp, unsigned *humi);
void HX711_Work(unsigned *dataHigh, unsigned *dataLow);
void MQX_work(void);
	
#endif
