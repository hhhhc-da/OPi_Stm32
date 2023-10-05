
#include "wwdg.h"

#define WWDG_TIMER_CALLBACK 1

#if WWDG_TIMER_CALLBACK

cb Delay_callBackFunction = IDLE;

#endif

// wwdg 定时器, 用于毫秒定时(一分频, 5.0062ms 定时器)
static const uint8_t pre_cnt = 0x2B + 0x40;
static uint32_t delayCount = 0;

// 看门狗总节拍数
static uint64_t wwdg_all_ticks = 0;

// 计时周期
static unsigned round = 0;

/*******************************************************************************
* 函 数 名			: wwdgInit
* 函数功能			: 初始化 wwdg
* 注意事项			: 无
*******************************************************************************/
void wwdgInit(void){
	// WWDG 时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); 
	
	// PC13提示灯用于显示看门狗中断工作情况
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_13;
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIOconstructor);

	
	// PCLK1 一分频, 即 36MHz
	WWDG_SetPrescaler(WWDG_Prescaler_1);
	// 设置窗口值, 也就是配置寄存器 WWDG_CFR 的后七位计时数值(用来和 WWDG_CR 作比较)
	WWDG_SetWindowValue(pre_cnt);
	// 使能提前唤醒中断
	WWDG_EnableIT();
	// 设置控制寄存器 WWDG_CR 的后七位计时数值
	WWDG_SetCounter(pre_cnt);
	// 清除提前唤醒中断标志位 
	WWDG_ClearFlag();
	// 使能看门狗中断
	WWDG_NVIC_Init();
	// 使能看门狗
	WWDG_Enable(pre_cnt);
	
	// Print("WWDG_Initialization OK...\r\n");
}

/*******************************************************************************
* 函 数 名			: WWDG_NVIC_Init
* 函数功能			: 开启窗口看门狗中断
* 注意事项			: 无
*******************************************************************************/
void WWDG_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 2:2 分配中断优先级位
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//WWDG中断(比较高)
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函 数 名			: WWDG_IRQHandler
* 函数功能			: 窗口看门狗中断服务程序
* 注意事项			: 喂狗要快不然就寄
*******************************************************************************/
void WWDG_IRQHandler(void)
{
	// 喂狗, 防止复位
	WWDG_SetCounter(pre_cnt);

	// 清除提前唤醒中断标志位
	WWDG_ClearFlag();	  
	
	// 看门狗总节拍数
	++wwdg_all_ticks;
	
	// 定时装置
	if(round != 0)
		--round;
}

/*******************************************************************************
* 函 数 名			: wDelay_ms
* 函数功能			: 看门狗毫秒延时
* 注意事项			: 只能是5的倍数(如果不是则向下取整！！)
*******************************************************************************/
void wDelay_ms(unsigned ms){
	// 初值
	unsigned cr = WWDG->CR;
	round = ms / 5;
	
	// 如果还有周期则判断 expr1 后退出, 如果还有不到一个周期，则判断 expr2 后退出
	while(round != 0 || WWDG->CR > cr);
	
#if WWDG_TIMER_CALLBACK
	Delay_callBackFunction();
#endif
}

/*******************************************************************************
* 函 数 名			: getTime
* 函数功能			: 返回系统运行时间
* 注意事项			: 分度值: 0.005s
*******************************************************************************/
uint64_t getTime(void){
	// 直接打印时间
	Print("Time: %fs\r\n",5.0*wwdg_all_ticks/1000);
	
	// 返回毫秒数
	return 5 * wwdg_all_ticks;
}


uint8_t wtime_check = 0;
uint64_t wtime_piece[2] = { 0, 0 };


/*******************************************************************************
 * 函 数 名			: wChrono
 * 函数功能			: 计时函数，返回间隔时间(秒级)
 * 注意事项			: 无
 *******************************************************************************/
double wChrono(void) {
	// 如果是开始计时，则重置周期并记录 VAL 寄存器现有值
	if(wtime_check == 0){
		wtime_piece[0] = wwdg_all_ticks;
		// 重置状态
		wtime_check = 1;
		
		return 0;
	}
	// 如果是结束计时，则计算周期
	else{
		wtime_piece[1] = wwdg_all_ticks;
		// 相隔时间片
		uint64_t time = wtime_piece[1] - wtime_piece[0];
		// 重置状态
		wtime_check = 0;
		
		return (time * 5.0062 / 1000);
	}
}

#if WWDG_TIMER_CALLBACK
/*******************************************************************************
* 函 数 名			: IDLE
* 函数功能			: 给 wDelay 回调钩子的计数玩函数
* 注意事项			: 你最好是给我换了
*******************************************************************************/
void IDLE(void){
	__asm {
		ADD delayCount, delayCount, #1
	}
}
#endif
