#include "Usart_Event.h"

// ���ʱ��ʾ����������
#define CLEAR_RE   					1

// �Ƿ��� DMA
#define DMA_STATUS					0

// �Ƿ��� USART2
#define Usart2						1

// �Զ��� IRQ �Ƿ���
#define Own_Handler 				1

// �Զ����������
#define PRINT						1

// �ٽ���Դ��������
unsigned usart2_status = 0;

#if Usart2
// DMA ����״̬����1���У�0æµ��
unsigned TX_Status = 1;

Buffer2 bufTX2;
Buffer2 bufRX2;

#if DMA_STATUS>0

// DMA ��ʼ����
DMA_InitTypeDef DMAconstructor;

/*******************************************************************************
* �� �� ��			: DMAInit
* ��������			: ��ʼ��DMA
* ע������			: ��
*******************************************************************************/
void DMAInit(void){
	NVIC_InitTypeDef NVICconstructor;
	
	// ���� DMA ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	// DMA ���ж�����
	NVICconstructor.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 1;
	NVICconstructor.NVIC_IRQChannelSubPriority = 1;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
	NVICconstructor.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 1;
	NVICconstructor.NVIC_IRQChannelSubPriority = 1;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
	// ���� DMA �����ã�ͨ��7��
	DMA_Cmd(DMA1_Channel7, DISABLE);									// ����ǰ�ر� DMA
	DMA_DeInit(DMA1_Channel7);											// ���ָ�Ĭ��ֵ
	
	DMAconstructor.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;		// �����ַ��U2 ���ݼĴ���
	DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufTX2.buf; 			// �ڴ��ַ�����ͻ����ַ
	DMAconstructor.DMA_DIR = DMA_DIR_PeripheralDST; 					// �����ڴ�-->����
	DMAconstructor.DMA_BufferSize = TXBUFLEN; 							// ���ͳ��ȣ����ͻ����ܳ�
	DMAconstructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		// �����ַ������
	DMAconstructor.DMA_MemoryInc = DMA_MemoryInc_Enable; 				// �ڴ��ַ����
	DMAconstructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// �����ֽڷ���
	DMAconstructor.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		// �ڴ��ֽڷ���
	DMAconstructor.DMA_Mode = DMA_Mode_Normal; 							// ���δ���ģʽ
	DMAconstructor.DMA_Priority = DMA_Priority_VeryHigh;				// ���ȼ����
	DMAconstructor.DMA_M2M = DMA_M2M_Disable; 							// ��ֹ�ڴ浽�ڴ�ģʽ
	
	DMA_Init(DMA1_Channel7, &DMAconstructor); 							// ��ʼ������
	DMA_ClearFlag(DMA1_FLAG_GL7); 										// ��� DMA ���б�־
	DMA_Cmd(DMA1_Channel7, DISABLE); 									// �رշ��� DMA����ÿ�η��Ϳ���һ��
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE); 					// ���� DMA ͨ���ж�
	
	// ���� DMA �����ã�ͨ��6��
	DMA_Cmd(DMA1_Channel6, DISABLE);									// ����ǰ�ر� DMA
	DMA_DeInit(DMA1_Channel6); 											// ���ָ�Ĭ��ֵ
	
	DMAconstructor.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;		// �����ַ��U2 ���ݼĴ���
	DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufRX2.buf; 			// �ڴ��ַ�����ջ����ַ
	DMAconstructor.DMA_DIR = DMA_DIR_PeripheralSRC; 					// ��������-->�ڴ�
	DMAconstructor.DMA_BufferSize = RXBUFLEN; 							// ���ճ��ȣ����ջ����ܳ�
	DMAconstructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		// �����ַ������
	DMAconstructor.DMA_MemoryInc = DMA_MemoryInc_Enable; 				// �ڴ��ַ����
	DMAconstructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// �����ֽڽ���
	DMAconstructor.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		// �ڴ��ֽڽ���
	DMAconstructor.DMA_Mode = DMA_Mode_Normal; 							// ���ν���ģʽ
	DMAconstructor.DMA_Priority = DMA_Priority_VeryHigh; 				// ���ȼ����
	DMAconstructor.DMA_M2M = DMA_M2M_Disable; 							// ��ֹ�ڴ浽�ڴ�ģʽ
	
	DMA_Init(DMA1_Channel6, &DMAconstructor); 							// ��ʼ������
	DMA_ClearFlag(DMA1_FLAG_GL6); 										// ��� DMA ���б�־
	DMA_Cmd(DMA1_Channel6, ENABLE); 									// �������� DMA
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); 					// ���� DMA ͨ���ж�
}
#endif

