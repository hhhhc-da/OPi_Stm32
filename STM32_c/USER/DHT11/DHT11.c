#include "DHT11.h"

#define printf USART2_Printf

/*******************************************************************************
* �� �� ��			: DHT11_Init
* ��������			: ��ʼ�� DHT11
* ע������			: ʹ�� GPIOA6
*******************************************************************************/
void DHT11_Init(void){
	//1.�򿪿���GPIOA��ʱ��(APB2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.���ýṹ��	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_6;      	//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 	//�������	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;	//Ƶ��

	//3.��ʼ��GPIO
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIOconstructor);
	
	DHT11_OUT();
	// ��������
	PAout(6) = 1;
	PBout(5) = 0;
	PBout(6) = 0;
}

/*******************************************************************************
* �� �� ��			: DHT11_Detect
* ��������			: ��� DHT11
* ע������			: ��
*******************************************************************************/
uint8_t DHT11_Detect(void){
	// Ĭ�ϴ�����
	uint8_t flag = 1;
	PBout(6) = !PBout(6);
	
	DHT11_OUT();
	
	// �������� 18ms
	PAout(6) = 0;
	Delay_ms(20);
	
	PAout(6) = 1;
	DHT11_IN();
	Delay_us(35);
	
	uint8_t time = 0;
	while(PAin(6) == 1){
		time++;
		if(time > (uint8_t)20)
			return flag;
		
		Delay_us(2);
	}
	
	flag = 0;
	Delay_us(50);
	
	time = 0;
	while(PAin(6) == 0){
		time++;
		if(time > (uint8_t)20)
			return flag;
		
		Delay_us(2);
	}
	
	// �ȴ���������
	while(PAin(6) == 1);
	
	return flag;
}

/*******************************************************************************
* �� �� ��			: DHT11_Read
* ��������			: ��ȡ DHT11
* ע������			: ��
*******************************************************************************/
uint32_t DHT11_Read(uint8_t *flag){
	uint8_t time = 0;
	
	while(DHT11_Detect()){
		time++;
		if(time > 3)
		{
			// ��ʱ������
			*flag = 1;
			return 0;
		}
		Delay_ms(500);
	}
	
	int i = 0, j = 0;
	uint8_t dataTmp = 0, dataCrc = 0;
	uint32_t result = 0;
	for(;i<4;++i){
		PBout(6) = !PBout(6);
		dataTmp = 0;
		
		// ����λ
		result <<= 8;
		
		for(j = 0;j < 8; ++j){
			dataTmp <<= 1;
			// �ȴ�����
			while(PAin(6) == 0);
			Delay_us(40);
			PBout(5) = !PBout(5);
			
			dataTmp |= PAin(6);
			
			while(PAin(6) == 1);
		}
		
		result |= dataTmp;
		// У����
		dataCrc += dataTmp;
		
		// ͬ��β�ź�
		while(PAin(6) == 1);
		while(PAin(6) == 0);
	}
	
	PBout(6) = !PBout(6);
	for(j = 0;j < 8; ++j){
		dataTmp <<= 1;
		// �ȴ�����
		while(PAin(6) == 0);
		Delay_us(40);
		PBout(5) = !PBout(5);
		
		dataTmp |= PAin(6);
		
		while(PAin(6) == 1);
	}
	PBout(6) = !PBout(6);
	
	
	if(dataTmp != dataCrc){
		// У�������
		*flag = 2;
		return result;
	}
	
	// ����������
	*flag = 0;
	return result;
}

/*******************************************************************************
* �� �� ��			: DHT11_GetData
* ��������			: ���� DHT11
* ע������			: ��
*******************************************************************************/
uint8_t DHT11_GetData(unsigned *temp, unsigned *humi){
	// Ĭ�ϴ�����
	uint8_t status = 3;
	
	uint32_t oreData = DHT11_Read(&status);
	
	if(status){
		return status;
	}
	
	// USART2_Printf("OreData: %x\r\n",oreData);
	
	// �ҵ� DHT11 �¶��ȳ���ʪ�Ⱥ����ͦ�����( �Ҳ���⣿ )
	*temp = (unsigned)(((oreData >> 28)&0x0f) * 10) + (unsigned)((oreData >> 24)&0x0f);
	
	*humi = (unsigned)(((oreData >> 12)&0x0f) * 10) + (unsigned)((oreData >> 8)&0x0f);
	
	status = 0;
	return status;
}
