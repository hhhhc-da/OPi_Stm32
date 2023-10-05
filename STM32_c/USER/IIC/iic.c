
#include "iic.h"

/*******************************************************************************
* �� �� ��			: IIC_Init
* ��������			: ��ʼ�� IIC �ܽ�
* ע������			: SCL - PB6�� SDA - PB7
* p.s.
* ʹ�õ������ IIC1, �ٶ������������Ժ�
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
* �� �� ��			: IIC_HardInit
* ��������			: ��ʼ�� IIC �ܽ�
* ע������			: SCL - PB6�� SDA - PB7
* p.s.
* ʹ�õ���Ӳ�� IIC1, ���ܻ��и���δ֪ bug
*******************************************************************************/
void IIC_HardInit(void)
{
	I2C_InitTypeDef I2C_InitStructure;	 // I2C�ṹ������            i2c.h����
	GPIO_InitTypeDef GPIO_InitStructure; // GPIO�ṹ������

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ʱ��ʹ��  rcc.h����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // IIC����ʱ����̬ ���ÿ�©
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// I2C
	I2C_DeInit(I2C1);														  // ��ʼ��ʹ����
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;								  // Ӧ�� ʹ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // ��ַѡ7λ
	I2C_InitStructure.I2C_ClockSpeed = 400000;								  // ʱ���ٶ�400K
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;						  // 2:1 16:9������
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;								  // I2Cģʽ
	I2C_InitStructure.I2C_OwnAddress1 = 0X10;								  // ������ַ ���д
	I2C_Init(I2C1, &I2C_InitStructure);										  // ��ʼ��I2C1
	I2C_Cmd(I2C1, ENABLE);													  // ʹ��I2C1
}

/*******************************************************************************
* �� �� ��			: IIC_Start
* ��������			: ����IIC��ʼ�ź�
* ע������			: ÿ���źſ�ʼʱ��Ҫ��������������俪ʼ
* p.s.
* IIC �����ٶ�400kb/s, ʵ���ٶ�100kb/s, ������ 1bit ԼΪ 10us, ������ʱ�� 1:1 ����
* �� 16:9 ����, ���ҿ��ǵ�������ʱ, �ó���� 4us ����ʱ
*******************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();									//sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	Delay_us(4);
 	IIC_SDA=0;									//START:when CLK is high,DATA change form high to low 
	Delay_us(4);
	IIC_SCL=0;									//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/*******************************************************************************
* �� �� ��			: IIC_Stop
* ��������			: ����IICֹͣ�ź�
* ע������			: ����м����������ݿ��Բ���ֹͣ�ź�
*******************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();									//sda�����
	IIC_SCL=0;
	Delay_us(2);
	IIC_SDA=0;									//STOP:when CLK is high DATA change form low to high
 	Delay_us(4);
	IIC_SCL=1;
	Delay_us(2);	
	IIC_SDA=1;									//����I2C���߽����ź�
	
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
* �� �� ��			: IIC_Wait_Ack
* ��������			: �ȴ�Ӧ���ź�
* ע������			: 1����Ӧ��ʧ��, 0����Ӧ��ɹ�
*******************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();									//SDA����Ϊ����  
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
	IIC_SCL=0;									//ʱ�����0 	   
	return 0;  
}

/*******************************************************************************
* �� �� ��			: IIC_Ack
* ��������			: ����ACKӦ��
* ע������			: ��
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
* �� �� ��			: IIC_NAck
* ��������			: ������ACKӦ��
* ע������			: ��
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
* �� �� ��			: IIC_Send_Byte
* ��������			: IIC����һ���ֽ�
* ע������			: 1��Ӧ��, 0��Ӧ��	
* p.s.
* ���شӻ�����Ӧ��
*******************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
		SDA_OUT(); 	    
    IIC_SCL=0;									//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		Delay_us(2);							//��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		Delay_us(2); 
		IIC_SCL=0;	
		Delay_us(2);
    }	 
}
/*******************************************************************************
* �� �� ��			: IIC_Read_Byte
* ��������			: IIC��ȡһ���ֽ�
* ע������			: ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK  
*******************************************************************************/
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();									//SDA����Ϊ����
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
			IIC_NAck();							//����nACK
	else
			IIC_Ack();							//����ACK   
	return receive;
}


/*******************************************************************************
* �� �� ��			: I2C_WriteByte
* ��������			: IIC���ַ��������
* ע������			: ��
*******************************************************************************/
/*
void I2C_WriteByte(uint16_t addr,uint8_t data,uint8_t device_addr)
{
	IIC_Start();  
	
	if(device_addr==0xA0){									//eeprom��ַ����1�ֽ�
		IIC_Send_Byte(0xA0 + ((addr/256)<<1));				//���͸ߵ�ַ
		IIC_Wait_Ack(); 
	}
	
	IIC_Send_Byte(addr&0xFF);								//���͵͵�ַ
	IIC_Wait_Ack(); 
	IIC_Send_Byte(data);									//�����ֽ�							   
  IIC_Wait_Ack();  		    	   
	IIC_Stop();											//����һ��ֹͣ���� 
	
	if(device_addr==0xA0)
		Delay_ms(10);
	else
		Delay_us(2);
}
*/
/*******************************************************************************
* �� �� ��			: I2C_ReadByte
* ��������			: IIC���ʵ�ַ����
* ע������			: ��
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

	IIC_Send_Byte(addr&0xFF);							//���͵͵�ַ
	IIC_Wait_Ack(); 
 
	IIC_Start();  	
	IIC_Send_Byte(device_addr+1);						//��������ַ
	IIC_Wait_Ack();
	if(ByteNumToRead == 1)								//LM75�¶�����Ϊ11bit
	{
		data=IIC_Read_Byte(0);
	}
	else
		{
			data=IIC_Read_Byte(1);
			data=(data<<8)+IIC_Read_Byte(0);
		}
	IIC_Stop();											//����һ��ֹͣ����	    
	return data;
}
*/
