
#include "systick.h"

/**************************  systick.h  *************************************************************
@def	ModeInterrupt			���ƿ����ж�
@arg	tick_now				ϵͳ��ǰ����ʱ��
***************************  systick.h  _end  *******************************************************/

// ģʽѡ��1Ϊ��ϵͳʱ���жϣ�0Ϊ�ر�
#define ModeInterrupt 1

#if ModeInterrupt
#define InterruptOn
#else
#define InterruptOff
#endif

#ifdef InterruptOn

static uint32_t tick_now = 0;

// ��¼ flag
uint8_t time_check = 0;
volatile uint32_t time_piece[4] = { 0, 0, 0, 0 };

/*******************************************************************************
 * �� �� ��			: SysTick_Handler
 * ��������			: ���ڶ�ʱ��ϵͳ����ʱ�䣩
 * ע������			: ��������EXTI+NVIC�����ֲ��ܸ�
 *******************************************************************************/
void SysTick_Handler(void)
{
	// ϵͳ����ʱ��
	++tick_now;

#if STEP_LEN && (!EventMode)
	if (tick_now % STEP_LEN == 0)
	{
		TimerTask();
	}
#endif
	
}

#endif

// ������ʱ���ɳ���1.864s���˷�Ƶ����£�

uint32_t fac_us = 0;
uint32_t fac_ms = 0;

/*******************************************************************************
 * �� �� ��			: SysTickInit
 * ��������			: ��ʼ��SysTick
 * ע������			: ��
 *******************************************************************************/
void SystickInit(void)
{
	// �˷�Ƶ������ʱʱ�����һ�㣬������Сʱ��ֶ�Ҳ�����
	fac_us = SystemCoreClock / 8000000;
	fac_ms = fac_us * 1000;

	// ����bit2��������ʱ��Դ�����Ե㿪�ļ��鿴�궨�壩
	// �˷�Ƶ����������ʱʱ�����һЩ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

#ifdef InterruptOn
	time_check = 0;
	
	// �����жϼ��Ϊ1ms
	static uint32_t InterruptTime = 1;
	
	// �������ȼ�
	NVIC_SetPriority(SysTick_IRQn, 1);
	// SCB->SHP[11] = 0xf0;

	// �����жϼ��
	SysTick->LOAD = InterruptTime * fac_ms;
	
	// �򿪶�ʱ�����ж�
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#endif

#ifdef InterruptOff
	// ����bit1Ϊ0������ֹ�жϣ�bit1�����ж�ʹ�ܣ�
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
	// ����bit0Ϊ0�����رն�ʱ����bit0��������ʱ����
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);

#endif
}

#ifdef InterruptOn
/*******************************************************************************
 * �� �� ��			: Chrono
 * ��������			: ��ʱ���������ؼ��ʱ��(���뼶)
 * ע������			: ��
 *******************************************************************************/
uint32_t Chrono(void) {
	// ����ǿ�ʼ��ʱ�����������ڲ���¼ VAL �Ĵ�������ֵ
	if(time_check == 0){
		time_piece[0] = tick_now;
		time_piece[1] = SysTick->VAL;
		// ����״̬
		time_check = 1;
		
		return 0;
	}
	// ����ǽ�����ʱ�����������
	else{
		time_piece[2] = tick_now;
		time_piece[3] = SysTick->VAL;
		
		// ���ʱ��Ƭ
		uint32_t time = (time_piece[2] - time_piece[0])*fac_ms + time_piece[1] - time_piece[3];
		// ����״̬
		time_check = 0;
		
		time_piece[3] = time_piece[2] = time_piece[1] = time_piece[0] = 0;
		
		// USART2_Printf("Systick: %x\r\n", time);
		
		return time;
	}
}

/*******************************************************************************
 * �� �� ��			: Delay_ms��iDelay_ms
 * ��������			: ϵͳ������ʱ
 * ע������			: ��������EXTI+NVIC
 *******************************************************************************/
