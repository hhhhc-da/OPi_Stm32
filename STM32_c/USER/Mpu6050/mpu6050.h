#ifndef MPU6050_H
#define MPU6050_H

#include "iic.h"
#include <stdio.h>

#define MPU6050_ADDR	0x68

#define SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz) 
#define CONFIG 			0x1A	//低通滤波频率，典型值：0x06(5Hz) 
#define GYRO_CONFIG 	0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s) 
#define ACCEL_CONFIG 	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz) 

/* 加速度相关寄存器地址 */
#define ACCEL_XOUT_H 	0x3B 
#define ACCEL_XOUT_L 	0x3C 
#define ACCEL_YOUT_H 	0x3D 
#define ACCEL_YOUT_L 	0x3E 
#define ACCEL_ZOUT_H 	0x3F 
#define ACCEL_ZOUT_L 	0x40 

/* 温度相关寄存器地址 */
#define TEMP_OUT_H 		0x41 
#define TEMP_OUT_L 		0x42 

/* 陀螺仪相关寄存器地址 */
#define GYRO_XOUT_H 	0x43
#define GYRO_XOUT_L 	0x44 
#define GYRO_YOUT_H 	0x45 
#define GYRO_YOUT_L 	0x46 
#define GYRO_ZOUT_H 	0x47 
#define GYRO_ZOUT_L 	0x48 

#define PWR_MGMT_1 		0x6B	//电源管理，典型值：0x00(正常启用) 
#define WHO_AM_I 		0x75	//IIC地址寄存器(默认数值0x68，只读) 
#define SlaveAddress 	0xD0	//IIC写入时的地址字节数据，+1为读取 

extern const unsigned int size;
extern char output_string00[], output_string01[], output_string02[];

#define ACK_SUCC		0
#define ACK_TOUT		1

void MPU6050_writeData(uint8_t addr, uint8_t data);
uint8_t MPU6050_readData(uint8_t addr);
void MPU6050_Init(void);
int16_t MPU6050_analysisData(uint8_t regAddr);
void MPU6050_Display(void);
uint8_t MPU6050_writeDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU6050_readDataByLength(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
void MPU6050_getAngels(uint16_t* x, uint16_t* y,uint16_t* z);

#endif
