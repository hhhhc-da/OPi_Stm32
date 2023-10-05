#include "mpu6050.h"

// ͬʱ��ʾ�� OLED ��
#define OLED_Display		1
#define DEBUG_MODE_MPU6050	0

#if OLED_Display
	#include "OLED.h"
#endif

#if DEBUG_MODE_MPU6050
	#define LED_TOOGLE()		{PCout(13) = !PCout(13);}
#endif

const unsigned int size = 30;
char output_string00[size], output_string01[size], output_string02[size];

/*******************************************************************************
 * �� �� ��			: MPU6050_writeData
 * ��������			: �� MPU6050 дһ���ֽ�
 * ע������			: ��
 *******************************************************************************/
void MPU6050_writeData(uint8_t addr, uint8_t data)
{
	IIC_Start();
	
	IIC_Send_Byte(MPU6050_ADDR<<1);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	
	IIC_Stop();
}

/*******************************************************************************
 * �� �� ��			: MPU6050_readData
 * ��������			: �� MPU6050 ��һ���ֽ�
 * ע������			: ��
 *******************************************************************************/
uint8_t MPU6050_readData(uint8_t addr)
{
	uint8_t receive = 0x00;
	IIC_Start();
	
	IIC_Send_Byte(MPU6050_ADDR<<1);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	
	IIC_Start();
	
	IIC_Send_Byte((MPU6050_ADDR<<1) | 1U);
	IIC_Wait_Ack();
	
	receive = IIC_Read_Byte(0);
	IIC_Wait_Ack();
	
	IIC_Stop();
	
	return receive;
}

/*******************************************************************************
 * �� �� ��			: MPU6050_Init
 * ��������			: ��ʼ�� MPU6050
 * ע������			: �ȳ�ʼ�� IIC Ŷ
 *******************************************************************************/
void MPU6050_Init(void)
{
    IIC_Init();  // I2C ��ʼ��     

    MPU6050_writeData(PWR_MGMT_1,	0x00);	//�������״̬     
    MPU6050_writeData(SMPLRT_DIV,	0x07);	//�����ǲ����ʣ�����ֵ��0x07(125Hz)     
    MPU6050_writeData(CONFIG,		0x06);	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)     
    MPU6050_writeData(GYRO_CONFIG,	0x18);	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)     
    MPU6050_writeData(ACCEL_CONFIG,	0x01);	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz) 
}

/*******************************************************************************
 * �� �� ��			: MPU6050_analysisData
 * ��������			: ������ȡ�������ݲ��ϳ���������
 * ע������			: ��
 *******************************************************************************/
int16_t MPU6050_analysisData(uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;
    
    Data_H = MPU6050_readData(regAddr);
    Data_L = MPU6050_readData(regAddr + 1);
    data = (Data_H << 8) | Data_L;  // �ϳ����� 
    return data;
}

/*******************************************************************************
 * �� �� ��			: MPU6050_Display
 * ��������			: ��ʾ MPU6050 ������
 * ע������			: ��
 *******************************************************************************/
void MPU6050_Display(void)
{
	/*
    // ��ӡ x, y, z ����ٶ�
    printf("ACCEL_X: %d\t", MPU6050_Get_Data(ACCEL_XOUT_H));
    printf("ACCEL_Y: %d\t", MPU6050_Get_Data(ACCEL_YOUT_H));
    printf("ACCEL_Z: %d\t", MPU6050_Get_Data(ACCEL_ZOUT_H));
    
    // ��ӡ�¶ȣ���Ҫ�����ֲ�Ĺ�ʽ����Ϊ���϶�
    printf("TEMP: %0.2f\t", MPU6050_Get_Data(TEMP_OUT_H) / 340.0 + 36.53);
  */  
	int16_t xyzData[3];
	
	xyzData[0] = MPU6050_analysisData(GYRO_XOUT_H);
	xyzData[1] = MPU6050_analysisData(GYRO_YOUT_H);
	xyzData[2] = MPU6050_analysisData(GYRO_ZOUT_H);


	sprintf(output_string00,"GYRO_X: %d", xyzData[0]);
	sprintf(output_string01,"GYRO_Y: %d", xyzData[1]);
	sprintf(output_string02,"GYRO_Z: %d", xyzData[2]);

    /* ��ӡ x, y, z ����ٶ� */
    USART2_Printf("%s\t%s\t%s\r\n",output_string00,output_string01,output_string02);
	
#if OLED_Display
	
	OLED_ShowStr(10,5,(uint8_t *)output_string00,1);
	OLED_ShowStr(10,6,(uint8_t *)output_string01,1);
	OLED_ShowStr(10,7,(uint8_t *)output_string02,1);
	
#endif
}

