
#include "myMQTT.h"

#define delay			Delay_ms
#define MQTT_update		ATSendDataLen

#define BUF				bufRX2.buf
#define BUF_pos			bufRX2.index
#define BUF_clear		ClearRXBuf

// 定位两个 LED
#define LED1			PAout(3)
#define LED2			PAout(2)

// 连接全局变量
MQTT_BaseType MQTT_userInfo = {
	(int8*)"578642",
	(int8*)"1055010844",
	(int8*)"fwsrtcTO1=EJ38ScOeW886RHBZI=",
	0
};

/*******************************************************************************
 * 函 数 名			: getIPD
 * 函数功能			: 获取收到的消息
 * 注意事项			: 无
 *******************************************************************************/
uint8_t* getIPD(uint16_t timeout)
{
	printf("MQTT get IPD start\r\n");
	
	char *pIPD = 0;
	uint8_t pos = BUF_pos;
	do {
		delay(1000);
		// 如果指针 1s 没动，我们就认为他接收完了
		if(pos == BUF_pos){
			
			// 保护临界资源
			// getUSART2();
			pIPD = strstr((char *)BUF, "IPD,");

			if(pIPD == 0) {
				printf("read IPD = 0\r\n");
			}
			else {
				pIPD = strchr(pIPD, ':');
				if(pIPD != 0) {
					pIPD++;
					printf("get result for IPD\r\n");
					return (uint8_t *)pIPD;
				} else {
					printf("No read result for IPD\r\n");
					return NULL;
				}
			}
		}
	} while(timeout--);	

	printf("read IPD timeout\r\n");
	// 被动退出
	//giveUSART2();
	return NULL;
}

/*******************************************************************************
 * 函 数 名			: MQTT_ConnectInit
 * 函数功能			: 连接 MQTT4
 * 注意事项			: 无
 *******************************************************************************/
uint8_t MQTT_ConnectInit(void)
{
	printf("MQTT connect initialization start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包
	uint8_t *dataPtr;
	uint8_t status = 1;
	
	BUF_clear();
	
	// 发送连接请求
	if (MQTT_PacketConnect(MQTT_userInfo.productID, MQTT_userInfo.API_KEY, MQTT_userInfo.deviceID, 300, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		MQTT_update(mqttPacket._data, mqttPacket._len); // 上传平台
			
		dataPtr = getIPD(250);							// 等待平台响应
		
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
			switch (MQTT_UnPacketConnectAck(dataPtr))
				{
				case 0:
					printf("Tips:连接成功\r\n");
					status = 0;
					break;
				case 1:
					printf("WARN:连接失败：协议错误\r\n");
					break;
				case 2:
					printf("WARN:连接失败：非法的 clientid\r\n");
					break;
				case 3:
					printf("WARN:连接失败：服务器失败\r\n");
					break;
				case 4:
					printf("WARN:连接失败：用户名或密码错误\r\n");
					break;
				case 5:
					printf("WARN:连接失败：非法链接(比如 token 非法)\r\n");
					break;
				default:
					printf("ERR:连接失败：未知错误\r\n");
					break;
				}
			}
		}
		else
		{
			printf("ERR:请求失败：getIPD 失败\r\n");
		}
		// 退出保护
		//giveUSART2();
		
		MQTT_DeleteBuffer(&mqttPacket); // 删包
	}
	else
	{
		printf("WARN: MQTT_PacketConnect Failed\r\n");
	}
	return status;
}

/*******************************************************************************
 * 函 数 名			: mallocJson
 * 函数功能			: 在 buf 中写入需要发送的 json 字串
 * 注意事项			: 别忘了自己清空内存
 *******************************************************************************/
uint8_t mallocJson(char **buf) {
	
	char* text = malloc(MALLOC_SIZE * sizeof(char));	
	memset(text, 0, (unsigned)MALLOC_SIZE);	
	
	strcpy(text, "{");
	
	double t = 32.5, h = 55.6;

	sprintf(text, "\"temp\":%f,\"humi\":%f", t, h);


	strcat(text, "}");
	
	*buf = text;
	
	printf("CJson ready\r\n");
	
	return strlen(text);
}

/*******************************************************************************
 * 函 数 名			: MQTT_Transmit
 * 函数功能			: 封装 MQTT 数据包并上传 OneNet
 * 注意事项			: 用完了记得关上
 *******************************************************************************/
