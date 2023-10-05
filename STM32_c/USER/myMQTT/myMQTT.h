#ifndef MY_MQTT_H
#define MY_MQTT_H

#include "main.h"
#include "MqttKit.h"

typedef struct {
	// ��Ʒ ID
	int8* productID;
	// �豸 ID
	int8* deviceID;
	// ��֤��Ϣ
	int8* API_KEY;
	// ����״̬
	uint1 connection;
	
} MQTT_BaseType;

uint8_t* getIPD(uint16_t timeout);
uint8_t MQTT_ConnectInit(void);
uint8_t mallocJson(char **buf);
void MQTT_Transmit(char *json);
void MQTT_ReceiveDeal(uint8_t *cmd);
void MQTT_Test(void);
uint1 MQTT_Disconnect(void);
void MQTT_Reset(void);
uint8_t MQTT_DetectHeartBeat(void);

// ͳһ���䶯̬�ڴ棬��ֹ�ڴ���Ƭ
#define MALLOC_SIZE		30

extern MQTT_BaseType MQTT_userInfo;

#endif