/*******************************************************************************
 * �� �� ��			: MPU6050_getAngels
 * ��������			: ��� MPU6050 ������
 * ע������			: ��
 *******************************************************************************/
void MPU6050_getAngels(uint16_t* x, uint16_t* y,uint16_t* z)
{
	*x = MPU6050_analysisData(GYRO_XOUT_H);
	*y = MPU6050_analysisData(GYRO_YOUT_H);
	*z = MPU6050_analysisData(GYRO_ZOUT_H);
	
	sprintf(output_string00,"GYRO_X: %d", *x);
	sprintf(output_string01,"GYRO_Y: %d", *y);
	sprintf(output_string02,"GYRO_Z: %d", *z);

    /* ��ӡ x, y, z ����ٶ� */
    // USART2_Printf("%s\t%s\t%s\r\n",output_string00,output_string01,output_string02);
}

/*******************************************************************************
 * �� �� ��			: MPU6050_writeDataByLength
 * ��������			: ��MPU6050������д����
 * 
 * @params	addr	: MPU6050��ַ
 * 			reg		: MPU6050Ҫ���ʵļĴ�����ַ
 * 			len		: Ҫ����д�ĳ���
 * 			buf		: д�����ݵ�ͷָ��
 * @return	ACK_SUCC: �ɹ���0
 * 			ACK_TOUT: ʧ�ܣ�1
 *
 * ע������			: ��
 *******************************************************************************/
uint8_t MPU6050_writeDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	IIC_Start();
	
	// Ѱ�� MPU6050
	IIC_Send_Byte(MPU6050_ADDR<<1);
	
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return ACK_TOUT;
	}
	
	// ��ַ
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	
	for (i = 0; i < len; i++)
	{
		IIC_Send_Byte(buf[i]);
		
		if (IIC_Wait_Ack())
		{
			IIC_Stop();
			return ACK_TOUT;
		}
	}
	
	IIC_Stop();
	
	return ACK_SUCC;
}

/*******************************************************************************
 * �� �� ��			: MPU6050_readDataByLength
 * ��������			: ��MPU6050������������
 * 
 * @params	addr	: MPU6050��ַ
 * 			reg		: MPU6050Ҫ���ʵļĴ�����ַ
 * 			len		: Ҫ�������ĳ���
 * 			buf		: �������ݵ�ͷָ��
 * @return	ACK_SUCC: �ɹ���0
 * 			ACK_TOUT: ʧ�ܣ�1
 *
 * ע������			: ��
 *******************************************************************************/
uint8_t MPU6050_readDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start();
	
	// Ѱַ
	IIC_Send_Byte(MPU6050_ADDR<<1);

	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return ACK_TOUT;
	}
	
	// ��ַ
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	
	IIC_Start();
	
	// ��ȡ
	IIC_Send_Byte((MPU6050_ADDR<<1) | 1U);

	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return ACK_TOUT;
	}
	while (len)
	{
		if (len == 1)
			*buf = IIC_Read_Byte(0);
		else
			*buf = IIC_Read_Byte(1);
		buf++;
		len--;
	}
	
	IIC_Stop();
	return ACK_SUCC;
}