void MQTT_Transmit(char *json)
{
	printf("MQTT transmit start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包
	short body_len = 0, i = 0;
	
	
	if (body_len)
	{
		if (MQTT_PacketSaveData(MQTT_userInfo.deviceID, body_len, NULL, 3, &mqttPacket) == 0)
		{
			for (; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = json[i];

			MQTT_update(mqttPacket._data, mqttPacket._len); // 上传数据到平台
			printf("Tips:	Send %d bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket); // 删包
		}
		else
		{
			printf("WARN:	MQTT_NewBuffer Failed\r\n");
		}
	}
	// 发完就回收内存
	free(json);
}
/*******************************************************************************
 * 函 数 名			: MQTT_ReceiveDeal
 * 函数功能			: 处理接收到的 MQTT 数据
 * 注意事项			: 无
 * p.s.
 * 	LED1 点亮命令：“led1:1”，熄灭命令：“led1:0”
 * 	LED2 点亮命令：“led2:1”，熄灭命令：“led2:0”
 *******************************************************************************/
void MQTT_ReceiveDeal(uint8_t *cmd)
{
	printf("MQTT deal receive start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short req_len = 0;
	unsigned char type = 0;
	short result = 0;
	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;

	type = MQTT_UnPacketRecv(cmd);
	switch (type)
	{
	case MQTT_PKT_CMD: // 命令下发
		result = MQTT_UnPacketCmd(cmd, &cmdid_topic,
								  &req_payload, &req_len); // 解出topic和消息体
		if (result == 0)
		{
			printf("cmdid: %s, req: %s, req_len: %d\r\n",
				   cmdid_topic, req_payload, req_len);
			if (MQTT_PacketCmdResp(cmdid_topic,
								   req_payload, &mqttPacket) == 0)
			{ // 命令回复组包
				printf("Tips:	Send CmdResp\r\n");
				MQTT_update(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}
		break;

	case MQTT_PKT_PUBACK: // 发送Publish消息，平台回复的Ack
		if (MQTT_UnPacketPublishAck(cmd) == 0)
			printf("Tips:	MQTT Publish Send OK\r\n");
		break;

	default:
		result = -1;
		break;
	}

	BUF_clear(); // 清空缓存
	if (result == -1)
		return;

	dataPtr = strchr(req_payload, ':'); // 搜索':'

	if (dataPtr != NULL && result != -1)
	{
		dataPtr++;
		// 判断是否是下发的命令控制数据
		while (*dataPtr >= '0' && *dataPtr <= '9')
			numBuf[num++] = *dataPtr++;
		num = atoi((const char *)numBuf); // 转为数值形式
		
		if (strstr((char *)req_payload, "led1"))
		{
			if (num == 1) // 控制数据如果为1，代表开
				LED1 = 1;
			else if (num == 0) // 控制数据如果为0，代表关
				LED1 = 0;
		}
		else if (strstr((char *)req_payload, "led2"))
		{
			if (num == 1)
				LED2 = 1;
			else if (num == 0)
				LED2 = 0;
		}
	}
	
	// 退出保护
	//giveUSART2();

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

/*******************************************************************************
 * 函 数 名			: MQTT_Disconnect
 * 函数功能			: MQTT 断开连接
 * 注意事项			: 无
 *******************************************************************************/
uint1 MQTT_Disconnect(void)
{
	printf("MQTT desconnect start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包
	
	if(MQTT_PacketDisConnect(&mqttPacket) == 0)
	{
		MQTT_update(mqttPacket._data, mqttPacket._len);
		
		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

/*******************************************************************************
 * 函 数 名			: MQTT_Reset
 * 函数功能			: MQTT 重置连接
 * 注意事项			: 无
 *******************************************************************************/
void MQTT_Reset(void)
{
	printf("MQTT reset start\r\n");
	
	MQTT_Disconnect();
	
	delay(500);
	
	uint8_t count = 0;
	while (MQTT_ConnectInit()){
		if(count > 4){
			printf("OneNET connect failed\r\n");
			break;
		}
		printf("Try to connect OneNET\r\n");
		count++;
		delay(500);
	}
}

/*******************************************************************************
 * 函 数 名			: MQTT_DetectHeartBeat
 * 函数功能			: MQTT 获取心跳
 * 注意事项			: 无
 *******************************************************************************/
uint8_t MQTT_DetectHeartBeat(void)
{
	printf("MQTT detect heart beats start\r\n");
	uint8_t flag = 1;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	uint8_t *dataPtr = malloc( MALLOC_SIZE * sizeof(char) );

	unsigned char sCount = 3;
	
	if(MQTT_PacketPing(&mqttPacket))
		return flag;
	
	while(sCount--)
	{
		MQTT_update(mqttPacket._data, mqttPacket._len);
		
		dataPtr = getIPD(250);
		
		if (dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP)
			{
				printf("Tips:	HeartBeat OK\r\n");
				free(dataPtr);

				//giveUSART2();
				flag = 0;
				break;
			}
			else
			{
				printf("Check Device\r\n");
			}
		}
		else
		{
			printf("ERR:请求失败：getIPD 失败\r\n");
		}
		
		free(dataPtr);
		
		delay(2);
	}
	
	// 退出保护
	//giveUSART2();
	
	MQTT_DeleteBuffer(&mqttPacket);
	return flag;

}

/*******************************************************************************
 * 函 数 名			: MQTT_Test
 * 函数功能			: MQTT 测试代码
 * 注意事项			: 无
 *******************************************************************************/
void MQTT_Test(void)
{
	uint16_t timeCount = 0;
	uint8_t *dataPtr = NULL;

	uint8_t count = 0;
	// 连接 OneNet——连接不成功则不跳出
	while (MQTT_ConnectInit()){
		if(count > 4){
			printf("OneNET connect failed\r\n");
			break;
		}
		printf("Try to connect OneNET\r\n");
		count++;
		delay(500);
	}
	
	while (1)
	{
		if(MQTT_DetectHeartBeat())
			MQTT_Reset();
			
		// 发送间隔 5s
		if (++timeCount >= 500)
		{
			char *buf;
			mallocJson(&buf);
			
			printf("already to send data\r\n");
			MQTT_Transmit(buf); // 发送数据
			timeCount = 0;
			BUF_clear();
		}
		
		// 只接收一次
		dataPtr = getIPD(0);
		
		if (dataPtr != NULL)
			MQTT_ReceiveDeal(dataPtr);
		delay(10);
	}
}