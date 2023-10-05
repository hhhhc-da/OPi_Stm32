#include "esp01s.h"

/*
 * Ҫ����ע�� ESP01S �Ĺ������⣬���粻�ȶ��ᵼ�¸�������
 * �ر�ָ����������ȵ�ʧ�ܣ��Ǿ��ǹ��������
 * PC14 - RST, PA2 - RX, PA3 - TX
 */
 
 MQTT_DataType sht20_info;

#define HTTP_ONENET					0
#define MQTT_ONENET					1

// SEND_MODE=1 ʱ�豸���˳� Send, ֱ�ӷ����ַ�������
#define SEND_MODE					0

 //��ƷID
#define DEVID	"1085742863"
//��Ȩ��Ϣ
#define APIKEY	"vOtKvq406YnkcyhzT=sv0o9BJe0="
 
 // CHECK �������, STACK �������ջ
#define CHECK 		0
#define STACK 		1

// DELAY(); ��ʱ����(��ʱ����)
#define DELAY() 	{int i;for(i=0;i<15;++i)Delay_ms(1000);}

// POP();	----	PUSH(c_string);		----	CLEAR();
// ����ջ��			ѹ��ջ��						���ջ
#define POP 		(ESP_STACK.F->pop_back)
#define PUSH		(ESP_STACK.F->push_back)
#define CLEAR		(ESP_STACK.F->clear)
 
//TCP Client ͸��ָ��(�����ú궨���滻�����ǲ������޸�)
_AT_ AT_start 		= 		"AT\r\n";
_AT_ AT_mode_sta 	= 		"AT+CWMODE=1\r\n";
_AT_ AT_mode_all	=		"AT+CWMODE=3\r\n";

_AT_ AT_restart 	= 		"AT+RST\r\n";
_AT_ AT_wifi 		= 		"AT+CWJAP=\"Hin\",\"622167bin\"\r\n";
_AT_ AT_IP	 		= 		"AT+CIFSR\r\n";
_AT_ AT_log_in 		= 		"AT+CIPSTART=\"TCP\",\"192.168.43.119\",8080\r\n";
_AT_ AT_mode_to	 	= 		"AT+CIPMODE=1\r\n";
_AT_ AT_send 		= 		"AT+CIPSEND\r\n";

_AT_ AT_exit 		= 		"+++";
_AT_ AT_flush		=		"\r\n";

_AT_ AT_cwmode		=		"AT+CWMODE=1\r\n";
_AT_ AT_cwdhcp		=		"AT+CWDHCP=1,1\r\n";

#if HTTP_ONENET
// HTTP Э������ 183.230.40.33 :80
_AT_ AT_onenet		=		"AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n";
#endif

#if MQTT_ONENET
// MQTT Э������ 183.230.40.39 :6002
_AT_ AT_onenet		=		"AT+CIPSTART=\"TCP\",\"183.230.40.33\",6002\r\n";
#endif

_AT_ AT_text1		=		"OK??\r\n";

//�������ڴ�ų���ָ��
_AT_ AT_Initializer[7];
_AT_ AT_Sleep[2];
_AT_ AT_Loop[3];

//���ջ��ά������
typedef struct ESP_functions{
	void (*pop_back)(void);
	void (*push_back)(_AT_ cmd);
	void (*clear)(void);
} ESP_FUNC;

void func_pop(void);						//����
void func_psh(_AT_ cmd);					//ѹ��
void func_clr(void);						//���
	
static ESP_FUNC Func = {func_pop, func_psh, func_clr};

//����ָ�����ջ
typedef struct ESP_stk{
	_AT_ Command_Str[20];
	uint8_t counter;
	ESP_FUNC *F;
} ESP_STK;

ESP_STK ESP_STACK;

/*******************************************************************************
 * �� �� ��			: AT_Test_Func
 * ��������			: ESP01S(ESP8266)���Դ���
 * ע������			: �Ѿ��߱���ѭ����
 *******************************************************************************/
