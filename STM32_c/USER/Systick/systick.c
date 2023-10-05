
#include "systick.h"

/**************************  systick.h  *************************************************************
@def	ModeInterrupt			控制开启中断
@arg	tick_now				系统当前运行时间
***************************  systick.h  _end  *******************************************************/

// 模式选择，1为打开系统时钟中断，0为关闭
#define ModeInterrupt 1

#if ModeInterrupt
#define InterruptOn
#else
#define InterruptOff
#endif

#ifdef InterruptOn

static uint32_t tick_now = 0;

// 记录 flag
uint8_t time_check = 0;
volatile uint32_t time_piece[4] = { 0, 0, 0, 0 };

/*******************************************************************************
 * 函 数 名			: SysTick_Handler
 * 函数功能			: 周期定时（系统运行时间）
 * 注意事项			: 不能用于EXTI+NVIC、名字不能改
 *******************************************************************************/
void SysTick_Handler(void)
{
	// 系统运行时间
	++tick_now;

#if STEP_LEN && (!EventMode)
	if (tick_now % STEP_LEN == 0)
	{
		TimerTask();
	}
#endif
	
}

#endif

// 单次延时不可超过1.864s（八分频情况下）

uint32_t fac_us = 0;
uint32_t fac_ms = 0;

/*******************************************************************************
 * 函 数 名			: SysTickInit
 * 函数功能			: 初始化SysTick
 * 注意事项			: 无
 *******************************************************************************/
void SystickInit(void)
{
	// 八分频，让延时时间更长一点，但是最小时间分度也变大了
	fac_us = SystemCoreClock / 8000000;
	fac_ms = fac_us * 1000;

	// 设置bit2，即设置时钟源（可以点开文件查看宏定义）
	// 八分频，可以让延时时间更长一些
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

#ifdef InterruptOn
	time_check = 0;
	
	// 设置中断间隔为1ms
	static uint32_t InterruptTime = 1;
	
	// 设置优先级
	NVIC_SetPriority(SysTick_IRQn, 1);
	// SCB->SHP[11] = 0xf0;

	// 设置中断间隔
	SysTick->LOAD = InterruptTime * fac_ms;
	
	// 打开定时器和中断
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#endif

#ifdef InterruptOff
	// 设置bit1为0，即禁止中断（bit1负责中断使能）
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
	// 设置bit0为0，即关闭定时器（bit0负责开启定时器）
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);

#endif
}

#ifdef InterruptOn
/*******************************************************************************
 * 函 数 名			: Chrono
 * 函数功能			: 计时函数，返回间隔时间(毫秒级)
 * 注意事项			: 无
 *******************************************************************************/
uint32_t Chrono(void) {
	// 如果是开始计时，则重置周期并记录 VAL 寄存器现有值
	if(time_check == 0){
		time_piece[0] = tick_now;
		time_piece[1] = SysTick->VAL;
		// 重置状态
		time_check = 1;
		
		return 0;
	}
	// 如果是结束计时，则计算周期
	else{
		time_piece[2] = tick_now;
		time_piece[3] = SysTick->VAL;
		
		// 相隔时间片
		uint32_t time = (time_piece[2] - time_piece[0])*fac_ms + time_piece[1] - time_piece[3];
		// 重置状态
		time_check = 0;
		
		time_piece[3] = time_piece[2] = time_piece[1] = time_piece[0] = 0;
		
		// USART2_Printf("Systick: %x\r\n", time);
		
		return time;
	}
}

/*******************************************************************************
 * 函 数 名			: Delay_ms、iDelay_ms
 * 函数功能			: 系统毫秒延时
 * 注意事项			: 不能用于EXTI+NVIC
 *******************************************************************************/
void iDelay_ms(uint32_t nms)
{
	// 停止时间
	uint32_t tick_to = nms + tick_now;
	// VAL寄存器中的数据
	uint32_t val = SysTick->VAL;
	while (1)
	{
		// 已经到达定时时间且小于原始数据(LOAD寄存器重新装载)
		if (tick_now == tick_to && val <= SysTick->VAL)
			break;
		// 若超过则直接跳出
		else if (tick_now >= tick_to)
			break;
	}
}

