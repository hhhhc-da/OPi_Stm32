#include "Usart_Event.h"

// 清除时显示缓冲区内容
#define CLEAR_RE   					1

// 是否开启 DMA
#define DMA_STATUS					0

// 是否开启 USART2
#define Usart2						1

// 自定义 IRQ 是否开启
#define Own_Handler 				1

// 自定义溢出回显
#define PRINT						1

// 临界资源保护变量
unsigned usart2_status = 0;

#if Usart2
// DMA 工作状态符（1空闲，0忙碌）
unsigned TX_Status = 1;

Buffer2 bufTX2;
Buffer2 bufRX2;

#if DMA_STATUS>0

// DMA 初始化器
DMA_InitTypeDef DMAconstructor;

/*******************************************************************************
* 函 数 名			: DMAInit
* 函数功能			: 初始化DMA
* 注意事项			: 无
*******************************************************************************/
void DMAInit(void){
	NVIC_InitTypeDef NVICconstructor;
	
	// 开启 DMA 时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	// DMA 的中断配置
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
	
	// 发送 DMA 的配置（通道7）
	DMA_Cmd(DMA1_Channel7, DISABLE);									// 配置前关闭 DMA
	DMA_DeInit(DMA1_Channel7);											// 并恢复默认值
	
	DMAconstructor.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;		// 外设基址：U2 数据寄存器
	DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufTX2.buf; 			// 内存基址：发送缓存基址
	DMAconstructor.DMA_DIR = DMA_DIR_PeripheralDST; 					// 方向：内存-->外设
	DMAconstructor.DMA_BufferSize = TXBUFLEN; 							// 发送长度：发送缓存总长
	DMAconstructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		// 外设基址不自增
	DMAconstructor.DMA_MemoryInc = DMA_MemoryInc_Enable; 				// 内存基址自增
	DMAconstructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// 外设字节发送
	DMAconstructor.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		// 内存字节发送
	DMAconstructor.DMA_Mode = DMA_Mode_Normal; 							// 单次传送模式
	DMAconstructor.DMA_Priority = DMA_Priority_VeryHigh;				// 优先级最高
	DMAconstructor.DMA_M2M = DMA_M2M_Disable; 							// 禁止内存到内存模式
	
	DMA_Init(DMA1_Channel7, &DMAconstructor); 							// 初始化配置
	DMA_ClearFlag(DMA1_FLAG_GL7); 										// 清除 DMA 所有标志
	DMA_Cmd(DMA1_Channel7, DISABLE); 									// 关闭发送 DMA——每次发送开启一次
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE); 					// 开启 DMA 通道中断
	
	// 接收 DMA 的配置（通道6）
	DMA_Cmd(DMA1_Channel6, DISABLE);									// 配置前关闭 DMA
	DMA_DeInit(DMA1_Channel6); 											// 并恢复默认值
	
	DMAconstructor.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;		// 外设基址：U2 数据寄存器
	DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufRX2.buf; 			// 内存基址：接收缓存基址
	DMAconstructor.DMA_DIR = DMA_DIR_PeripheralSRC; 					// 方向：外设-->内存
	DMAconstructor.DMA_BufferSize = RXBUFLEN; 							// 接收长度：接收缓存总长
	DMAconstructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		// 外设基址不自增
	DMAconstructor.DMA_MemoryInc = DMA_MemoryInc_Enable; 				// 内存基址自增
	DMAconstructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// 外设字节接收
	DMAconstructor.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		// 内存字节接收
	DMAconstructor.DMA_Mode = DMA_Mode_Normal; 							// 单次接收模式
	DMAconstructor.DMA_Priority = DMA_Priority_VeryHigh; 				// 优先级最高
	DMAconstructor.DMA_M2M = DMA_M2M_Disable; 							// 禁止内存到内存模式
	
	DMA_Init(DMA1_Channel6, &DMAconstructor); 							// 初始化配置
	DMA_ClearFlag(DMA1_FLAG_GL6); 										// 清除 DMA 所有标志
	DMA_Cmd(DMA1_Channel6, ENABLE); 									// 开启接收 DMA
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); 					// 开启 DMA 通道中断
}
#endif