void AT_Test_Func(void){
	ATInit();
	printf("AT Initialized...\r\n");
	
	ATRun(AT_Initializer,7U);
	printf("AT Initialized OK...\r\n");
	
	int j = 0,k = 1;
	while(1){
		EventLoop();
		
		if(++j>2)
			if(k == 1){
				ATFastRun(AT_Sleep,2);
				k--;
			}
			else;
		else{
			ATFastRun(AT_Loop,3U);
		}
	}
}

/*******************************************************************************
 * �� �� ��			: AT_OneNET_Http
 * ��������			: �� OneNET ��������
 * ע������			: ��
 *******************************************************************************/
void AT_OneNET_Http(void){
	ATInit();
	printf("AT Initialized...\r\n");
	
	ATRun(AT_Initializer,
#if SEND_MODE
	7U
#else
	6U
#endif
	);
	printf("AT Initialized OK...\r\n");
	
	double data = 0.5F;
	
	while(1){
		EventLoop();
		
		if(data>90){
			data = 0.5F;
		}
		
		sht20_info.humidity = data;
		sht20_info.tempreture = data;
		
		data = data + 1;
		
		ATHttpSend();
		DELAY();
	}
}

/*******************************************************************************
 * �� �� ��			: ATInit
 * ��������			: ��ʼ�� AT ָ���ָ�����ջ
 * ע������			: �ǵ������������ʵ�ַ���ָ��
 *******************************************************************************/
void ATInit(void){
/*
	// ���ӱ��� TCP �������
	AT_Initializer[0] = AT_start;
	AT_Initializer[1] = AT_mode_sta;
	AT_Initializer[2] = AT_restart;
	AT_Initializer[3] = AT_wifi;
	AT_Initializer[4] = AT_log_in;
	AT_Initializer[5] = AT_mode_to;
	AT_Initializer[6] = AT_send;
*/
	
/*
	// ���� OneNET ���� ( HTTP )
	AT_Initializer[0] = AT_start;
	AT_Initializer[1] = AT_mode_all;
	AT_Initializer[2] = AT_restart;
	AT_Initializer[3] = AT_wifi;
	AT_Initializer[4] = AT_onenet;
	AT_Initializer[5] = AT_mode_to;

#if SEND_MODE
	AT_Initializer[6] = AT_send;
#endif
*/

// ���� OneNET ���� ( MQTT )
	AT_Initializer[0] = AT_start;
	AT_Initializer[1] = AT_mode_sta;
	AT_Initializer[2] = AT_wifi;
	AT_Initializer[3] = AT_onenet;
	AT_Initializer[4] = AT_mode_to;

#if SEND_MODE
	AT_Initializer[6] = AT_send;
#endif
	
	AT_Sleep[0] = AT_exit;
	AT_Sleep[1] = AT_flush;
	
	/*
	AT_Loop[0] = AT_start;
	AT_Loop[1] = AT_restart;
	AT_Loop[2] = AT_IP;
	*/
	
	ESP_STACK.F = &Func;
	ESP_STACK.counter = 0;
	
	int i = 0;
	for(;i<20;++i){
		ESP_STACK.Command_Str[i] = NULL;
	}
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	Delay_ms(500);
}

/*******************************************************************************
 * �� �� ��			: ATReset
 * ��������			: ����ESP01S
 * ע������			: GPIOC14
 *******************************************************************************/
void ATReset(void){
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	Delay_ms(500);
	
	ERR_SEND("ESP01S reset OK...\r\n");
}

/*******************************************************************************
 * �� �� ��			: func_pop
 * ��������			: ESP_STACK.F->pop_back()
 * ע������			: ��
 *******************************************************************************/
void func_pop(void){
	//���˵���
	ESP_STACK.Command_Str[ESP_STACK.counter--] = NULL;
}