void Delay_ms(uint32_t nms)
{
	uint32_t ticks_until;
	uint32_t value;
	ticks_until = tick_now + nms;
	value = SysTick->VAL;
	while (1)
	{
		if ((tick_now == ticks_until) && (SysTick->VAL <= value))
			break;
		else if (tick_now > ticks_until)
			break;
	}
}

/*******************************************************************************
 * 函 数 名			: Delay_us、iDelay_us
 * 函数功能			: 系统微秒延时
 * 注意事项			: 不能用于EXTI+NVIC
 *******************************************************************************/
void iDelay_us(uint32_t nus)
{
	// 目前VAL寄存器中的时间(第一时间构造确保延时误差小)
	uint32_t tick_beg = SysTick->VAL;
	// 需要延时的时间(假定微秒延时不超过1ms)
	uint32_t tick_info = nus * fac_us;
	// LOAD寄存器中的时间间隔(默认为1ms)
	uint32_t tick_load = SysTick->LOAD;
	// 用于装载计算好的时间(数据结构里的循环队列吧应该/这个顺序避免出现负数同时也不会过大)
	uint32_t tick_to = (tick_beg + tick_load - tick_info) % tick_load;
	// 异常退出
	uint32_t tick_err = 1 + tick_now;
	// 构造数据和数据运算需要一定的时间(暂未处理/遇到了再说)
	while (SysTick->VAL != tick_to)
	{ // 异常中断(延时超过一ms/同毫秒延时原理)
		if (tick_now >= tick_err && SysTick->VAL <= tick_beg)
			break;
	}
}

void Delay_us(uint32_t nus)
{
	// 变量声明(tick_cnts 特殊处理，为0)
	uint32_t ticks, tick_last, tick_now;
	uint32_t tick_cnts = 0;
	uint32_t tick_load;
	// ticks 为需要延时的时间
	// tick_load 为LOAD寄存器的时间间隔(默认1ms)
	// tick_last 为VAL寄存器当前数据
	ticks = nus * fac_us;
	tick_load = SysTick->LOAD;
	tick_last = SysTick->VAL;

	while (1)
	{
		// tock_now 为VAL寄存器当前时间（动态变化的/循环更新）
		tick_now = SysTick->VAL;
		// 跳过本数据（防止多次计入）
		if (tick_now != tick_last)
		{
			// 如果VAL寄存器发生了变化且还没有重新装载，cnt新增 目前与记录时间的差值（走过的时间片）
			if (tick_now < tick_last)
				tick_cnts += tick_last - tick_now;
			// 如果VAL寄存器未变化或已经重新装载，cnt新增 目前与记录时间的差值 + 时间间隔（1ms）
			else
				tick_cnts += tick_load - tick_now + tick_last;
			// 动态更新 tick_last（循环更新）
			tick_last = tick_now;
			// 检测若cnt超过设定值，退出循环
			if (tick_cnts >= ticks)
				break;
		}
	}
}

#endif

#ifdef InterruptOff

/*******************************************************************************
 * 函 数 名			: Delay_ms
 * 函数功能			: 硬件毫秒延时
 * 注意事项			: 无
 *******************************************************************************/
void Delay_ms(uint32_t nms)
{
	// 设置需要延时的时间
	SysTick->LOAD = nms * fac_ms;
	// 产生赋值操作，将LOAD寄存器数据存入VAL寄存器
	SysTick->VAL = 0x00;
	// 开启定时器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// 等待VAL的第bit16置位（即等待到时间到）
	while (!(SysTick->CTRL & (1 << 16)))
		;
	// 关闭定时器
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

/*******************************************************************************
 * 函 数 名			: Delay_us
 * 函数功能			: 硬件微秒延时
 * 注意事项			: 无
 *******************************************************************************/
void Delay_us(uint32_t nus)
{
	// 设置需要延时的时间
	SysTick->LOAD = nus * fac_us;
	// 产生赋值操作，将LOAD寄存器数据存入VAL寄存器
	SysTick->VAL = 0x00;
	// 开启定时器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// 等待VAL的第bit16置位（即等待到时间到）
	while (!(SysTick->CTRL & (1 << 16)))
		;
	// 关闭定时器
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	// VAL寄存器由1变成0的时候，会产生一个中断请求信号
}

#endif
