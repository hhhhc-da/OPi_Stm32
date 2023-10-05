#ifndef DS18B20_H
#define DS18B20_H

#include "arduino.h"
#include "main.h"

#define use 11

#define func_input pinMode(GPIO_A,use,INPUT)
#define func_output pinMode(GPIO_A,use,OUTPUT)

#define func_delay_us delayMicroseconds
#define func_delay_ms delay

#define func_set digitalWrite(GPIO_A,use,HIGH)
#define func_clear digitalWrite(GPIO_A,use,LOW)

#define func_read digitalRead(GPIO_A,use)

#define SUCCESS 1
#define FAILURE 0

//ds18b20�¶�ֵһ�������ֽڣ��������Ĵ��������Բ���һ�ζ�ʮ��λ���ȶ��Ͱ�λ����λ�ȳ�
#endif
