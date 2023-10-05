#ifndef MAIN_H
#define MAIN_H

//--------------------------------------------------------------------------------//
// ϵͳ�Դ��Ŀ�
						// �����ر�������
#include "stm32f10x.h"
						// string��
#include "string.h"
						// IOλ��������
#include "sys.h"
						// ��� uint1 �� bool ����
#include <stdint.h>

//--------------------------------------------------------------------------------//
// �Զ���Ŀ⺯��
						// USART1��(��DMA)
#include "usart.h"
						// USART2��(��DMA)
#include "Usart_Event.h"
						// ��ʱ����(����systick)
#include "Timer.h"
						// ϵͳ�δ�ʱ����
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
// ����Ŀ⺯��
						// MQTT�⺯��
// #include "MqttKit.h"
						// MPU6050 �ٷ� DMP �����
// #include "inv_mpu.h"
// #include "inv_mpu_dmp_motion_driver.h"

//--------------------------------------------------------------------------------//
// �ṹ������
typedef struct{
	uint8_t MQx_Flag;
	unsigned DHT11_Humi, DHT11_Temp;
	unsigned CS_LengthH, CS_LengthL;
	unsigned HX_WeightH, HX_WeightL;
} XgkData;

//--------------------------------------------------------------------------------//
// ��������

void periphInit(void);
void CS_Work(unsigned *dataHigh, unsigned *dataLow);
void DHT11_Work(unsigned *temp, unsigned *humi);
void HX711_Work(unsigned *dataHigh, unsigned *dataLow);
void MQX_work(void);
	
#endif