/*******************************************************************************
* 函 数 名			: USART2Init
* 函数功能			: 初始化USART2
* 注意事项			: PA2发送，PA3接收
*******************************************************************************/
void USART2Init(unsigned int br){
	// 打开GPIO和USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	// 初始化PA2-3(又双叒叕GPIO复用)
	GPIO_InitTypeDef GPIOconstructor;
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_2;
	// 复用推挽输出模式
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_3;
	// 浮空输入
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	// 对USART进行配置(基本只需要更改波特率，可以从ST官方给的案例中找到，出标准后很少需要修改)
	USART_InitTypeDef USARTconstructor;
	
	USARTconstructor.USART_BaudRate = br;
	USARTconstructor.USART_WordLength = USART_WordLength_8b;
	USARTconstructor.USART_StopBits = USART_StopBits_1;
	USARTconstructor.USART_Parity = USART_Parity_No;
	USARTconstructor.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USARTconstructor.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	// 初始化USART(不要重名，C语言没有重载)
	USART_Init(USART2, &USARTconstructor);
	
	// 中断接收输入信号
	NVIC_InitTypeDef NVICconstructor;
	
	NVICconstructor.NVIC_IRQChannel = USART2_IRQn;
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 3;
	NVICconstructor.NVIC_IRQChannelSubPriority = 3;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICconstructor);
	
#if DMA_STATUS == 0
	// 接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#else
	// 空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
#endif
	// 使能USART(可以在stm32f10x_usart.h库中找到对应的函数名，第二个参数是一个枚举类型)
	USART_Cmd(USART2,ENABLE);
	
	// 第一个字符出问题的解决方案
	// USART_GetFlagStatus(USART2,USART_FLAG_TC);
	
#if DMA_STATUS == 1
	// 初始化 DMA
	DMAInit();
	// 打开 DMA 的接收和输出
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
#endif
}

#if Own_Handler
/*******************************************************************************
* 函 数 名			: USART2_IRQHandler
* 函数功能			: USART2 中断处理函数
* 注意事项			: 无
*******************************************************************************/
void USART2_IRQHandler(void)
{
// 事件相关代码
#if MAX_EVENT_NUM>0
	
#endif

#if USART2_MUTEX
	// 直接舍弃这段数据
	if(detectUSART2()){
		return;
	}
	
	// USART2 临界
	getUSART2();
#endif
	
#if DMA_STATUS == 0
	// 检测是否接收完毕
	if(USART_GetFlagStatus(USART2,USART_IT_RXNE) != RESET)
	{
		// 超过缓存大小则清空
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
		// 清除标志位
		bufRX2.buf[bufRX2.index++] = USART_ReceiveData(USART2);
	}

#else
	// 产生空闲中断
	if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)
	{
		// 暂时关闭 DMA
		DMA_Cmd(DMA1_Channel6,DISABLE);
		// 获取到的长度
		bufRX2.index = RXBUFLEN - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		// DMA 设置
		DMAconstructor.DMA_MemoryBaseAddr = (uint32_t)bufRX2.buf;

#if PRINT
		printf("%s\r\n",bufRX2.buf);
#endif
		
		// DMAconstructor.DMA_BufferSize = RXBUFLEN;
		DMA_Init(DMA1_Channel6, &DMAconstructor);
		DMA1_Channel6->CNDTR = RXBUFLEN;
		DMA_Cmd(DMA1_Channel6, ENABLE); 						// 再开启接收 DMA
		USART_ReceiveData(USART2); 								// 读取 DR，清除 IDLE 标志
	}
#endif
	// 如果清除未生效
	if(USART_GetFlagStatus(USART2, USART_IT_ORE) != RESET) 
	{
		USART_ClearFlag(USART2, USART_FLAG_ORE);
		USART_ReceiveData(USART2); 								// 读取 DR，清除 ORE 标志
	}
	
#if USART2_MUTEX
	// 释放临界资源
	giveUSART2();
#endif
}