/*******************************************************************************
* �� �� ��			: USART2Init
* ��������			: ��ʼ��USART2
* ע������			: PA2���ͣ�PA3����
*******************************************************************************/
void USART2Init(unsigned int br){
	// ��GPIO��USART2��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	// ��ʼ��PA2-3(��˫����GPIO����)
	GPIO_InitTypeDef GPIOconstructor;
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_2;
	// �����������ģʽ
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_3;
	// ��������
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	// ��USART��������(����ֻ��Ҫ���Ĳ����ʣ����Դ�ST�ٷ����İ������ҵ�������׼�������Ҫ�޸�)
	USART_InitTypeDef USARTconstructor;
	
	USARTconstructor.USART_BaudRate = br;
	USARTconstructor.USART_WordLength = USART_WordLength_8b;
	USARTconstructor.USART_StopBits = USART_StopBits_1;
	USARTconstructor.USART_Parity = USART_Parity_No;
	USARTconstructor.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USARTconstructor.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	// ��ʼ��USART(��Ҫ������C����û������)
	USART_Init(USART2, &USARTconstructor);
	
	// �жϽ��������ź�
	NVIC_InitTypeDef NVICconstructor;
	
	NVICconstructor.NVIC_IRQChannel = USART2_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 3;
	NVICconstructor.NVIC_IRQChannelSubPriority = 3;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
#if DMA_STATUS == 0
	// �����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#else
	// �����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
#endif
	// ʹ��USART(������stm32f10x_usart.h�����ҵ���Ӧ�ĺ��������ڶ���������һ��ö������)
	USART_Cmd(USART2,ENABLE);
	
	// ��һ���ַ�������Ľ������
	// USART_GetFlagStatus(USART2,USART_FLAG_TC);
	
#if DMA_STATUS == 1
	// ��ʼ�� DMA
	DMAInit();
	// �� DMA �Ľ��պ����
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
#endif
}

#if Own_Handler
/*******************************************************************************
* �� �� ��			: USART2_IRQHandler
* ��������			: USART2 �жϴ�����
* ע������			: ��
*******************************************************************************/
void USART2_IRQHandler(void)
{
// �¼���ش���
#if MAX_EVENT_NUM>0
	
#endif

#if USART2_MUTEX
	// ֱ�������������
	if(detectUSART2()){
		return;
	}
	
	// USART2 �ٽ�
	getUSART2();
#endif
	
#if DMA_STATUS == 0
	// ����Ƿ�������
	if(USART_GetFlagStatus(USART2,USART_IT_RXNE) != RESET)
	{
		// ���������С�����
		if(bufRX2.index > RXBUFLEN)
		{
			
#if PRINT
		printf("%s\r\n",bufRX2.buf);
#endif
			
			bufRX2.index = 0;
			unsigned i=0;
			for(;i<RXBUFLEN;++i)
			{
				bufRX2.buf[i] = 0;
			}
		}
		// �����־λ
		bufRX2.buf[bufRX2.index++] = USART_ReceiveData(USART2);
	}

#else
	// ���������ж�
	if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)
	{
		// ��ʱ�ر� DMA
		DMA_Cmd(DMA1_Channel6,DISABLE);
		// ��ȡ���ĳ���
		bufRX2.index = RXBUFLEN - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		// DMA ����
		DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufRX2.buf;

#if PRINT
		printf("%s\r\n",bufRX2.buf);
#endif
		
		// DMAconstructor.DMA_BufferSize = RXBUFLEN;
		DMA_Init(DMA1_Channel6, &DMAconstructor);
		DMA1_Channel6->CNDTR = RXBUFLEN;
		DMA_Cmd(DMA1_Channel6, ENABLE); 						// �ٿ������� DMA
		USART_ReceiveData(USART2); 								// ��ȡ DR����� IDLE ��־
	}
