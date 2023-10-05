/* Includes ------------------------------------------------------------------*/
/* Uncomment/Comment the line below to enable/disable peripheral header file inclusion */
// #include "stm32f10x_adc.h"			// ģ��ת����
// #include "stm32f10x_bkp.h"			// ���ݼĴ���
// #include "stm32f10x_can.h"			// ����������������
// #include "stm32f10x_cec.h"			// ����Ƶ�ӿ�
// #include "stm32f10x_crc.h"			// CRC���㵥Ԫ
// #include "stm32f10x_dac.h"			// ��ģת����
// #include "stm32f10x_dbgmcu.h"		// ����֧��
// #include "stm32f10x_dma.h"  // ֱ���ڴ��ȡ������
#include "stm32f10x_exti.h" // �ⲿ�ж��¼�������
// #include "stm32f10x_flash.h"		    	// ����洢��
// #include "stm32f10x_fsmc.h"			// ���ľ�̬�洢��������
#include "stm32f10x_gpio.h" // ͨ����������ӿ�
#include "stm32f10x_i2c.h"  // I2C�ӿ�
// #include "stm32f10x_iwdg.h"			// �������Ź�
// #include "stm32f10x_pwr.h"			// ��Դ/���Ŀ���
#include "stm32f10x_rcc.h" // ��λ��ʱ�ӿ�����
// #include "stm32f10x_rtc.h"			// ʵʱʱ��
// #include "stm32f10x_sdio.h"			// SDIO�ӿ�
// #include "stm32f10x_spi.h"			// ��������ӿ�
#include "stm32f10x_tim.h"			// ��ʱ��
#include "stm32f10x_usart.h" // ͨ��ͬ��/�첽�շ���
#include "stm32f10x_wwdg.h"  // ���ڿ��Ź�
#include "misc.h"
/* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT

/**
 * @brief  The assert_param macro is used for function's parameters check.
 * @param  expr: If expr is false, it calls assert_failed function which reports
 *         the name of the source file and the source line number of the call
 *         that failed. If expr is true, it returns no value.
 * @retval None
 */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
