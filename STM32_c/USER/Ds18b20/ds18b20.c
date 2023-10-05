#include "ds18b20.h"
#include <stm32f10x.h>
/*******************************************************************************
 * 函 数 名			: Reset
 * 函数功能			: 启动DS18B20
 * 注意事项			: 使用前必须用
 *******************************************************************************/
uint32_t Reset()   //启动DS18B20函数
{
	uint32_t i=0;
	func_output;
	func_clear;
	func_delay_us(642);
	func_set;
	func_input;
	while(func_read)
	{
		func_delay_us(100);
		i++;
		if(i>8)
		{
			printf("数据读取失败！！1s后重试\r\n");
			func_delay_ms(1000);
			return FAILURE;//返回0
		}
	}
	return SUCCESS;//返回1
}

/*******************************************************************************
 * 函 数 名			: Write
 * 函数功能			: 写一个字节
 * 注意事项			: 无
 *******************************************************************************/
void Write(unsigned char value)
{
	func_output;
	uint8_t j=0;
	for(j=0;j<8;++j){
		if(value & 0x01){
			func_clear;
			func_delay_us(2);
			func_set;
		}
		else{
			func_clear;
		}
		func_delay_us(100);
		func_set;
		value>>=1;
		func_delay_us(2);
	}
}

/*******************************************************************************
 * 函 数 名			: Read
 * 函数功能			: 读一个字节
 * 注意事项			: 无
 *******************************************************************************/
uint8_t Read()
{
	uint8_t temp3=0, result=0;
	uint8_t j=0;
	for(j=0;j<8;j++)
	{
		func_output;
		func_clear;
		func_delay_us(1);
		func_set;
		func_delay_us(1);
		func_input;
		temp3=func_read;
		result=(result>>1)|(temp3<<7);		//低位先出
		func_delay_us(55);
	}
	return result;
}

/*******************************************************************************
 * 函 数 名			: Control
 * 函数功能			: 温度转换指令（将现在的温度写入寄存器）
 * 注意事项			: 无
 *******************************************************************************/
void Control()
{
	while(!Reset());
	func_delay_us(250);
	Write(0xCC);		//写入跳过命令
	Write(0x44);		//写入转换命令
	func_delay_ms(1);
}

/*******************************************************************************
 * 函 数 名			: _Control
 * 函数功能			: 温度读取指令（将现在的温度写入寄存器）
 * 注意事项			: 无
 *******************************************************************************/
void _Control()
{
	while(!Reset());
	func_delay_us(250);
	Write(0xCC);		//写入跳过命令
	Write(0xBE);		//读取温度命令
}

/*******************************************************************************
 * 函 数 名			: Do
 * 函数功能			: 全套流水函数
 * 注意事项			: 记得用int接收，是真实值的100倍！！
 *******************************************************************************/
int Do()
{
	uint32_t temp1=0,temp2=0;
	int num=0;
	float tp=0;
  
	Control();
	func_delay_us(250);
	_Control();

	temp1=Read();
	temp2=Read();
  
	num=temp2;		//先读高位寄存器
	num<<=8;
	num|=temp1;
  
	if(num<0) {		//按照说明书弄一弄
		num--;
		num = ~num;
		tp = num;
		num = -(tp*0.0625*100+0.5);
	} else {
		tp = num;
		num = tp*0.0625*100+0.5;
	}
	return num;
}
