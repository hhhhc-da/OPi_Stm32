
#include "iic.h"

/*******************************************************************************
* 函 数 名			: IIC_Init
* 函数功能			: 初始化 IIC 管脚
* 注意事项			: SCL - PB6、 SDA - PB7
* p.s.
* 使用的是软件 IIC1, 速度略慢但兼容性好
*******************************************************************************/
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;
 
}

/*******************************************************************************
* 函 数 名			: IIC_HardInit
* 函数功能			: 初始化 IIC 管脚
* 注意事项			: SCL - PB6、 SDA - PB7
* p.s.
* 使用的是硬件 IIC1, 可能会有各类未知 bug
*******************************************************************************/
void IIC_HardInit(void)
{
	I2C_InitTypeDef I2C_InitStructure;	 // I2C结构体配置            i2c.h里面
	GPIO_InitTypeDef GPIO_InitStructure; // GPIO结构体配置

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 时钟使能  rcc.h里面
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // IIC空闲时高阻态 复用开漏
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// I2C
	I2C_DeInit(I2C1);														  // 初始化使用它
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;								  // 应答 使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 地址选7位
	I2C_InitStructure.I2C_ClockSpeed = 400000;								  // 时钟速度400K
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;						  // 2:1 16:9都可以
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;								  // I2C模式
	I2C_InitStructure.I2C_OwnAddress1 = 0X10;								  // 主机地址 随便写
	I2C_Init(I2C1, &I2C_InitStructure);										  // 初始化I2C1
	I2C_Cmd(I2C1, ENABLE);													  // 使能I2C1
}

/*******************************************************************************
* 函 数 名			: IIC_Start
* 函数功能			: 产生IIC起始信号
* 注意事项			: 每个信号开始时都要用这个开表明传输开始
* p.s.
* IIC 理论速度400kb/s, 实际速度100kb/s, 经换算 1bit 约为 10us, 而传输时按 1:1 传输
* 或 16:9 传输, 并且考虑到各类延时, 得出这个 4us 的延时
*******************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();									//sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	Delay_us(4);
 	IIC_SDA=0;									//START:when CLK is high,DATA change form high to low 
	Delay_us(4);
	IIC_SCL=0;									//钳住I2C总线，准备发送或接收数据 
}

/*******************************************************************************
* 函 数 名			: IIC_Stop
* 函数功能			: 产生IIC停止信号
* 注意事项			: 如果有继续发的数据可以不发停止信号
*******************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();									//sda线输出
	IIC_SCL=0;
	Delay_us(2);
	IIC_SDA=0;									//STOP:when CLK is high DATA change form low to high
 	Delay_us(4);
	IIC_SCL=1;
	Delay_us(2);	
	IIC_SDA=1;									//发送I2C总线结束信号
	
	while(!READ_SDA){
		IIC_SCL=0;
		IIC_SCL=1;
		Delay_us(1);
		IIC_SDA=1;
		Delay_us(1);
	}
	Delay_us(4);							   	
}

/*******************************************************************************
* 函 数 名			: IIC_Wait_Ack
* 函数功能			: 等待应答信号
* 注意事项			: 1接收应答失败, 0接收应答成功
*******************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();									//SDA设置为输入  
	IIC_SDA=1;Delay_us(1);	   
	IIC_SCL=1;Delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;									//时钟输出0 	   
	return 0;  
}

/*******************************************************************************
* 函 数 名			: IIC_Ack
* 函数功能			: 产生ACK应答
* 注意事项			: 无
*******************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	Delay_us(2);
	IIC_SDA=0;
	Delay_us(2);
	IIC_SCL=1;
	Delay_us(2);
	IIC_SCL=0;
}

/*******************************************************************************
* 函 数 名			: IIC_NAck
* 函数功能			: 不产生ACK应答
* 注意事项			: 无
*******************************************************************************/
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	Delay_us(2);
	IIC_SCL=1;
	Delay_us(2);
	IIC_SCL=0;
}
/*******************************************************************************
* 函 数 名			: IIC_Send_Byte
* 函数功能			: IIC发送一个字节
* 注意事项			: 1有应答, 0无应答	
* p.s.
* 返回从机有无应答
*******************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
		SDA_OUT(); 	    
    IIC_SCL=0;									//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		Delay_us(2);							//对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		Delay_us(2); 
		IIC_SCL=0;	
		Delay_us(2);
    }	 
}
/*******************************************************************************
* 函 数 名			: IIC_Read_Byte
* 函数功能			: IIC读取一个字节
* 注意事项			: 读1个字节，ack=1时，发送ACK，ack=0，发送nACK  
*******************************************************************************/
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();									//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    IIC_SCL=0; 
    Delay_us(2);
		IIC_SCL=1;
    receive<<=1;
    if(READ_SDA)receive++;   
		Delay_us(1); 
  }					 
	if (!ack)
			IIC_NAck();							//发送nACK
	else
			IIC_Ack();							//发送ACK   
	return receive;
}


/*******************************************************************************
* 函 数 名			: I2C_WriteByte
* 函数功能			: IIC向地址发送数据
* 注意事项			: 无
*******************************************************************************/
/*
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr)
{
	IIC_Start();  
	
	if(device_addr==0xA0){									//eeprom地址大于1字节
		IIC_Send_Byte(0xA0 + ((addr/256)<<1));				//发送高地址
		IIC_Wait_Ack(); 
	}
	
	IIC_Send_Byte(addr&0xFF);								//发送低地址
	IIC_Wait_Ack(); 
	IIC_Send_Byte(data);									//发送字节							   
  IIC_Wait_Ack();  		    	   
	IIC_Stop();											//产生一个停止条件 
	
	if(device_addr==0xA0)
		Delay_ms(10);
	else
		Delay_us(2);
}
*/
/*******************************************************************************
* 函 数 名			: I2C_ReadByte
* 函数功能			: IIC访问地址数据
* 注意事项			: 无
*******************************************************************************/
/*
uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead)
{	
	uint16_t data;
	IIC_Start();  
	if(device_addr==0xA0)
	{
		IIC_Send_Byte(0xA0 + ((addr/256)<<1));
		IIC_Wait_Ack();
	}

	IIC_Send_Byte(addr&0xFF);							//发送低地址
	IIC_Wait_Ack(); 
 
	IIC_Start();  	
	IIC_Send_Byte(device_addr+1);						//发器件地址
	IIC_Wait_Ack();
	if(ByteNumToRead == 1)								//LM75温度数据为11bit
	{
		data=IIC_Read_Byte(0);
	}
	else
		{
			data=IIC_Read_Byte(1);
			data=(data<<8)+IIC_Read_Byte(0);
		}
	IIC_Stop();											//产生一个停止条件	    
	return data;
}
*/
