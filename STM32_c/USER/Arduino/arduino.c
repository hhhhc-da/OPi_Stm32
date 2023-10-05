#include "arduino.h"
#include <stm32f10x.h>
/*******************************************************************************
 * 函 数 名			: delay
 * 函数功能			: 硬件毫秒延时
 * 注意事项			: 相关设置在sysick.h里
 *******************************************************************************/
void delay(unsigned ms)
{
	Delay_ms(ms);
}

/*******************************************************************************
 * 函 数 名			: delayMicroseconds
 * 函数功能			: 硬件微秒延时
 * 注意事项			: 无
 *******************************************************************************/
void delayMicroseconds(unsigned us)
{
	Delay_us(us);
}

/*******************************************************************************
 * 函 数 名			: pinMode
 * 函数功能			: 初始化管脚
 * 注意事项			: 比Arduino多了一个GPIOtype，注意！！
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
 * 函 数 名			: digitalWrite
 * 函数功能			: 设置输出高低电平
 * 注意事项			: 必须先初始化，否则无效果
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
 * 函 数 名			: digitalRead
 * 函数功能			: 读取管脚
 * 注意事项			: 必须先初始化，否则无效果
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