#endif
	// ������δ��Ч
	if(USART_GetFlagStatus(USART2, USART_IT_ORE) != RESET) 
	{
		USART_ClearFlag(USART2, USART_FLAG_ORE);
		USART_ReceiveData(USART2); 								// ��ȡ DR����� ORE ��־
	}
	
#if USART2_MUTEX
	// �ͷ��ٽ���Դ
	giveUSART2();
#endif
}

#if DMA_STATUS>0
/*******************************************************************************
* �� �� ��			: DMA1_Channel7_IRQHandler
* ��������			: USART2 ���ʹ����жϺ���
* ע������			: ��
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void){
	// ȷ�� DMA ��û�з������
	if(DMA_GetITStatus(DMA1_FLAG_GL7)){
		// �����־λ
		DMA_ClearFlag(DMA1_FLAG_GL7);
		// �ٴο��� DMA
		DMA_Cmd(DMA1_Channel7,DISABLE);
		// ��ʶΪ����״̬��1��
		TX_Status = 1;
	}
}

/*******************************************************************************
* �� �� ��			: DMA1_Channel7_IRQHandler
* ��������			: USART2 �����������жϺ���
* ע������			: �ַ���ǧ��Ҫ���� 256����Ȼ�ᱻֱ������
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void){
	if(DMA_GetITStatus(DMA1_FLAG_TC6)){
		getUSART2();
		
		DMA_ClearFlag(DMA1_FLAG_TC6);
		DMA_Cmd(DMA1_Channel7,DISABLE);
		
		// �ý��ջ����㹻���������
		DMA1_Channel6->CNDTR = RXBUFLEN;
		
		// ��������� 256��ֱ���������ݣ�������
#if PRINT
		printf("%s\r\n",bufRX2.buf);
#endif
		
		unsigned i=0;
		for(;i<RXBUFLEN;++i)
		{
			bufRX2.buf[i] = 0;
		}
		
		giveUSART2();
		DMA_Cmd(DMA1_Channel6, ENABLE);
	}
}
#endif

#endif

/*******************************************************************************
* �� �� ��			: USART2_send
* ��������			: USART2 ���ͺ���
* ע������			: ��
* p.s.
* �����ֱ��Ҹģ���������
*******************************************************************************/
void USART2_send(uint8_t *arr, unsigned len){
// ������� DMA
#if DMA_STATUS>0
	// ����ǿ���״̬
	while(TX_Status == 0);
	// ����Ϊæµ״̬
	TX_Status = 0;
	// ���� len �ֽ�����
	memcpy(bufTX2.buf, arr, len);
	// ���ü�����
	DMA1_Channel7->CNDTR = len;
	// ���� DMA ͨ��
	DMA_Cmd(DMA1_Channel7, ENABLE);
// ���δ���� DMA
#else
	// �����Ĵ���ͨѶ
	unsigned i=0;
	while(i < len) {
	USART_SendData(USART2, arr[i++]);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
#endif
}

/*******************************************************************************
* �� �� ��			: USART2_Printf
* ��������			: USART2 �� printf ����
* ע������			: ��
* p.s.
* û�±��һ��� uint8_t unsigned �� int
*******************************************************************************/
void USART2_Printf(char *fmt, ...) {
#if DMA_STATUS>0
	// �ȴ� EMA ״̬Ϊ���У��������0��һֱѭ����
	while(TX_Status == 0);
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf((char *)bufTX2.buf, TXBUFLEN, fmt, arg_ptr);
	va_end(arg_ptr);
	// ��������
	USART2_send((uint8_t *)bufTX2.buf,strlen((char *)bufTX2.buf));
#else
	// �������
	unsigned i = 0;
	while(TX_Status == 0);
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf((char *)bufTX2.buf, TXBUFLEN, fmt, arg_ptr);
	va_end(arg_ptr);
	while((i<TXBUFLEN) && (bufTX2.buf[i])) {
		USART_SendData(USART2, bufTX2.buf[i++]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
#endif
}

/*
// u2_printf ���������Աȣ��������û�� DMA �Ǹ���
void u2_printf(char *fmt,...)
{
	// �ȴ��������
	while(TX_Status == 0);
	// bufTX2 ��ȫ�ֽṹ�壬����һ�� char ���� buf ��һ�� int �� index
	uint16_t i=0;
	va_list arg_ptr;
	va_start(arg_ptr,fmt);
	vsnprintf((char*)bufTX2.buf,TXBUFLEN,fmt,arg_ptr);
	while(i<TXBUFLEN && bufTX2.buf[i])
	{
		while((USART2->SR & 0x40) == 0);
		USART_SendData(USART2,bufTX2.buf[i]);
		i++;
	}
	va_end(arg_ptr);
}
*/

/*******************************************************************************
* �� �� ��			: ClearRXBuf
* ��������			: ���� USART2 �Ķ�������
* ע������			: ��
*******************************************************************************/
void ClearRXBuf(void)
{
#if USART2_MUTEX
	getUSART2();
#endif
	
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel6, DISABLE);
#endif
	
// ��� RX-buffer ����
#if CLEAR_RE
	USART2_Printf("%s",bufRX2.buf);
#endif	
	bufRX2.index = 0;
	unsigned i=0;
	for(;i<RXBUFLEN;++i)
	{
		bufRX2.buf[i] = 0;
	}
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel6, ENABLE);
#endif
	
#if USART2_MUTEX
	giveUSART2();
#endif
}

/*******************************************************************************
* �� �� ��			: ClearRXBuf
* ��������			: ���� USART2 ��д������
* ע������			: ��
*******************************************************************************/
void ClearTXBuf(void)
{
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel7, DISABLE);
#endif
	
// ��� TX-buffer ����
#if CLEAR_RE
	USART2_Printf("%s",bufTX2.buf);
#endif	
	bufRX2.index = 0;
	unsigned i=0;
	for(;i<TXBUFLEN;++i)
	{
		bufRX2.buf[i] = 0;
	}
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel7, DISABLE);
#endif
}

