#include "HX711.h"

/*******************************************************************************
* �� �� ��			: HX711_Init
* ��������			: ��ʼ�� HX711
* ע������			: SCK - GPIOB1, DT - GPIOB0
*******************************************************************************/
void HX711_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_1;      	//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 	//�������	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;	//Ƶ��

	GPIO_Init(GPIOB, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_0;      	//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IPU; 	//��������
	
	GPIO_Init(GPIOB, &GPIOconstructor);
}

/*******************************************************************************
* �� �� ��			: HX711_Read
* ��������			: ��ȡ HX711 ԭ����
* ע������			: ��
*******************************************************************************/
unsigned HX711_Read(void){
	unsigned data = 0;
	
	// ��ʼ���ź�
	SCK_LOW;
	Delay_us(1);
	
	// �ȴ� SDA ����
	while(SDA);
	
	int i = 0;
	for(;i < 24; ++i){
		SCK_HIGH;
		Delay_us(1);
		
		data <<= 1;
		
		SCK_LOW;
		if(SDA)
			data++;
	}
	
	SCK_HIGH;
	data ^= 0x800000;
	
	Delay_us(1);
	SCK_LOW;
	
	// USART2_Printf("HX711 OreData: %x\r\n", data);
	
	return data;
}

/*******************************************************************************
* �� �� ��			: HX711_GetWeight
* ��������			: ת�� HX711 ����
* ע������			: ֻ���Ա�����λС��
*******************************************************************************/
uint8_t HX711_GetWeight(unsigned* pointLeft, unsigned* pointRight){
	unsigned oreData = HX711_Read();
	
	// ��������
	*pointLeft = (oreData - HX711_OFFSET)/HX711_SCALE;
	// ������λС��
	*pointRight = ((oreData - HX711_OFFSET)%HX711_SCALE)*100/HX711_SCALE;
	
	if(oreData < HX711_OFFSET){
		*pointLeft = 0;
		*pointRight = 0;
	}
	
	// У���Ƿ�ȫΪ0
	unsigned data = (*pointLeft) | (*pointRight);
	
	if(data > 0)
		return 0;
	else
		return 1;
}