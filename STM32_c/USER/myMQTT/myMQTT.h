#ifndef MY_MQTT_H
#define MY_MQTT_H

#include "main.h"
#include "MqttKit.h"

typedef struct {
	// 产品 ID
	int8* productID;
	// 设备 ID
	int8* deviceID;
	// 认证信息
	int8* API_KEY;
	// 连接状态
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

// 统一分配动态内存，防止内存碎片
#define MALLOC_SIZE		30

extern MQTT_BaseType MQTT_userInfo;

#endif
