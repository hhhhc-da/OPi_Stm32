#include "PWM.h"
/*******************************************************************************
 * �� �� ��			: TIM3_PWM_Init
 * ��������			: ��ʼ�� TIM3 Ϊ  PWM ���
 * @params		arr	: ��װ��ֵ
 * 				psc	: Ԥ��Ƶϵ��
 * ע������			: ��
 *******************************************************************************/
void TIM3_PWM_Init(uint16_t arr,uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStrue;
    TIM_OCInitTypeDef TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3�����GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// ʹ��GPIOBʱ��(LED��PB5����),ʹ��AFIOʱ��(��ʱ��3ͨ��2��Ҫ��ӳ�䵽PB5����)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStrue.GPIO_Pin = GPIO_Pin_5;			// TIM_CH2
    GPIO_InitStrue.GPIO_Mode = GPIO_Mode_AF_PP;	// ��������
    GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;	//�����������ٶ�
    GPIO_Init(GPIOB, &GPIO_InitStrue);			//GPIO�˿ڳ�ʼ������
    
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
    
    TIM_TimeBaseInitStrue.TIM_Period = arr;		//�����Զ���װ��ֵ
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc;	//Ԥ��Ƶϵ��
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;	//�������������
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;		//ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue);				//TIM3��ʼ������(����PWM������)
    
    TIM_OCInitStrue.TIM_OCMode = TIM_OCMode_PWM1;					// PWMģʽ1:CNT<CCRʱ�����Ч
    TIM_OCInitStrue.TIM_OCPolarity = TIM_OCPolarity_High;			// ���ü���-��ЧΪ�ߵ�ƽ
    TIM_OCInitStrue.TIM_OutputState = TIM_OutputState_Enable;		// ���ʹ��
    TIM_OC2Init(TIM3, &TIM_OCInitStrue);							//TIM3��ͨ��2PWM ģʽ����

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);	//ʹ��Ԥװ�ؼĴ���
    
    TIM_Cmd(TIM3, ENABLE);	//ʹ��TIM3
}

/*******************************************************************************
 * �� �� ��			: PWM_test
 * ��������			: ���� PWM ���
 * ע������			: led0pwmval ���� 899����С�����趨��
 *******************************************************************************/
void PWM_test(void)
 {    
    uint8_t i=1;         			// ���÷��� 0:�䰵 1:����
    uint16_t led0pwmval;        // ����CCRֵ

    TIM3_PWM_Init(899,0); 						//����Ƶ��Ϊ80KHz����ʽΪ�����ʱ��Tout=(arr+1)(psc+1)/Tclk
                                                //TclkΪͨ�ö�ʱ����ʱ�ӣ����APB1û�з�Ƶ�����Ϊϵͳʱ�ӣ�72MHZ
                                                //PWMʱ��Ƶ��=72000000/(899+1) = 80KHZ,�����Զ�װ��ֵ899,Ԥ��Ƶϵ��0(����Ƶ)
    while(1)
    {    
        Delay_ms(10); 
		
        if(i)
			led0pwmval++;        // �ɰ�����
		else
			led0pwmval--;        // �����䰵
        
        if(led0pwmval==0 || led0pwmval>100)
			i = !i;
        
        TIM_SetCompare2(TIM3, led0pwmval);        //�ı�Ƚ�ֵ TIM3->CCR2 �ﵽ����ռ�ձȵ�Ч��
    }
 }
 