#include "MQx.h"

/* ���Ź������ȼ��� MQx �ߣ����õ������ʱ���� MCU ���͸�λ�ź� */
// �Ƿ����ж���ʽ��
#define NVIC_MODE	1

#define printf USART2_Printf

uint8_t dangerManager = 0;

/*******************************************************************************
* �� �� ��			: MQx_Init
* ��������			: ��ʼ�� MQx ��������������
* ע������			: ����ֵ�ǵ�ǰ������ D0 Ĭ��״̬(ʹ�� GPIOA7)
*******************************************************************************/
uint8_t MQx_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_7;      		//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IPD; 		//��������	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//Ƶ��
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_4;      		//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 		//��������	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//Ƶ��
	
	GPIO_Init(GPIOB, &GPIOconstructor);
	
	PBout(4) = 0;

#if !NVIC_MODE
	return (uint8_t)PAin(7);
#else
	// �����ж�
	NVIC_InitTypeDef NVICconstructor;
	EXTI_InitTypeDef EXTIconstructor;
	
	//EXTI���ó�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
	
	EXTIconstructor.EXTI_Line = EXTI_Line7;
	EXTIconstructor.EXTI_Mode = EXTI_Mode_Interrupt;
	// �����½���ͳͳ����, 
	EXTIconstructor.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIconstructor.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTIconstructor);
	
	//NVIC��ʼ��
	
	//��λ��ռ���ȼ���û�������ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVICconstructor.NVIC_IRQChannel = EXTI9_5_IRQn;	
	
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVICconstructor.NVIC_IRQChannelSubPriority = 0x00;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVICconstructor);
	
	dangerManager = 0;
	
	return dangerManager;
#endif
}

#if !NVIC_MODE
/*******************************************************************************
* �� �� ��			: MQx_GetStatus
* ��������			: ��ȡ D0 ״̬������
* ע������			: ��Ҫ�����ʼ�����Ĭ��״̬��������
*******************************************************************************/
uint8_t MQx_GetStatus(uint8_t price){
	// δ֪������
	uint8_t flag = 2;
	uint8_t volatile status = PAin(7);
	
	if(price == (uint8_t)0x00){
		if(status == 1){
			// ����������
			flag = 1;
			printf("Flag 0 branch\r\n");
		}
		else{
			// ����������
			flag = 0;
			printf("Flag 0 branch\r\n");
		}
	}
	else if(price == (uint8_t)0x01){
		if(status == 0){
			flag = 1;
			printf("Flag 1 branch\r\n");
		}
		else{
			flag = 0;
			printf("Flag 1 branch\r\n");
		}
	}
	else{
		printf("Fatal Error: No status in MQx_GetStatus: %d\r\n", price);
	}
	
	return flag;
}

#else
/*******************************************************************************
* �� �� ��			: EXTI4_IRQHandler
* ��������			: �ⲿ�ж� 4 ����
* ע������			: ���ܴ��ر��鷳�ĺ��������ֲ��ܸ�
*******************************************************************************/
void EXTI9_5_IRQHandler(void) {
	// ��������
	Delay_ms(2);
	dangerManager = 1;
	PBout(4) = 1;
	
	// USART2_Printf("Trig occured!\r\n");
	EXTI_ClearITPendingBit(EXTI_Line7);
}
#endif
