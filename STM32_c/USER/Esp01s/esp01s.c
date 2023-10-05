#include "esp01s.h"

/*
 * 要格外注意 ESP01S 的供电问题，供电不稳定会导致各类问题
 * 特别指出如果连接热点失败，那就是供电的问题
 * PC14 - RST, PA2 - RX, PA3 - TX
 */
 
 MQTT_DataType sht20_info;

#define HTTP_ONENET					0
#define MQTT_ONENET					1

// SEND_MODE=1 时设备不退出 Send, 直接发送字符串即可
#define SEND_MODE					0

 //产品ID
#define DEVID	"1085742863"
//鉴权信息
#define APIKEY	"vOtKvq406YnkcyhzT=sv0o9BJe0="
 
 // CHECK 开启检查, STACK 开启软件栈
#define CHECK 		0
#define STACK 		1

// DELAY(); 延时函数(延时五秒)
#define DELAY() 	{int i;for(i=0;i<15;++i)Delay_ms(1000);}

// POP();	----	PUSH(c_string);		----	CLEAR();
// 弹出栈顶			压入栈顶						清空栈
#define POP 		(ESP_STACK.F->pop_back)
#define PUSH		(ESP_STACK.F->push_back)
#define CLEAR		(ESP_STACK.F->clear)
 
//TCP Client 透传指令(可以用宏定义替换，但是不容易修改)
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
// HTTP 协议下是 183.230.40.33 :80
_AT_ AT_onenet		=		"AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n";
#endif

#if MQTT_ONENET
// MQTT 协议下是 183.230.40.39 :6002
_AT_ AT_onenet		=		"AT+CIPSTART=\"TCP\",\"183.230.40.33\",6002\r\n";
#endif

_AT_ AT_text1		=		"OK??\r\n";

//数组用于存放成套指令
_AT_ AT_Initializer[7];
_AT_ AT_Sleep[2];
_AT_ AT_Loop[3];

//软件栈运维函数集
typedef struct ESP_functions{
	void (*pop_back)(void);
	void (*push_back)(_AT_ cmd);
	void (*clear)(void);
} ESP_FUNC;

void func_pop(void);						//弹出
void func_psh(_AT_ cmd);					//压入
void func_clr(void);						//清空
	
static ESP_FUNC Func = {func_pop, func_psh, func_clr};

//请求指令软件栈
typedef struct ESP_stk{
	_AT_ Command_Str[20];
	uint8_t counter;
	ESP_FUNC *F;
} ESP_STK;

ESP_STK ESP_STACK;

/*******************************************************************************
 * 函 数 名			: AT_Test_Func
 * 函数功能			: ESP01S(ESP8266)测试代码
 * 注意事项			: 已经具备死循环了
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
 * 函 数 名			: AT_OneNET_Http
 * 函数功能			: 向 OneNET 报告数据
 * 注意事项			: 无
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
 * 函 数 名			: ATInit
 * 函数功能			: 初始化 AT 指令集和指令软件栈
 * 注意事项			: 记得配合其它函数实现发送指令
 *******************************************************************************/
