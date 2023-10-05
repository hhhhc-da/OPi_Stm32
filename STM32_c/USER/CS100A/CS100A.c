#include "CS100A.h"

#define printf USART2_Printf

// ʱ��Ƭ
static uint32_t time_period = 0;
// �ж���
static uint8_t mutex = 0;
// ��ѯ�¼�
static uint8_t event = 0;

/*******************************************************************************
* �� �� ��			: CS100A_Init
* ��������			: ��ʼ�� HC-SR04 ��ഫ����
* ע������			: �ܽ�����: Trig - GPIOA5, Echo - GPIOA4
*******************************************************************************/
void CSA_Init(void){
	//ʹ�ýṹ���ʼ��GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_5;      		//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 		//�������
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//Ƶ��
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	PAout(5) = 0;
	
	// �����ж�
	NVIC_InitTypeDef NVICconstructor;
	EXTI_InitTypeDef EXTIconstructor;
	
	//GPIO��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_4;
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	//EXTI���ó�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	
	EXTIconstructor.EXTI_Line = EXTI_Line4;
	EXTIconstructor.EXTI_Mode = EXTI_Mode_Interrupt;
	// �����½���ͳͳ����, 
	EXTIconstructor.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIconstructor.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTIconstructor);
	
	//NVIC��ʼ��
	
	//��λ��ռ���ȼ���û�������ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVICconstructor.NVIC_IRQChannel = EXTI4_IRQn;	
	
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVICconstructor.NVIC_IRQChannelSubPriority = 0x00;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVICconstructor);
	
	mutex = 0;
}

/*******************************************************************************
* �� �� ��			: CS100A_SendTrig
* ��������			: ���������ź�
* ע������			: ��
*******************************************************************************/
void CSA_SendTrig(void){
	// �����ź�
	PAout(5) = 1;
	Delay_us(10);
	PAout(5) = 0;
}

/*******************************************************************************
* �� �� ��			: CS100A_GetData
* ��������			: ��ȡ������Ϣ
* ע������			: ��
*******************************************************************************/
unsigned CSA_GetData(void){
	if(time_period != 0){
		unsigned us = time_period / fac_us;
		
		// USART2_Printf("Us: %u.\r\n", us);
		// ��λ��΢��
		unsigned um = 340 * us;
		
		time_period = 0;
		mutex = 0;
		
		return um;
	}
	else
		// ������
		return 0xffffffff;
}

/*******************************************************************************
* �� �� ��			: CS100A_GetOreData
* ��������			: ��ȡ���ʱ��Ƭ��Ϣ
* ע������			: ��
*******************************************************************************/
uint32_t CSA_GetOreData(void){
	if(mutex == 1){
		uint32_t data = time_period;
		
		// USART2_Printf("GetData: %x.\r\n", data);

		mutex = 0;
		
		return data;
	}
	else
		// ������
		return 0xffffffff;
}

/*******************************************************************************
* �� �� ��			: EXTI4_IRQHandler
* ��������			: �ⲿ�ж� 4 ����
* ע������			: ���ܴ��ر��鷳�ĺ��������ֲ��ܸ�
*******************************************************************************/
void EXTI4_IRQHandler(void) {
	// Systick ��ʱ����
	uint32_t ret = Chrono();
	
	if(ret != 0){
		if(mutex == 1){
			// USART2_Printf("EXTI4: Old data exists: %x.\r\n", time_period);
			// ���ھ�����ֱ�������־λȻ���˳�
			EXTI_ClearITPendingBit(EXTI_Line4);
			return;
		}
		
		time_period = ret;
		// USART2_Printf("EXTI4: New data exists: %x.\r\n", time_period);
		
		mutex = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}