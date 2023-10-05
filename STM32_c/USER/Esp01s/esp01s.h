#ifndef ESP01S_H
#define ESP01S_H

#include "main.h"
#include "string.h"
#include "Usart_Event.h"

//服务器 IP 和 Wifi 的相关信息请去 esp01s.c 里更改

//此处直接使用 usart2 的 printf 函数作为输出
#define AT_SEND			USART2_Printf
#define ERR_SEND		printf

#define U2_get			readUsart2Buffer

//AT指令集类型
typedef char *_AT_;

//传输协议类型
typedef enum{
	TCP 		= 	0x01,
	UDP
} Client_Type;

//请求事件类型(便于查找)
typedef enum{
	TEST 		= 	0x01,
	START,
	MODE_STA,
	RESTART,
	WIFI,
	GET_IP,
	LOG_IN,
	SEND,
	EXIT,
	FLUSH
} ESP_Type;

//指令复合集
extern _AT_ AT_Initializer[7];
extern _AT_ AT_Sleep[2];
extern _AT_ AT_Loop[3];

void ATInit(void);
void ATRun(_AT_ *commands,unsigned count);
void ATChangeServer(Client_Type ct, char* ip, char* port);
void ATChangeWifi(char* usr, char* pwd);
void STKPrintErr(void);
void ATCheck(void);
void ATReset(void);
void ATRunC(_AT_ commands);
void ATFastRun(_AT_ *commands,unsigned count);
void AT_Test_Func(void);
void AT_OneNET_Http(void);
void ATSendData(unsigned char *data, unsigned len);
void ATtoString(char *buf);
void ATHttpSend(void);
void ATSendDataLen(uint8_t *data, uint16_t len);

typedef struct {
	double tempreture;
	double humidity;
} MQTT_DataType;
	
#endif
