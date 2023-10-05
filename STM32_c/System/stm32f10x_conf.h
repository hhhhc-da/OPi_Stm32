/* Includes ------------------------------------------------------------------*/
/* Uncomment/Comment the line below to enable/disable peripheral header file inclusion */
// #include "stm32f10x_adc.h"			// 模数转换器
// #include "stm32f10x_bkp.h"			// 备份寄存器
// #include "stm32f10x_can.h"			// 控制器局域网总线
// #include "stm32f10x_cec.h"			// 音视频接口
// #include "stm32f10x_crc.h"			// CRC计算单元
// #include "stm32f10x_dac.h"			// 数模转换器
// #include "stm32f10x_dbgmcu.h"		// 调试支持
// #include "stm32f10x_dma.h"  // 直接内存存取控制器
#include "stm32f10x_exti.h" // 外部中断事件控制器
// #include "stm32f10x_flash.h"		    	// 闪存存储器
// #include "stm32f10x_fsmc.h"			// 灵活的静态存储器控制器
#include "stm32f10x_gpio.h" // 通用输入输出接口
#include "stm32f10x_i2c.h"  // I2C接口
// #include "stm32f10x_iwdg.h"			// 独立看门狗
// #include "stm32f10x_pwr.h"			// 电源/功耗控制
#include "stm32f10x_rcc.h" // 复位与时钟控制器
// #include "stm32f10x_rtc.h"			// 实时时钟
// #include "stm32f10x_sdio.h"			// SDIO接口
// #include "stm32f10x_spi.h"			// 串行外设接口
#include "stm32f10x_tim.h"			// 定时器
#include "stm32f10x_usart.h" // 通用同步/异步收发器
#include "stm32f10x_wwdg.h"  // 窗口看门狗
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
