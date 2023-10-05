
#include "usart.h"
/**********************************************************************************/

#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{
	x = x; 
} 

void _ttywrch(int ch)
{
    ch = ch;
}

//printf�ض���USART1
int fputc(int ch, FILE *f) {
	//��������
	while((USART1->SR & 0x40) == 0);
	USART1->DR = (uint8_t)ch;
	return ch;
}

//֧��printf��������ѡ��use MicroLIB
int GetKey(void)
{
	while(!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1ff));
}

/**********************************************************************************/

// ������
Buffer bufs;

/*******************************************************************************
* �� �� ��			: USART1Init
* ��������			: ��ʼ��USART1
* ע������			: PA9���ͣ�PA10����
* p.s.
* �����˲���������
*******************************************************************************/
void USART1Init(unsigned int br){
	//��GPIO��USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
	
	//��ʼ��PA9-10(��˫����GPIO����)
	GPIO_InitTypeDef GPIOconstructor;
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_9;
	//�����������ģʽ
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_10;
	//��������
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	//��USART��������(����ֻ��Ҫ���Ĳ����ʣ����Դ�ST�ٷ����İ������ҵ�������׼�������Ҫ�޸�)
	USART_InitTypeDef USARTconstructor;
	
	USARTconstructor.USART_BaudRate = br;
	USARTconstructor.USART_WordLength = USART_WordLength_8b;
	USARTconstructor.USART_StopBits = USART_StopBits_1;
	USARTconstructor.USART_Parity = USART_Parity_No;
	USARTconstructor.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USARTconstructor.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//��ʼ��USART(��Ҫ������C����û������)
	USART_Init(USART1, &USARTconstructor);
	
	//�жϽ��������ź�
	NVIC_InitTypeDef NVICconstructor;
	
	NVICconstructor.NVIC_IRQChannel = USART1_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 3;
	NVICconstructor.NVIC_IRQChannelSubPriority = 3;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//ʹ��USART(������stm32f10x_usart.h�����ҵ���Ӧ�ĺ��������ڶ���������һ��ö������)
	USART_Cmd(USART1,ENABLE);
	
	//��һ���ַ�������Ľ������
	//USART_GetFlagStatus(USART1,USART_FLAG_TC);
}

/*******************************************************************************
* �� �� ��			: USART1_IRQHandler
* ��������			: USART1�жϴ�����
* ע������			: ��
*******************************************************************************/
void USART1_IRQHandler(void)
{
	//����Ƿ�������
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) != RESET)
	{
		//���������С�����
		if(bufs.index > BUFFER_LEN)
		{
			bufs.index = 0;
			uint8_t i=0;
			for(;i<BUFFER_LEN;++i)
			{
				bufs.buf[i] = 0;
			}
		}
		bufs.buf[bufs.index++] = USART_ReceiveData(USART1);
	}
}

/*******************************************************************************
* �� �� ��			: ClearBuf
* ��������			: ����USART1����������
* ע������			: ��
*******************************************************************************/
void ClearBuf(void)
{
//��պ���
#if CLEAR_RE
	printf("%s",bufs.buf);
#endif	
	bufs.index = 0;
	uint8_t i=0;
	for(;i<BUFFER_LEN;++i)
	{
		bufs.buf[i] = 0;
	}
}

/*******************************************************************************
* �� �� ��			: isReadable
* ��������			: ��ѯ�Ƿ���Զ�ȡ����
* ע������			: ��
*******************************************************************************/
unsigned isReadable(void){
	if(bufs.buf[0] != 0)
		return 1;
	return 0;
}

/*******************************************************************************
* �� �� ��			: readBuffer
* ��������			: ��ȡ��������
* ע������			: ȫ��char�Լ������
*******************************************************************************/
char *readBuffer(void){
	if(bufs.buf[0] != 0)
		return NULL;
	return bufs.buf;
}

/*******************************************************************************
* �� �� ��			: writeBuffer
* ��������			: ��ȡ��������
* ע������			: ȫ��char�Լ������
*******************************************************************************/
char *writeBuffer(char* buffer){
	printf("%s",buffer);
}