#include "CS100A.h"

#define printf USART2_Printf

// 时间片
static uint32_t time_period = 0;
// 中断锁
static uint8_t mutex = 0;
// 查询事件
static uint8_t event = 0;

/*******************************************************************************
* 函 数 名			: CS100A_Init
* 函数功能			: 初始化 HC-SR04 测距传感器
* 注意事项			: 管脚连接: Trig - GPIOA5, Echo - GPIOA4
*******************************************************************************/
void CSA_Init(void){
	//使用结构体初始化GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_5;      		//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 		//推挽输出
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//频率
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	PAout(5) = 0;
	
	// 开启中断
	NVIC_InitTypeDef NVICconstructor;
	EXTI_InitTypeDef EXTIconstructor;
	
	//GPIO初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_4;
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	//EXTI复用初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	
	EXTIconstructor.EXTI_Line = EXTI_Line4;
	EXTIconstructor.EXTI_Mode = EXTI_Mode_Interrupt;
	// 上升下降沿统统触发, 
	EXTIconstructor.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIconstructor.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTIconstructor);
	
	//NVIC初始化
	
	//四位抢占优先级，没有子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVICconstructor.NVIC_IRQChannel = EXTI4_IRQn;	
	
	NVICconstructor.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVICconstructor.NVIC_IRQChannelSubPriority = 0x00;
	NVICconstructor.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVICconstructor);
	
	mutex = 0;
}

/*******************************************************************************
* 函 数 名			: CS100A_SendTrig
* 函数功能			: 发送脉冲信号
* 注意事项			: 无
*******************************************************************************/
void CSA_SendTrig(void){
	// 触发信号
	PAout(5) = 1;
	Delay_us(10);
	PAout(5) = 0;
}

/*******************************************************************************
* 函 数 名			: CS100A_GetData
* 函数功能			: 获取距离信息
* 注意事项			: 无
*******************************************************************************/
unsigned CSA_GetData(void){
	if(time_period != 0){
		unsigned us = time_period / fac_us;
		
		// USART2_Printf("Us: %u.\r\n", us);
		// 单位是微米
		unsigned um = 340 * us;
		
		time_period = 0;
		mutex = 0;
		
		return um;
	}
	else
		// 错误码
		return 0xffffffff;
}

/*******************************************************************************
* 函 数 名			: CS100A_GetOreData
* 函数功能			: 获取间隔时间片信息
* 注意事项			: 无
*******************************************************************************/
uint32_t CSA_GetOreData(void){
	if(mutex == 1){
		uint32_t data = time_period;
		
		// USART2_Printf("GetData: %x.\r\n", data);

		mutex = 0;
		
		return data;
	}
	else
		// 错误码
		return 0xffffffff;
}

/*******************************************************************************
* 函 数 名			: EXTI4_IRQHandler
* 函数功能			: 外部中断 4 函数
* 注意事项			: 不能带特别麻烦的函数、名字不能改
*******************************************************************************/
void EXTI4_IRQHandler(void) {
	// Systick 计时函数
	uint32_t ret = Chrono();
	
	if(ret != 0){
		if(mutex == 1){
			// USART2_Printf("EXTI4: Old data exists: %x.\r\n", time_period);
			// 存在旧数据直接清除标志位然后退出
			EXTI_ClearITPendingBit(EXTI_Line4);
			return;
		}
		
		time_period = ret;
		// USART2_Printf("EXTI4: New data exists: %x.\r\n", time_period);
		
		mutex = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}