void iDelay_ms(uint32_t nms)
{
	// ֹͣʱ��
	uint32_t tick_to = nms + tick_now;
	// VAL�Ĵ����е�����
	uint32_t val = SysTick->VAL;
	while (1)
	{
		// �Ѿ����ﶨʱʱ����С��ԭʼ����(LOAD�Ĵ�������װ��)
		if (tick_now == tick_to && val <= SysTick->VAL)
			break;
		// ��������ֱ������
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
 * �� �� ��			: Delay_us��iDelay_us
 * ��������			: ϵͳ΢����ʱ
 * ע������			: ��������EXTI+NVIC
 *******************************************************************************/
void iDelay_us(uint32_t nus)
{
	// ĿǰVAL�Ĵ����е�ʱ��(��һʱ�乹��ȷ����ʱ���С)
	uint32_t tick_beg = SysTick->VAL;
	// ��Ҫ��ʱ��ʱ��(�ٶ�΢����ʱ������1ms)
	uint32_t tick_info = nus * fac_us;
	// LOAD�Ĵ����е�ʱ����(Ĭ��Ϊ1ms)
	uint32_t tick_load = SysTick->LOAD;
	// ����װ�ؼ���õ�ʱ��(���ݽṹ���ѭ�����а�Ӧ��/���˳�������ָ���ͬʱҲ�������)
	uint32_t tick_to = (tick_beg + tick_load - tick_info) % tick_load;
	// �쳣�˳�
	uint32_t tick_err = 1 + tick_now;
	// �������ݺ�����������Ҫһ����ʱ��(��δ����/��������˵)
	while (SysTick->VAL != tick_to)
	{ // �쳣�ж�(��ʱ����һms/ͬ������ʱԭ��)
		if (tick_now >= tick_err && SysTick->VAL <= tick_beg)
			break;
	}
}

void Delay_us(uint32_t nus)
{
	// ��������(tick_cnts ���⴦��Ϊ0)
	uint32_t ticks, tick_last, tick_now;
	uint32_t tick_cnts = 0;
	uint32_t tick_load;
	// ticks Ϊ��Ҫ��ʱ��ʱ��
	// tick_load ΪLOAD�Ĵ�����ʱ����(Ĭ��1ms)
	// tick_last ΪVAL�Ĵ�����ǰ����
	ticks = nus * fac_us;
	tick_load = SysTick->LOAD;
	tick_last = SysTick->VAL;

	while (1)
	{
		// tock_now ΪVAL�Ĵ�����ǰʱ�䣨��̬�仯��/ѭ�����£�
		tick_now = SysTick->VAL;
		// ���������ݣ���ֹ��μ��룩
		if (tick_now != tick_last)
		{
			// ���VAL�Ĵ��������˱仯�һ�û������װ�أ�cnt���� Ŀǰ���¼ʱ��Ĳ�ֵ���߹���ʱ��Ƭ��
			if (tick_now < tick_last)
				tick_cnts += tick_last - tick_now;
			// ���VAL�Ĵ���δ�仯���Ѿ�����װ�أ�cnt���� Ŀǰ���¼ʱ��Ĳ�ֵ + ʱ������1ms��
			else
				tick_cnts += tick_load - tick_now + tick_last;
			// ��̬���� tick_last��ѭ�����£�
			tick_last = tick_now;
			// �����cnt�����趨ֵ���˳�ѭ��
			if (tick_cnts >= ticks)
				break;
		}
	}
}

#endif

#ifdef InterruptOff

/*******************************************************************************
 * �� �� ��			: Delay_ms
 * ��������			: Ӳ��������ʱ
 * ע������			: ��
 *******************************************************************************/
void Delay_ms(uint32_t nms)
{
	// ������Ҫ��ʱ��ʱ��
	SysTick->LOAD = nms * fac_ms;
	// ������ֵ��������LOAD�Ĵ������ݴ���VAL�Ĵ���
	SysTick->VAL = 0x00;
	// ������ʱ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// �ȴ�VAL�ĵ�bit16��λ�����ȴ���ʱ�䵽��
	while (!(SysTick->CTRL & (1 << 16)))
		;
	// �رն�ʱ��
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

/*******************************************************************************
 * �� �� ��			: Delay_us
 * ��������			: Ӳ��΢����ʱ
 * ע������			: ��
 *******************************************************************************/
void Delay_us(uint32_t nus)
{
	// ������Ҫ��ʱ��ʱ��
	SysTick->LOAD = nus * fac_us;
	// ������ֵ��������LOAD�Ĵ������ݴ���VAL�Ĵ���
	SysTick->VAL = 0x00;
	// ������ʱ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// �ȴ�VAL�ĵ�bit16��λ�����ȴ���ʱ�䵽��
	while (!(SysTick->CTRL & (1 << 16)))
		;
	// �رն�ʱ��
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	// VAL�Ĵ�����1���0��ʱ�򣬻����һ���ж������ź�
}

#endif