void ATInit(void){
/*
	// 连接本地 TCP 服务代码
	AT_Initializer[0] = AT_start;
	AT_Initializer[1] = AT_mode_sta;
	AT_Initializer[2] = AT_restart;
	AT_Initializer[3] = AT_wifi;
	AT_Initializer[4] = AT_log_in;
	AT_Initializer[5] = AT_mode_to;
	AT_Initializer[6] = AT_send;
*/
	
/*
	// 连接 OneNET 代码 ( HTTP )
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

// 连接 OneNET 代码 ( MQTT )
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

	//ESP8266复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	Delay_ms(500);
}

/*******************************************************************************
 * 函 数 名			: ATReset
 * 函数功能			: 重启ESP01S
 * 注意事项			: GPIOC14
 *******************************************************************************/
void ATReset(void){
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	Delay_ms(500);
	
	ERR_SEND("ESP01S reset OK...\r\n");
}

/*******************************************************************************
 * 函 数 名			: func_pop
 * 函数功能			: ESP_STACK.F->pop_back()
 * 注意事项			: 无
 *******************************************************************************/
void func_pop(void){
	//顶端弹出
	ESP_STACK.Command_Str[ESP_STACK.counter--] = NULL;
}

/*******************************************************************************
 * 函 数 名			: func_psh
 * 函数功能			: ESP_STACK.F->push_back()
 * 注意事项			: 无
 *******************************************************************************/
void func_psh(_AT_ cmd){
	//顶端压入
	if(ESP_STACK.counter>18){
		printf("Stack is already full_filled！\r\n");
	};
	
	ESP_STACK.Command_Str[ESP_STACK.counter++] = cmd;
	
}

/*******************************************************************************
 * 函 数 名			: func_clr
 * 函数功能			: ESP_STACK.F->clear()
 * 注意事项			: 无
 *******************************************************************************/
void func_clr(void){
	STKPrintErr();
	//清空
	ESP_STACK.counter = 0;
	int i = 0;
	for(;i<20;++i){
		ESP_STACK.Command_Str[i] = NULL;
	}
}

/*******************************************************************************
 * 函 数 名			: ATRun
 * 函数功能			: 开始运行指令集的内容
 * 注意事项			: 每个指令有五秒的等待时间，以避免 busy p...
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
 * 函 数 名			: ATFastRun
 * 函数功能			: 开始运行指令集的内容
 * 注意事项			: 无
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
 * 函 数 名			: ATRunC
 * 函数功能			: 开始运行指令的内容
 * 注意事项			: 无
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
 * 函 数 名			: ATChangeServer
 * 函数功能			: 更改连接的服务器
 * 注意事项			: 无
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
 * 函 数 名			: ATChangeWifi
 * 函数功能			: 更改连接的 Wifi
 * 注意事项			: 无
 *******************************************************************************/
void ATChangeWifi(char* usr, char* pwd){
	AT_wifi = strcat(strcat(strcat(strcat("AT+CWJAP=\"",usr),"\",\""),pwd),"\"\r\n");
	
	AT_SEND("%s",AT_wifi);
}

/*******************************************************************************
 * 函 数 名			: STKPrintErr
 * 函数功能			: 串口1打印错误信息
 * 注意事项			: 无
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
 * 函 数 名			: ATCheck
 * 函数功能			: 检测是否发生错误
 * 注意事项			: 无
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
 * 函 数 名			: ATSendData
 * 函数功能			: 发送定长数据
 * 注意事项			: 无
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
 * 函 数 名			: ATtoString
 * 函数功能			: 拼接 onenet 的 http 格式的字符串
 * 注意事项			: 注意要用 http 协议
 *******************************************************************************/
void ATtoString(char *buf)
{
	// text 字符串临时载体
	char text[24];
	// buf1 数据 json
	char buf1[128];
	
	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));
	
	// 此处开始是数据(Cjson 格式)
	strcpy(buf1, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":%0.2f,", sht20_info.tempreture);
	strcat(buf1, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humidity\":%0.2f", sht20_info.humidity);
	strcat(buf1, text);
	
	strcat(buf1, "}");
	
	//此处开始是完整字符串
	sprintf(buf, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\nContent-Length:%d\r\n\r\n", DEVID, APIKEY, strlen(buf1));
					
	strcat(buf, buf1);

	/* 格式结果(http 报文)
	 *
	 * POST /devices/1085742863/datapoints?type=3 HTTP/1.1
	 * api-key:3M51=bGyu1b2g1UsTt=DWKtmgYo=
	 * Host:api.heclouds.com
	 * Content-Length:38
	 *
	 * {"Temperature":xx.xx,"Humidity":xx.xx}
	 * 
	 * 没研究明白那个 datapoint?type=3 是什么意思,也可以直接 datapoint 来着
	 */
}

/*******************************************************************************
 * 函 数 名			: ATSendData
 * 函数功能			: 发送定长数据
 * 注意事项			: 无
 *******************************************************************************/
void ATHttpSend(void)
{
	// 发送的 http 字符串总长度
	char buf[256];
	
	memset(buf, 0, sizeof(buf));
	
	ATtoString(buf);								
	
	ERR_SEND("%s\r\n//------------- Data -------------//\r\n",buf);
	ATSendData((unsigned char *)buf, strlen(buf));	
	
}

/*******************************************************************************
 * 函 数 名			: ATSendDataLen
 * 函数功能			: 发送定长数据
 * 注意事项			: 无
 *******************************************************************************/
void ATSendDataLen(uint8_t *data, uint16_t len)
{
	char AT_SendBuf[32];

	sprintf(AT_SendBuf, "AT+CIPSEND=%d\r\n", len);
	Delay_ms(20);

    USART2_send(data, len);
}
