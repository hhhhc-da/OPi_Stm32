#include "PWM.h"
/*******************************************************************************
 * 函 数 名			: TIM3_PWM_Init
 * 函数功能			: 初始化 TIM3 为  PWM 输出
 * @params		arr	: 重装载值
 * 				psc	: 预分频系数
 * 注意事项			: 无
 *******************************************************************************/
void TIM3_PWM_Init(uint16_t arr,uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStrue;
    TIM_OCInitTypeDef TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3和相关GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// 使能GPIOB时钟(LED在PB5引脚),使能AFIO时钟(定时器3通道2需要重映射到PB5引脚)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStrue.GPIO_Pin = GPIO_Pin_5;			// TIM_CH2
    GPIO_InitStrue.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽
    GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;	//设置最大输出速度
    GPIO_Init(GPIOB, &GPIO_InitStrue);			//GPIO端口初始化设置
    
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
    
    TIM_TimeBaseInitStrue.TIM_Period = arr;		//设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc;	//预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;	//计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue);				//TIM3初始化设置(设置PWM的周期)
    
    TIM_OCInitStrue.TIM_OCMode = TIM_OCMode_PWM1;					// PWM模式1:CNT<CCR时输出有效
    TIM_OCInitStrue.TIM_OCPolarity = TIM_OCPolarity_High;			// 设置极性-有效为高电平
    TIM_OCInitStrue.TIM_OutputState = TIM_OutputState_Enable;		// 输出使能
    TIM_OC2Init(TIM3, &TIM_OCInitStrue);							//TIM3的通道2PWM 模式设置

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);	//使能预装载寄存器
    
    TIM_Cmd(TIM3, ENABLE);	//使能TIM3
}

/*******************************************************************************
 * 函 数 名			: PWM_test
 * 函数功能			: 测试 PWM 输出
 * 注意事项			: led0pwmval 上限 899，大小随意设定。
 *******************************************************************************/
void PWM_test(void)
 {    
    uint8_t i=1;         			// 设置方向 0:变暗 1:变亮
    uint16_t led0pwmval;        // 设置CCR值

    TIM3_PWM_Init(899,0); 						//设置频率为80KHz，公式为：溢出时间Tout=(arr+1)(psc+1)/Tclk
                                                //Tclk为通用定时器的时钟，如果APB1没有分频，则就为系统时钟，72MHZ
                                                //PWM时钟频率=72000000/(899+1) = 80KHZ,设置自动装载值899,预分频系数0(不分频)
    while(1)
    {    
        Delay_ms(10); 
		
        if(i)
			led0pwmval++;        // 由暗变亮
		else
			led0pwmval--;        // 由亮变暗
        
        if(led0pwmval==0 || led0pwmval>100)
			i = !i;
        
        TIM_SetCompare2(TIM3, led0pwmval);        //改变比较值 TIM3->CCR2 达到调节占空比的效果
    }
 }
 