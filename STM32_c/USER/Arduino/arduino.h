#ifndef ARDUINO_H
#define ARDUINO_H

// stm32�⺯��
#include "stm32f10x.h"
#include "systick.h"
#include "sys.h"

// ״̬��
#define OUTPUT 1U
#define INPUT 0U

#define HIGH 1U
#define LOW 0U

// ���е������жϷ���
#define RISING EXTI_Trigger_Rising
#define FALLING EXTI_Trigger_Falling
#define RISING_FALLING EXTI_Trigger_Rising_Falling

// ö�����ͣ�ָ��Ҫ������GPIO
typedef enum GPIO_Choice
{
	GPIO_A = 0x00,
	GPIO_B = 0x01,
	GPIO_C = 0x02,
	GPIO_D = 0x03,
	GPIO_E = 0x04,
	GPIO_F = 0x05,
	GPIO_G = 0x06
} GC;

// �����룬�Ժ����ȫ�ĳɷ���unsigned
typedef enum ERR
{
	pin_err = 2U,
	mode_err = 3U,
	io_err = 4U
} Err;

// �պ���
typedef void (*ptr)(void);

// ��������
void pinMode(GC _IO, unsigned pin, unsigned mode);
void digitalWrite(GC _IO, unsigned pin, unsigned status);
unsigned digitalRead(GC _IO, unsigned pin);
void delay(unsigned ms);
void delayMicroseconds(unsigned us);

#endif