#if DMA_STATUS>0
/*******************************************************************************
* 函 数 名			: DMA1_Channel7_IRQHandler
* 函数功能			: USART2 发送处理中断函数
* 注意事项			: 无
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void){
	// 确认 DMA 有没有发送完毕
	if(DMA_GetITStatus(DMA1_FLAG_GL7)){
		// 清除标志位
		DMA_ClearFlag(DMA1_FLAG_GL7);
		// 再次开启 DMA
		DMA_Cmd(DMA1_Channel7,DISABLE);
		// 标识为空闲状态（1）
		TX_Status = 1;
	}
}

/*******************************************************************************
* 函 数 名			: DMA1_Channel7_IRQHandler
* 函数功能			: USART2 接收满处理中断函数
* 注意事项			: 字符数千万不要超过 256，不然会被直接销毁
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void){
	if(DMA_GetITStatus(DMA1_FLAG_TC6)){
		getUSART2();
		
		DMA_ClearFlag(DMA1_FLAG_TC6);
		DMA_Cmd(DMA1_Channel7,DISABLE);
		
		// 让接收缓冲足够大接收数据
		DMA1_Channel6->CNDTR = RXBUFLEN;
		
		// 如果超过了 256，直接销毁数据！！！！
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
* 函 数 名			: USART2_send
* 函数功能			: USART2 发送函数
* 注意事项			: 无
* p.s.
* 这名字别乱改，容易重名
*******************************************************************************/
void USART2_send(uint8_t *arr, unsigned len){
// 如果开启 DMA
#if DMA_STATUS>0
	// 如果是空闲状态
	while(TX_Status == 0);
	// 设置为忙碌状态
	TX_Status = 0;
	// 拷贝 len 字节数据
	memcpy(bufTX2.buf, arr, len);
	// 设置计数器
	DMA1_Channel7->CNDTR = len;
	// 开启 DMA 通道
	DMA_Cmd(DMA1_Channel7, ENABLE);
// 如果未开启 DMA
#else
	// 正常的串口通讯
	unsigned i=0;
	while(i < len) {
	USART_SendData(USART2, arr[i++]);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
#endif
}

/*******************************************************************************
* 函 数 名			: USART2_Printf
* 函数功能			: USART2 的 printf 函数
* 注意事项			: 无
* p.s.
* 没事别乱混用 uint8_t unsigned 和 int
*******************************************************************************/
void USART2_Printf(char *fmt, ...) {
#if DMA_STATUS>0
	// 等待 EMA 状态为空闲（如果等于0就一直循环）
	while(TX_Status == 0);
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf((char *)bufTX2.buf, TXBUFLEN, fmt, arg_ptr);
	va_end(arg_ptr);
	// 发送数据
	USART2_send((uint8_t *)bufTX2.buf,strlen((char *)bufTX2.buf));
#else
	// 正常输出
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
// u2_printf 函数（作对比，这个等于没有 DMA 那个）
void u2_printf(char *fmt,...)
{
	// 等待发送完毕
	while(TX_Status == 0);
	// bufTX2 是全局结构体，包含一个 char 数组 buf 和一个 int 的 index
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
* 函 数 名			: ClearRXBuf
* 函数功能			: 销毁 USART2 的读缓冲区
* 注意事项			: 无
*******************************************************************************/
void ClearRXBuf(void)
{
#if USART2_MUTEX
	getUSART2();
#endif
	
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel6, DISABLE);
#endif
	
// 清空 RX-buffer 函数
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
* 函 数 名			: ClearRXBuf
* 函数功能			: 销毁 USART2 的写缓冲区
* 注意事项			: 无
*******************************************************************************/
void ClearTXBuf(void)
{
#if DMA_STATUS > 0
	DMA_Cmd(DMA1_Channel7, DISABLE);
#endif
	
// 清空 TX-buffer 函数
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
* 函 数 名			: isReadable
* 函数功能			: 查询是否可以读取数组
* 注意事项			: 无
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
* 函 数 名			: readUsart2Buffer
* 函数功能			: 读取数组内容
* 注意事项			: 全是char自己处理吧
*******************************************************************************/
char *readUsart2Buffer(void){
	if(bufRX2.buf[0] != 0)
		return NULL;
	return bufRX2.buf;
}

/*******************************************************************************
* 函 数 名			: writeUsart2Buffer
* 函数功能			: 读取数组内容
* 注意事项			: 全是char自己处理吧
*******************************************************************************/
char *writeUsart2Buffer(char* buffer){
	USART2_Printf("%s",buffer);
}

#if USART2_MUTEX
/*******************************************************************************
* 函 数 名			: getUSART2
* 函数功能			: 获取 USART2 使用权
* 注意事项			: 无
*******************************************************************************/
char getUSART2(void){
	// 等待清零
	while(usart2_status);
	
	// 获取使用权
	usart2_status = 1;
}

/*******************************************************************************
* 函 数 名			: giveUSART2
* 函数功能			: 释放 USART2 使用权
* 注意事项			: 无
*******************************************************************************/
char giveUSART2(void){
	if(usart2_status == 0)
		printf("ERR:	usart2_status already zero!\r\n");
	// 释放使用权
	usart2_status = 0;
}

/*******************************************************************************
* 函 数 名			: detectUSART2
* 函数功能			: 尝试读取 USART2 使用情况
* 注意事项			: 无
*******************************************************************************/
uint8_t detectUSART2(void){
	return usart2_status;
}
#endif

#endif