/*******************************************************************************
* �� �� ��			: isReadable
* ��������			: ��ѯ�Ƿ���Զ�ȡ����
* ע������			: ��
*******************************************************************************/
unsigned isUsart2Readable(void){
	if(bufRX2.buf[0] != 0 
#if USART2_MUTEX
	|| detectUSART2() 
#endif
	)
		return 1;
	return 0;
}

/*******************************************************************************
* �� �� ��			: readUsart2Buffer
* ��������			: ��ȡ��������
* ע������			: ȫ��char�Լ������
*******************************************************************************/
char *readUsart2Buffer(void){
	if(bufRX2.buf[0] != 0)
		return NULL;
	return bufRX2.buf;
}

/*******************************************************************************
* �� �� ��			: writeUsart2Buffer
* ��������			: ��ȡ��������
* ע������			: ȫ��char�Լ������
*******************************************************************************/
char *writeUsart2Buffer(char* buffer){
	USART2_Printf("%s",buffer);
}

#if USART2_MUTEX
/*******************************************************************************
* �� �� ��			: getUSART2
* ��������			: ��ȡ USART2 ʹ��Ȩ
* ע������			: ��
*******************************************************************************/
char getUSART2(void){
	// �ȴ�����
	while(usart2_status);
	
	// ��ȡʹ��Ȩ
	usart2_status = 1;
}

/*******************************************************************************
* �� �� ��			: giveUSART2
* ��������			: �ͷ� USART2 ʹ��Ȩ
* ע������			: ��
*******************************************************************************/
char giveUSART2(void){
	if(usart2_status == 0)
		printf("ERR:	usart2_status already zero!\r\n");
	// �ͷ�ʹ��Ȩ
	usart2_status = 0;
}

/*******************************************************************************
* �� �� ��			: detectUSART2
* ��������			: ���Զ�ȡ USART2 ʹ�����
* ע������			: ��
*******************************************************************************/
uint8_t detectUSART2(void){
	return usart2_status;
}
#endif

#endif
