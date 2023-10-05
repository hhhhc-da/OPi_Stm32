#include "DHT11.h"

#define printf USART2_Printf

/*******************************************************************************
* 函 数 名			: DHT11_Init
* 函数功能			: 初始化 DHT11
* 注意事项			: 使用 GPIOA6
*******************************************************************************/
void DHT11_Init(void){
	//1.打开控制GPIOA的时钟(APB2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.配置结构体	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_6;      	//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 	//推挽输出	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;	//频率

	//3.初始化GPIO
	GPIO_Init(GPIOA, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIOconstructor);
	
	DHT11_OUT();
	// 拉高总线
	PAout(6) = 1;
	PBout(5) = 0;
	PBout(6) = 0;
}

/*******************************************************************************
* 函 数 名			: DHT11_Detect
* 函数功能			: 检测 DHT11
* 注意事项			: 无
*******************************************************************************/
uint8_t DHT11_Detect(void){
	// 默认错误码
	uint8_t flag = 1;
	PBout(6) = !PBout(6);
	
	DHT11_OUT();
	
	// 拉低至少 18ms
	PAout(6) = 0;
	Delay_ms(20);
	
	PAout(6) = 1;
	DHT11_IN();
	Delay_us(35);
	
	uint8_t time = 0;
	while(PAin(6) == 1){
		time++;
		if(time > (uint8_t)20)
			return flag;
		
		Delay_us(2);
	}
	
	flag = 0;
	Delay_us(50);
	
	time = 0;
	while(PAin(6) == 0){
		time++;
		if(time > (uint8_t)20)
			return flag;
		
		Delay_us(2);
	}
	
	// 等待总线拉低
	while(PAin(6) == 1);
	
	return flag;
}

/*******************************************************************************
* 函 数 名			: DHT11_Read
* 函数功能			: 读取 DHT11
* 注意事项			: 无
*******************************************************************************/
uint32_t DHT11_Read(uint8_t *flag){
	uint8_t time = 0;
	
	while(DHT11_Detect()){
		time++;
		if(time > 3)
		{
			// 超时错误码
			*flag = 1;
			return 0;
		}
		Delay_ms(500);
	}
	
	int i = 0, j = 0;
	uint8_t dataTmp = 0, dataCrc = 0;
	uint32_t result = 0;
	for(;i<4;++i){
		PBout(6) = !PBout(6);
		dataTmp = 0;
		
		// 先移位
		result <<= 8;
		
		for(j = 0;j < 8; ++j){
			dataTmp <<= 1;
			// 等待数据
			while(PAin(6) == 0);
			Delay_us(40);
			PBout(5) = !PBout(5);
			
			dataTmp |= PAin(6);
			
			while(PAin(6) == 1);
		}
		
		result |= dataTmp;
		// 校验码
		dataCrc += dataTmp;
		
		// 同步尾信号
		while(PAin(6) == 1);
		while(PAin(6) == 0);
	}
	
	PBout(6) = !PBout(6);
	for(j = 0;j < 8; ++j){
		dataTmp <<= 1;
		// 等待数据
		while(PAin(6) == 0);
		Delay_us(40);
		PBout(5) = !PBout(5);
		
		dataTmp |= PAin(6);
		
		while(PAin(6) == 1);
	}
	PBout(6) = !PBout(6);
	
	
	if(dataTmp != dataCrc){
		// 校验错误码
		*flag = 2;
		return result;
	}
	
	// 正常错误码
	*flag = 0;
	return result;
}

/*******************************************************************************
* 函 数 名			: DHT11_GetData
* 函数功能			: 解析 DHT11
* 注意事项			: 无
*******************************************************************************/
uint8_t DHT11_GetData(unsigned *temp, unsigned *humi){
	// 默认错误码
	uint8_t status = 3;
	
	uint32_t oreData = DHT11_Read(&status);
	
	if(status){
		return status;
	}
	
	// USART2_Printf("OreData: %x\r\n",oreData);
	
	// 我的 DHT11 温度先出，湿度后出，挺逆天的( 我不理解？ )
	*temp = (unsigned)(((oreData >> 28)&0x0f) * 10) + (unsigned)((oreData >> 24)&0x0f);
	
	*humi = (unsigned)(((oreData >> 12)&0x0f) * 10) + (unsigned)((oreData >> 8)&0x0f);
	
	status = 0;
	return status;
}