/*******************************************************************************
 * �� �� ��			: func_psh
 * ��������			: ESP_STACK.F->push_back()
 * ע������			: ��
 *******************************************************************************/
void func_psh(_AT_ cmd){
	//����ѹ��
	if(ESP_STACK.counter>18){
		printf("Stack is already full_filled��\r\n");
	};
	
	ESP_STACK.Command_Str[ESP_STACK.counter++] = cmd;
	
}

/*******************************************************************************
 * �� �� ��			: func_clr
 * ��������			: ESP_STACK.F->clear()
 * ע������			: ��
 *******************************************************************************/
void func_clr(void){
	STKPrintErr();
	//���
	ESP_STACK.counter = 0;
	int i = 0;
	for(;i<20;++i){
		ESP_STACK.Command_Str[i] = NULL;
	}
}

/*******************************************************************************
 * �� �� ��			: ATRun
 * ��������			: ��ʼ����ָ�������
 * ע������			: ÿ��ָ��������ĵȴ�ʱ�䣬�Ա��� busy p...
 *******************************************************************************/
void ATRun(_AT_ *commands,unsigned count){
	_AT_ *Initializer = commands;
	
	unsigned i;
	
	for(i = 0;i<count;++i){
		AT_SEND("%s",commands[i]);
		
#if STACK
		ESP_STACK.F->push_back(commands[i]);
		
		ERR_SEND("%s\r\nAT run OK...\r\n",commands[i]);
		
		STKPrintErr();
#endif
		
		DELAY();

#if CHECK
		ATCheck();
#endif
		
	}
	
	ERR_SEND("AT all run OK...\r\n");
	
#if STACK
	ESP_STACK.F->clear();
#endif
}

/*******************************************************************************
 * �� �� ��			: ATFastRun
 * ��������			: ��ʼ����ָ�������
 * ע������			: ��
 *******************************************************************************/
void ATFastRun(_AT_ *commands,unsigned count){
	_AT_ *Initializer = commands;
	
	unsigned i;
	
	for(i = 0;i<count;++i){
		AT_SEND("%s",commands[i]);
		
#if STACK
		ESP_STACK.F->push_back(commands[i]);
#endif
		
		Delay_ms(20);

#if CHECK
		ATCheck();
#endif
		
	}
	
	ERR_SEND("%s\r\n","AT run OK...");
	
#if STACK
	ESP_STACK.F->clear();
#endif
}

/*******************************************************************************
 * �� �� ��			: ATRunC
 * ��������			: ��ʼ����ָ�������
 * ע������			: ��
 *******************************************************************************/
void ATRunC(_AT_ commands){
#if STACK
		ESP_STACK.F->push_back(commands);
#endif
	
	AT_SEND("%s",commands);
	DELAY();

#if STACK
	ESP_STACK.F->clear();
#endif
}

/*******************************************************************************
 * �� �� ��			: ATChangeServer
 * ��������			: �������ӵķ�����
 * ע������			: ��
 *******************************************************************************/
void ATChangeServer(Client_Type ct, char* ip, char* port){
	if(ct == UDP){
		AT_log_in = strcat(strcat(strcat(strcat("AT+CIPSTART=\"TCP\",\"",ip),"\","),port),"\r\n");
	}
	else if(ct == TCP){
		AT_log_in = strcat(strcat(strcat(strcat("AT+CIPSTART=\"UDP\",\"",ip),"\","),port),"\r\n");
	}
	
	AT_SEND("%s",AT_log_in);
}

/*******************************************************************************
 * �� �� ��			: ATChangeWifi
 * ��������			: �������ӵ� Wifi
 * ע������			: ��
 *******************************************************************************/
void ATChangeWifi(char* usr, char* pwd){
	AT_wifi = strcat(strcat(strcat(strcat("AT+CWJAP=\"",usr),"\",\""),pwd),"\"\r\n");
	
	AT_SEND("%s",AT_wifi);
}

