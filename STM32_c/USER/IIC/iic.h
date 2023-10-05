
#ifndef IIC_H
#define IIC_H

#include "main.h"

//ʹ��IIC1		����M24C02,OLED,LM75AD,HT1382			PB6,PB7
#define SDA_IN()	{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT()	{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}
 
//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 
 
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void I2C_HardInit(void);					//Ӳ��IIC��ʼ��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�
 
// void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr);
// uint16_t I2C_ReadByte(uint16_t addr,uint8_t device_addr,uint8_t ByteNumToRead);//�Ĵ�����ַ��������ַ��Ҫ�����ֽ��� 

#endif
