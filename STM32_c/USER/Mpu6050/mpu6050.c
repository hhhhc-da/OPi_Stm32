#include "mpu6050.h"

// 同时显示在 OLED 上
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
 * 函 数 名			: MPU6050_writeData
 * 函数功能			: 向 MPU6050 写一个字节
 * 注意事项			: 无
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
 * 函 数 名			: MPU6050_readData
 * 函数功能			: 向 MPU6050 读一个字节
 * 注意事项			: 无
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
 * 函 数 名			: MPU6050_Init
 * 函数功能			: 初始化 MPU6050
 * 注意事项			: 先初始化 IIC 哦
 *******************************************************************************/
void MPU6050_Init(void)
{
    IIC_Init();  // I2C 初始化     

    MPU6050_writeData(PWR_MGMT_1,	0x00);	//解除休眠状态     
    MPU6050_writeData(SMPLRT_DIV,	0x07);	//陀螺仪采样率，典型值：0x07(125Hz)     
    MPU6050_writeData(CONFIG,		0x06);	//低通滤波频率，典型值：0x06(5Hz)     
    MPU6050_writeData(GYRO_CONFIG,	0x18);	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)     
    MPU6050_writeData(ACCEL_CONFIG,	0x01);	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz) 
}

/*******************************************************************************
 * 函 数 名			: MPU6050_analysisData
 * 函数功能			: 连续读取两个数据并合成完整数据
 * 注意事项			: 无
 *******************************************************************************/
int16_t MPU6050_analysisData(uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;
    
    Data_H = MPU6050_readData(regAddr);
    Data_L = MPU6050_readData(regAddr + 1);
    data = (Data_H << 8) | Data_L;  // 合成数据 
    return data;
}

/*******************************************************************************
 * 函 数 名			: MPU6050_Display
 * 函数功能			: 显示 MPU6050 的数据
 * 注意事项			: 无
 *******************************************************************************/
void MPU6050_Display(void)
{
	/*
    // 打印 x, y, z 轴加速度
    printf("ACCEL_X: %d\t", MPU6050_Get_Data(ACCEL_XOUT_H));
    printf("ACCEL_Y: %d\t", MPU6050_Get_Data(ACCEL_YOUT_H));
    printf("ACCEL_Z: %d\t", MPU6050_Get_Data(ACCEL_ZOUT_H));
    
    // 打印温度，需要根据手册的公式换算为摄氏度
    printf("TEMP: %0.2f\t", MPU6050_Get_Data(TEMP_OUT_H) / 340.0 + 36.53);
  */  
	int16_t xyzData[3];
	
	xyzData[0] = MPU6050_analysisData(GYRO_XOUT_H);
	xyzData[1] = MPU6050_analysisData(GYRO_YOUT_H);
	xyzData[2] = MPU6050_analysisData(GYRO_ZOUT_H);


	sprintf(output_string00,"GYRO_X: %d", xyzData[0]);
	sprintf(output_string01,"GYRO_Y: %d", xyzData[1]);
	sprintf(output_string02,"GYRO_Z: %d", xyzData[2]);

    /* 打印 x, y, z 轴角速度 */
    USART2_Printf("%s\t%s\t%s\r\n",output_string00,output_string01,output_string02);
	
#if OLED_Display
	
	OLED_ShowStr(10,5,(uint8_t *)output_string00,1);
	OLED_ShowStr(10,6,(uint8_t *)output_string01,1);
	OLED_ShowStr(10,7,(uint8_t *)output_string02,1);
	
#endif
}

/*******************************************************************************
 * 函 数 名			: MPU6050_getAngels
 * 函数功能			: 获得 MPU6050 的数据
 * 注意事项			: 无
 *******************************************************************************/
void MPU6050_getAngels(uint16_t* x, uint16_t* y,uint16_t* z)
{
	*x = MPU6050_analysisData(GYRO_XOUT_H);
	*y = MPU6050_analysisData(GYRO_YOUT_H);
	*z = MPU6050_analysisData(GYRO_ZOUT_H);
	
	sprintf(output_string00,"GYRO_X: %d", *x);
	sprintf(output_string01,"GYRO_Y: %d", *y);
	sprintf(output_string02,"GYRO_Z: %d", *z);

    /* 打印 x, y, z 轴角速度 */
    // USART2_Printf("%s\t%s\t%s\r\n",output_string00,output_string01,output_string02);
}

/*******************************************************************************
 * 函 数 名			: MPU6050_writeDataByLength
 * 函数功能			: 向MPU6050中连续写数据
 * 
 * @params	addr	: MPU6050地址
 * 			reg		: MPU6050要访问的寄存器地址
 * 			len		: 要连续写的长度
 * 			buf		: 写入数据的头指针
 * @return	ACK_SUCC: 成功，0
 * 			ACK_TOUT: 失败，1
 *
 * 注意事项			: 无
 *******************************************************************************/
uint8_t MPU6050_writeDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	IIC_Start();
	
	// 寻找 MPU6050
	IIC_Send_Byte(MPU6050_ADDR<<1);
	
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return ACK_TOUT;
	}
	
	// 地址
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
 * 函 数 名			: MPU6050_readDataByLength
 * 函数功能			: 向MPU6050中连续读数据
 * 
 * @params	addr	: MPU6050地址
 * 			reg		: MPU6050要访问的寄存器地址
 * 			len		: 要连续读的长度
 * 			buf		: 接收数据的头指针
 * @return	ACK_SUCC: 成功，0
 * 			ACK_TOUT: 失败，1
 *
 * 注意事项			: 无
 *******************************************************************************/
uint8_t MPU6050_readDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start();
	
	// 寻址
	IIC_Send_Byte(MPU6050_ADDR<<1);

	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return ACK_TOUT;
	}
	
	// 地址
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	
	IIC_Start();
	
	// 读取
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
