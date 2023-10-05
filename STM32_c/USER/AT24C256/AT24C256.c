
#include "AT24C256.h"

/*******************************************************************************
* 函 数 名			: AT24C256_WriteOneByte
* 函数功能			: 利用 IIC 写入某地址上的某一个字节
* 注意事项			: 无
*******************************************************************************/
void AT24C256_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite) {
	IIC_Start();

	if(EE_TYPE>AT24C16){
		IIC_Send_Byte(0XA0); //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
	}
	else {
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));//发送器件地址,写
	}
	
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256); //发送低地址
	IIC_Wait_Ack();
	
	
	IIC_Send_Byte(DataToWrite); //发送字节
	IIC_Wait_Ack();
	
	IIC_Stop(); //产生一个停止条件
	Delay_ms(10);
}

/*******************************************************************************
* 函 数 名			: AT24C256_ReadOneByte
* 函数功能			: 利用 IIC 读取某地址上的某一个字节
* 注意事项			: 无
*******************************************************************************/
uint8_t AT24C256_ReadOneByte(uint16_t ReadAddr) {
	uint8_t temp=0;
	IIC_Start();
	
	if(EE_TYPE>AT24C16){
		IIC_Send_Byte(0XA0); //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
	}
	else {
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));//发送器件地址,写
	}
	
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256); //发送低地址
	IIC_Wait_Ack();
	IIC_Start();
	
	IIC_Send_Byte(0XA1); //进入接收模式

	IIC_Wait_Ack();
	// NACK 有不再接收数据的意思
	temp = IIC_Read_Byte(0);
	IIC_Stop(); //产生一个停止条件
	
	return temp;
}

/*******************************************************************************
* 函 数 名			: WriteAT24C256
* 函数功能			: 利用 IIC 连续写入某地址
* 注意事项			: 无
*******************************************************************************/
void WriteAT24C256(uint16_t addr, uint8_t *buf, uint16_t len) {
	while(len--) {
		AT24C256_WriteOneByte(addr, *buf);
		addr++;
		buf++;
		
	}
}

/*******************************************************************************
* 函 数 名			: ReadAT24C256
* 函数功能			: 利用 IIC 连续读取某地址
* 注意事项			: 请勿直接用指针地址接收
*******************************************************************************/
void ReadAT24C256(uint16_t addr, uint8_t *buf, uint16_t len) {
	while(len--) {
		*buf++ = AT24C256_ReadOneByte(addr++);
	}
}

/*******************************************************************************
* 函 数 名			: pReadAT24C256MallocPointer
* 函数功能			: 利用 IIC 连续读取某地址
* 注意事项			: 记得用完之后交还内存
*******************************************************************************/
void pReadAT24C256MallocPointer(uint16_t addr, uint8_t **buf, uint16_t len) {
	uint8_t *pTmp = malloc(len), *pTmp2 = pTmp; 
	while(len--) 
		*pTmp2++ = AT24C256_ReadOneByte(addr++);
	*buf = pTmp;
}
