#ifndef ESP01S_H
#define ESP01S_H

#include "main.h"
#include "string.h"
#include "Usart_Event.h"

//������ IP �� Wifi �������Ϣ��ȥ esp01s.c �����

//�˴�ֱ��ʹ�� usart2 �� printf ������Ϊ���
#define AT_SEND			USART2_Printf
#define ERR_SEND		printf

#define U2_get			readUsart2Buffer

//ATָ�����
typedef char *_AT_;

//����Э������
typedef enum{
	TCP 		= 	0x01,
	UDP
} Client_Type;

//�����¼�����(���ڲ���)
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

//ָ��ϼ�
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
