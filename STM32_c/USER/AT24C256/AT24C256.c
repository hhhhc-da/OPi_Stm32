
#include "AT24C256.h"

/*******************************************************************************
* �� �� ��			: AT24C256_WriteOneByte
* ��������			: ���� IIC д��ĳ��ַ�ϵ�ĳһ���ֽ�
* ע������			: ��
*******************************************************************************/
void AT24C256_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite) {
	IIC_Start();

	if(EE_TYPE>AT24C16){
		IIC_Send_Byte(0XA0); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
	}
	else {
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));//����������ַ,д
	}
	
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();
	
	
	IIC_Send_Byte(DataToWrite); //�����ֽ�
	IIC_Wait_Ack();
	
	IIC_Stop(); //����һ��ֹͣ����
	Delay_ms(10);
}

/*******************************************************************************
* �� �� ��			: AT24C256_ReadOneByte
* ��������			: ���� IIC ��ȡĳ��ַ�ϵ�ĳһ���ֽ�
* ע������			: ��
*******************************************************************************/
uint8_t AT24C256_ReadOneByte(uint16_t ReadAddr) {
	uint8_t temp=0;
	IIC_Start();
	
	if(EE_TYPE>AT24C16){
		IIC_Send_Byte(0XA0); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
	}
	else {
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));//����������ַ,д
	}
	
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Start();
	
	IIC_Send_Byte(0XA1); //�������ģʽ

	IIC_Wait_Ack();
	// NACK �в��ٽ������ݵ���˼
	temp = IIC_Read_Byte(0);
	IIC_Stop(); //����һ��ֹͣ����
	
	return temp;
}

/*******************************************************************************
* �� �� ��			: WriteAT24C256
* ��������			: ���� IIC ����д��ĳ��ַ
* ע������			: ��
*******************************************************************************/
void WriteAT24C256(uint16_t addr, uint8_t *buf, uint16_t len) {
	while(len--) {
		AT24C256_WriteOneByte(addr, *buf);
		addr++;
		buf++;
		
	}
}

/*******************************************************************************
* �� �� ��			: ReadAT24C256
* ��������			: ���� IIC ������ȡĳ��ַ
* ע������			: ����ֱ����ָ���ַ����
*******************************************************************************/
void ReadAT24C256(uint16_t addr, uint8_t *buf, uint16_t len) {
	while(len--) {
		*buf++ = AT24C256_ReadOneByte(addr++);
	}
}

/*******************************************************************************
* �� �� ��			: pReadAT24C256MallocPointer
* ��������			: ���� IIC ������ȡĳ��ַ
* ע������			: �ǵ�����֮�󽻻��ڴ�
*******************************************************************************/
void pReadAT24C256MallocPointer(uint16_t addr, uint8_t **buf, uint16_t len) {
	uint8_t *pTmp = malloc(len), *pTmp2 = pTmp; 
	while(len--) 
		*pTmp2++ = AT24C256_ReadOneByte(addr++);
	*buf = pTmp;
}
