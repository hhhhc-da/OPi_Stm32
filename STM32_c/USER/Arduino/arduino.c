#include "arduino.h"
#include <stm32f10x.h>
/*******************************************************************************
 * �� �� ��			: delay
 * ��������			: Ӳ��������ʱ
 * ע������			: ���������sysick.h��
 *******************************************************************************/
void delay(unsigned ms)
{
	Delay_ms(ms);
}

/*******************************************************************************
 * �� �� ��			: delayMicroseconds
 * ��������			: Ӳ��΢����ʱ
 * ע������			: ��
 *******************************************************************************/
void delayMicroseconds(unsigned us)
{
	Delay_us(us);
}

/*******************************************************************************
 * �� �� ��			: pinMode
 * ��������			: ��ʼ���ܽ�
 * ע������			: ��Arduino����һ��GPIOtype��ע�⣡��
 *******************************************************************************/
void pinMode(GC _IO, unsigned pin, unsigned mode)
{
	if (mode == 1U)
	{
		if (_IO == 0x00)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1U << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOA, &GPIOconstructor);
		}
		else if (_IO == 0x01)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1U << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOB, &GPIOconstructor);
		}
		else if (_IO == 0x02)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1U << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOC, &GPIOconstructor);
		}
		else
			return;
	}
	else if (mode == 0U)
	{
		if (_IO == 0x00)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1 << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			if (pin == 0)
				GPIOconstructor.GPIO_Mode = GPIO_Mode_IPU;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOA, &GPIOconstructor);
		}
		else if (_IO == 0x01)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1 << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_IPD;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOB, &GPIOconstructor);
		}
		else if (_IO == 0x02)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

			GPIO_InitTypeDef GPIOconstructor;

			GPIOconstructor.GPIO_Pin = (uint16_t)1 << pin;
			GPIOconstructor.GPIO_Mode = GPIO_Mode_IPD;
			GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init(GPIOC, &GPIOconstructor);
		}
		else
			return;
	}
}

/*******************************************************************************
 * �� �� ��			: digitalWrite
 * ��������			: ��������ߵ͵�ƽ
 * ע������			: �����ȳ�ʼ����������Ч��
 *******************************************************************************/
void digitalWrite(GC _IO, unsigned pin, unsigned status)
{
	if (status > 1 || pin > 15)
		return;

	if (_IO == 0x00)
	{
		PAout(pin) = status;
	}
	else if (_IO == 0x01)
	{
		PBout(pin) = status;
	}
	else if (_IO == 0x02)
	{
		PCout(pin) = status;
	}
	else
		return;
}

/*******************************************************************************
 * �� �� ��			: digitalRead
 * ��������			: ��ȡ�ܽ�
 * ע������			: �����ȳ�ʼ����������Ч��
 *******************************************************************************/
unsigned digitalRead(GC _IO, unsigned pin)
{
	if (pin > 15)
		return pin_err;

	if (_IO == 0x00)
	{
		return PAin(pin);
	}
	else if (_IO == 0x01)
	{
		return PBin(pin);
	}
	else if (_IO == 0x02)
	{
		return PCin(pin);
	}
	else
		return io_err;
}
