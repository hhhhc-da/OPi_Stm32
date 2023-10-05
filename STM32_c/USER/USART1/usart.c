
#include "usart.h"
/**********************************************************************************/

#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{
	x = x; 
} 

void _ttywrch(int ch)
{
    ch = ch;
}

//printf重定向到USART1
int fputc(int ch, FILE *f) {
	//发送数据
	while((USART1->SR & 0x40) == 0);
	USART1->DR = (uint8_t)ch;
	return ch;
}

//支持printf函数，需选择use MicroLIB
int GetKey(void)
{
	while(!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1ff));
}

/**********************************************************************************/

// 缓冲区
Buffer bufs;

/*******************************************************************************
* 函 数 名			: USART1Init
* 函数功能			: 初始化USART1
* 注意事项			: PA9发送，PA10接收
* p.s.
* 别忘了参数波特率
*******************************************************************************/
void USART1Init(unsigned int br){
	//打开GPIO和USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
	
	//初始化PA9-10(又双叒叕GPIO复用)
	GPIO_InitTypeDef GPIOconstructor;
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_9;
	//复用推挽输出模式
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_10;
	//浮空输入
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	//对USART进行配置(基本只需要更改波特率，可以从ST官方给的案例中找到，出标准后很少需要修改)
	USART_InitTypeDef USARTconstructor;
	
	USARTconstructor.USART_BaudRate = br;
	USARTconstructor.USART_WordLength = USART_WordLength_8b;
	USARTconstructor.USART_StopBits = USART_StopBits_1;
	USARTconstructor.USART_Parity = USART_Parity_No;
	USARTconstructor.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USARTconstructor.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//初始化USART(不要重名，C语言没有重载)
	USART_Init(USART1, &USARTconstructor);
	
	//中断接收输入信号
	NVIC_InitTypeDef NVICconstructor;
	
	NVICconstructor.NVIC_IRQChannel = USART1_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 3;
	NVICconstructor.NVIC_IRQChannelSubPriority = 3;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能USART(可以在stm32f10x_usart.h库中找到对应的函数名，第二个参数是一个枚举类型)
	USART_Cmd(USART1,ENABLE);
	
	//第一个字符出问题的解决方案
	//USART_GetFlagStatus(USART1,USART_FLAG_TC);
}

/*******************************************************************************
* 函 数 名			: USART1_IRQHandler
* 函数功能			: USART1中断处理函数
* 注意事项			: 无
*******************************************************************************/
void USART1_IRQHandler(void)
{
	//检测是否接收完毕
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) != RESET)
	{
		//超过缓存大小则清空
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
* 函 数 名			: ClearBuf
* 函数功能			: 销毁USART1缓冲区内容
* 注意事项			: 无
*******************************************************************************/
void ClearBuf(void)
{
//清空函数
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
* 函 数 名			: isReadable
* 函数功能			: 查询是否可以读取数组
* 注意事项			: 无
*******************************************************************************/
unsigned isReadable(void){
	if(bufs.buf[0] != 0)
		return 1;
	return 0;
}

/*******************************************************************************
* 函 数 名			: readBuffer
* 函数功能			: 读取数组内容
* 注意事项			: 全是char自己处理吧
*******************************************************************************/
char *readBuffer(void){
	if(bufs.buf[0] != 0)
		return NULL;
	return bufs.buf;
}

/*******************************************************************************
* 函 数 名			: writeBuffer
* 函数功能			: 读取数组内容
* 注意事项			: 全是char自己处理吧
*******************************************************************************/
char *writeBuffer(char* buffer){
	printf("%s",buffer);
}