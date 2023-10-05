#include "MQx.h"

/* 看门狗的优先级比 MQx 高，不用担心输出时狗给 MCU 发送复位信号 */
// 是否以中断形式打开
#define NVIC_MODE	1

#define printf USART2_Printf

uint8_t dangerManager = 0;

/*******************************************************************************
* 函 数 名			: MQx_Init
* 函数功能			: 初始化 MQx 空气质量传感器
* 注意事项			: 返回值是当前传感器 D0 默认状态(使用 GPIOA7)
*******************************************************************************/
uint8_t MQx_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_7;      		//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IPD; 		//下拉输入	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//频率
	
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_4;      		//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 		//下拉输入	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;		//频率
	
	GPIO_Init(GPIOB, &GPIOconstructor);
	
	PBout(4) = 0;

#if !NVIC_MODE
	return (uint8_t)PAin(7);
#else
	// 开启中断
	NVIC_InitTypeDef NVICconstructor;
	EXTI_InitTypeDef EXTIconstructor;
	
	//EXTI复用初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
	
	EXTIconstructor.EXTI_Line = EXTI_Line7;
	EXTIconstructor.EXTI_Mode = EXTI_Mode_Interrupt;
	// 上升下降沿统统触发, 
	EXTIconstructor.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIconstructor.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTIconstructor);
	
	//NVIC初始化
	
	//四位抢占优先级，没有子优先级
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
* 函 数 名			: MQx_GetStatus
* 函数功能			: 获取 D0 状态并检验
* 注意事项			: 需要传入初始化后的默认状态进行区分
*******************************************************************************/
uint8_t MQx_GetStatus(uint8_t price){
	// 未知错误码
	uint8_t flag = 2;
	uint8_t volatile status = PAin(7);
	
	if(price == (uint8_t)0x00){
		if(status == 1){
			// 报警错误码
			flag = 1;
			printf("Flag 0 branch\r\n");
		}
		else{
			// 正常错误码
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
* 函 数 名			: EXTI4_IRQHandler
* 函数功能			: 外部中断 4 函数
* 注意事项			: 不能带特别麻烦的函数、名字不能改
*******************************************************************************/
void EXTI9_5_IRQHandler(void) {
	// 消除抖动
	Delay_ms(2);
	dangerManager = 1;
	PBout(4) = 1;
	
	// USART2_Printf("Trig occured!\r\n");
	EXTI_ClearITPendingBit(EXTI_Line7);
}
#endif
