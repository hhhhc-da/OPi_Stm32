#include "HX711.h"

/*******************************************************************************
* 函 数 名			: HX711_Init
* 函数功能			: 初始化 HX711
* 注意事项			: SCK - GPIOB1, DT - GPIOB0
*******************************************************************************/
void HX711_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_1;      	//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_Out_PP; 	//推挽输出	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;	//频率

	GPIO_Init(GPIOB, &GPIOconstructor);
	
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_0;      	//GPIO引脚
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IPU; 	//上拉输入
	
	GPIO_Init(GPIOB, &GPIOconstructor);
}

/*******************************************************************************
* 函 数 名			: HX711_Read
* 函数功能			: 读取 HX711 原数据
* 注意事项			: 无
*******************************************************************************/
unsigned HX711_Read(void){
	unsigned data = 0;
	
	// 初始化信号
	SCK_LOW;
	Delay_us(1);
	
	// 等待 SDA 拉低
	while(SDA);
	
	int i = 0;
	for(;i < 24; ++i){
		SCK_HIGH;
		Delay_us(1);
		
		data <<= 1;
		
		SCK_LOW;
		if(SDA)
			data++;
	}
	
	SCK_HIGH;
	data ^= 0x800000;
	
	Delay_us(1);
	SCK_LOW;
	
	// USART2_Printf("HX711 OreData: %x\r\n", data);
	
	return data;
}

/*******************************************************************************
* 函 数 名			: HX711_GetWeight
* 函数功能			: 转换 HX711 数据
* 注意事项			: 只可以保留两位小数
*******************************************************************************/
uint8_t HX711_GetWeight(unsigned* pointLeft, unsigned* pointRight){
	unsigned oreData = HX711_Read();
	
	// 整数部分
	*pointLeft = (oreData - HX711_OFFSET)/HX711_SCALE;
	// 保留两位小数
	*pointRight = ((oreData - HX711_OFFSET)%HX711_SCALE)*100/HX711_SCALE;
	
	if(oreData < HX711_OFFSET){
		*pointLeft = 0;
		*pointRight = 0;
	}
	
	// 校验是否全为0
	unsigned data = (*pointLeft) | (*pointRight);
	
	if(data > 0)
		return 0;
	else
		return 1;
}