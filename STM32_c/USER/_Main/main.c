
#include "main.h"
/*
 * ģ�����ʾ�� STM32 �����ļ�
 * ���ߣ�Mr.Nanoka
 * ����ؾ�
 */

/*
 * MQx		- GPIOA7		, GPIOB4(DO)
 * ʹ���ж��� - EXTI1
 *
 * ������	- GPIOB5
 * ( ��������û����Ŷ~ )
 *
 * DHT11	- GPIOA6
 *
 * HX711	- GPIOB1(SCK)	, GPIOB0(DT)
 *
 * HC-SR04	- GPIOA5(Trig)	, GPIOA4(Echo)	�� GPIOC14(Key)
 * ʹ���ж��� - EXTI4
 *
 * OLED		- GPIOB6(SCL)	, GPIOB7(SDA)
 * ( OLED��û����Ŷ~ )
 *
 * USART1	- GPIOA9(TX)	, GPIOA10(RX)
 * USART2	- GPIOA2(TX)	, GPIOA3(RX)
 *
 * �ź���	- GPIOB8(Bit0)	, GPIOB9(Bit1)
 * 			- GPIOB10(Bit2)	, GPIOB11(SYNC)
 */

// ��ʼ״̬������
uint8_t flag = 0;
XgkData xgkData = {0, 0, 0, 0, 0, 0, 0};

unsigned tmp1,tmp2;
char tmp[20];
 
int main(void){
	//��ʼ������
	periphInit();

	while(1){
		uint8_t readMode = 0x00;
		
		if(PBin(8) == 1)
			readMode |= 1U;
		if(PBin(9) == 1)
			readMode |= 1U<<1;
		if(PBin(10) == 1)
			readMode |= 1U<<2;
		
		if(readMode == 0x01)
			USART2_Printf("%u", xgkData.DHT11_Humi);
		if(readMode == 0x02)
			USART2_Printf("%u", xgkData.DHT11_Temp);
		if(readMode == 0x03)
			USART2_Printf("%u", xgkData.MQx_Flag);
		if(readMode == 0x04)
			USART2_Printf("%u", xgkData.CS_LengthH);
		if(readMode == 0x05)
			USART2_Printf("%u", xgkData.CS_LengthL);
		if(readMode == 0x06)
			USART2_Printf("%u", xgkData.HX_WeightH);
		if(readMode == 0x07)
			USART2_Printf("%u", xgkData.HX_WeightL);
		
		MQX_work();
		
		sprintf(tmp,"MQX: %1u    ",dangerManager);
		OLED_ShowStr(10,4,(unsigned char*)tmp, 1);
		
		DHT11_Work(&tmp1, &tmp2);
		xgkData.DHT11_Humi = tmp2;
		xgkData.DHT11_Temp = tmp1;
		
		sprintf(tmp,"HUMI: %02u    ",tmp2);
		OLED_ShowStr(10,0,(unsigned char*)tmp, 1);
		
		sprintf(tmp,"TEMP: %02u    ",tmp1);
		OLED_ShowStr(10,1,(unsigned char*)tmp, 1);
		
		Delay_ms(500);
		
		HX711_Work(&tmp1, &tmp2);
		xgkData.HX_WeightH = tmp1;
		xgkData.HX_WeightL = tmp2;
		
		sprintf(tmp,"WEIGHT: %02u.%02u    ",tmp1, tmp2);
		OLED_ShowStr(10,2,(unsigned char*)tmp, 1);
		
		Delay_ms(500);
		PBout(11) = !PBout(11);
		
		CS_Work(&tmp1, &tmp2);
		xgkData.CS_LengthH = tmp1;
		xgkData.CS_LengthL = tmp2;
		
		sprintf(tmp,"DIST: %02u.%06u    ",tmp1, tmp2);
		OLED_ShowStr(10,3,(unsigned char*)tmp, 1);
		
		Delay_ms(1000);
		
		PCout(13) = !PCout(13);
		
		
	}
}

void periphInit(void){
	// ��λ�ź�λ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIOconstructor;
	GPIOconstructor.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;      	//GPIO����
	GPIOconstructor.GPIO_Mode  = GPIO_Mode_IPD; 	//�������	
	GPIOconstructor.GPIO_Speed = GPIO_Speed_50MHz;	//Ƶ��

	GPIO_Init(GPIOB, &GPIOconstructor);
	
	PBout(11) = 0;
	
	SystickInit();
	
	USART1Init(115200);
	USART2Init(115200);
	
	flag = MQx_Init();
	CSA_Init();
	DHT11_Init();
	HX711_Init();
	
	IIC_Init();
	OLED_Init();
	
	OLED_Clear();
	
	OLED_ShowStr(10,6,(unsigned char*)"OLED DEBUG SCREEN   ", 1);
	
	printf("Initialized OK, flag = %d\r\n",flag);
	
	wwdgInit();
}

void CS_Work(unsigned *dataHigh, unsigned *dataLow){
	// �������ģ��
	CSA_SendTrig();
	
	wDelay_ms(20);
	
	uint32_t oreData = CSA_GetOreData();
	
	if(oreData == 0xffffffff)
		return ;
	
	unsigned data = CSA_GetData();
	
	*dataHigh = data / 1000000;
	*dataLow = data % 1000000;
}

void DHT11_Work(unsigned *temp, unsigned *humi){
	// DHT11ģ��
	unsigned DH = 0, DL = 0;
	uint8_t ret = DHT11_GetData(&DH, &DL);
	
	*temp = DH;
	*humi = DL;
}

void HX711_Work(unsigned *dataHigh, unsigned *dataLow){
	// HX711ģ��
	unsigned DH = 0, DL = 0;
	uint8_t ret = HX711_GetWeight(&DH, &DL);
	
	*dataHigh = DH;
	*dataLow = DL;
}

void MQX_work(void){
	if(dangerManager){
		xgkData.MQx_Flag = 1;
	}
}
