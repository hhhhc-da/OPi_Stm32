
#include "wwdg.h"

#define WWDG_TIMER_CALLBACK 1

#if WWDG_TIMER_CALLBACK

cb Delay_callBackFunction = IDLE;

#endif

// wwdg ��ʱ��, ���ں��붨ʱ(һ��Ƶ, 5.0062ms ��ʱ��)
static const uint8_t pre_cnt = 0x2B + 0x40;
static uint32_t delayCount = 0;

// ���Ź��ܽ�����
static uint64_t wwdg_all_ticks = 0;

// ��ʱ����
static unsigned round = 0;

/*******************************************************************************
* �� �� ��			: wwdgInit
* ��������			: ��ʼ�� wwdg
* ע������			: ��
*******************************************************************************/
void wwdgInit(void){
	// WWDG ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); 
	
	// PC13��ʾ��������ʾ���Ź��жϹ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_13;
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIOconstructor);

	
	// PCLK1 һ��Ƶ, �� 36MHz
	WWDG_SetPrescaler(WWDG_Prescaler_1);
	// ���ô���ֵ, Ҳ�������üĴ��� WWDG_CFR �ĺ���λ��ʱ��ֵ(������ WWDG_CR ���Ƚ�)
	WWDG_SetWindowValue(pre_cnt);
	// ʹ����ǰ�����ж�
	WWDG_EnableIT();
	// ���ÿ��ƼĴ��� WWDG_CR �ĺ���λ��ʱ��ֵ
	WWDG_SetCounter(pre_cnt);
	// �����ǰ�����жϱ�־λ 
	WWDG_ClearFlag();
	// ʹ�ܿ��Ź��ж�
	WWDG_NVIC_Init();
	// ʹ�ܿ��Ź�
	WWDG_Enable(pre_cnt);
	
	// Print("WWDG_Initialization OK...\r\n");
}

/*******************************************************************************
* �� �� ��			: WWDG_NVIC_Init
* ��������			: �������ڿ��Ź��ж�
* ע������			: ��
*******************************************************************************/
void WWDG_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 2:2 �����ж����ȼ�λ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//WWDG�ж�(�Ƚϸ�)
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* �� �� ��			: WWDG_IRQHandler
* ��������			: ���ڿ��Ź��жϷ������
* ע������			: ι��Ҫ�첻Ȼ�ͼ�
*******************************************************************************/
void WWDG_IRQHandler(void)
{
	// ι��, ��ֹ��λ
	WWDG_SetCounter(pre_cnt);

	// �����ǰ�����жϱ�־λ
	WWDG_ClearFlag();	  
	
	// ���Ź��ܽ�����
	++wwdg_all_ticks;
	
	// ��ʱװ��
	if(round != 0)
		--round;
}

/*******************************************************************************
* �� �� ��			: wDelay_ms
* ��������			: ���Ź�������ʱ
* ע������			: ֻ����5�ı���(�������������ȡ������)
*******************************************************************************/
void wDelay_ms(unsigned ms){
	// ��ֵ
	unsigned cr = WWDG->CR;
	round = ms / 5;
	
	// ��������������ж� expr1 ���˳�, ������в���һ�����ڣ����ж� expr2 ���˳�
	while(round != 0 || WWDG->CR > cr);
	
#if WWDG_TIMER_CALLBACK
	Delay_callBackFunction();
#endif
}

/*******************************************************************************
* �� �� ��			: getTime
* ��������			: ����ϵͳ����ʱ��
* ע������			: �ֶ�ֵ: 0.005s
*******************************************************************************/
uint64_t getTime(void){
	// ֱ�Ӵ�ӡʱ��
	Print("Time: %fs\r\n",5.0*wwdg_all_ticks/1000);
	
	// ���غ�����
	return 5 * wwdg_all_ticks;
}


uint8_t wtime_check = 0;
uint64_t wtime_piece[2] = { 0, 0 };


/*******************************************************************************
 * �� �� ��			: wChrono
 * ��������			: ��ʱ���������ؼ��ʱ��(�뼶)
 * ע������			: ��
 *******************************************************************************/
double wChrono(void) {
	// ����ǿ�ʼ��ʱ�����������ڲ���¼ VAL �Ĵ�������ֵ
	if(wtime_check == 0){
		wtime_piece[0] = wwdg_all_ticks;
		// ����״̬
		wtime_check = 1;
		
		return 0;
	}
	// ����ǽ�����ʱ�����������
	else{
		wtime_piece[1] = wwdg_all_ticks;
		// ���ʱ��Ƭ
		uint64_t time = wtime_piece[1] - wtime_piece[0];
		// ����״̬
		wtime_check = 0;
		
		return (time * 5.0062 / 1000);
	}
}

#if WWDG_TIMER_CALLBACK
/*******************************************************************************
* �� �� ��			: IDLE
* ��������			: �� wDelay �ص����ӵļ����溯��
* ע������			: ������Ǹ��һ���
*******************************************************************************/
void IDLE(void){
	__asm {
		ADD delayCount, delayCount, #1
	}
}
#endif