/*******************************************************************************
 * �� �� ��			: STKPrintErr
 * ��������			: ����1��ӡ������Ϣ
 * ע������			: ��
 *******************************************************************************/
void STKPrintErr(void){
	ERR_SEND("/-------------------------------\r\n");
	int i = 0;
	for(;i<20;++i){
		if(ESP_STACK.Command_Str[i] == NULL)
			break;
		
		ERR_SEND("%s",ESP_STACK.Command_Str[i]);
	}
	ERR_SEND("////////////////////////////////\r\n");
}

/*******************************************************************************
 * �� �� ��			: ATCheck
 * ��������			: ����Ƿ�������
 * ע������			: ��
 *******************************************************************************/
void ATCheck(void){
	if(strcmp(U2_get(),"\rERROR\r")){
		STKPrintErr();
		
		ERR_SEND("ESP01S reset start...\r\n");
		
		ATReset();
		ATInit();
	}
}

/*******************************************************************************
 * �� �� ��			: ATSendData
 * ��������			: ���Ͷ�������
 * ע������			: ��
 *******************************************************************************/
void ATSendData(unsigned char *data, unsigned len)
{

	char buf[32];
	
	
#if !SEND_MODE
	sprintf(buf, "AT+CIPSEND=%d\r\n", len);
	
	PUSH(buf);
	
	AT_SEND("%s",buf);
#endif
	
	Delay_ms(1000);
	
	AT_SEND("%s",data);

#if !SEND_MODE
	CLEAR();
#endif
}

/*******************************************************************************
 * �� �� ��			: ATtoString
 * ��������			: ƴ�� onenet �� http ��ʽ���ַ���
 * ע������			: ע��Ҫ�� http Э��
 *******************************************************************************/
void ATtoString(char *buf)
{
	// text �ַ�����ʱ����
	char text[24];
	// buf1 ���� json
	char buf1[128];
	
	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));
	
	// �˴���ʼ������(Cjson ��ʽ)
	strcpy(buf1, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":%0.2f,", sht20_info.tempreture);
	strcat(buf1, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humidity\":%0.2f", sht20_info.humidity);
	strcat(buf1, text);
	
	strcat(buf1, "}");
	
	//�˴���ʼ�������ַ���
	sprintf(buf, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\nContent-Length:%d\r\n\r\n", DEVID, APIKEY, strlen(buf1));
					
	strcat(buf, buf1);

	/* ��ʽ���(http ����)
	 *
	 * POST /devices/1085742863/datapoints?type=3 HTTP/1.1
	 * api-key:3M51=bGyu1b2g1UsTt=DWKtmgYo=
	 * Host:api.heclouds.com
	 * Content-Length:38
	 *
	 * {"Temperature":xx.xx,"Humidity":xx.xx}
	 * 
	 * û�о������Ǹ� datapoint?type=3 ��ʲô��˼,Ҳ����ֱ�� datapoint ����
	 */
}

/*******************************************************************************
 * �� �� ��			: ATSendData
 * ��������			: ���Ͷ�������
 * ע������			: ��
 *******************************************************************************/
void ATHttpSend(void)
{
	// ���͵� http �ַ����ܳ���
	char buf[256];
	
	memset(buf, 0, sizeof(buf));
	
	ATtoString(buf);								
	
	ERR_SEND("%s\r\n//------------- Data -------------//\r\n",buf);
	ATSendData((unsigned char *)buf, strlen(buf));	
	
}

/*******************************************************************************
 * �� �� ��			: ATSendDataLen
 * ��������			: ���Ͷ�������
 * ע������			: ��
 *******************************************************************************/
void ATSendDataLen(uint8_t *data, uint16_t len)
{
	char AT_SendBuf[32];

	sprintf(AT_SendBuf, "AT+CIPSEND=%d\r\n", len);
	Delay_ms(20);

    USART2_send(data, len);
}
