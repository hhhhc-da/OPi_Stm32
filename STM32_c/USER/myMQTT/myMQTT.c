
#include "myMQTT.h"

#define delay			Delay_ms
#define MQTT_update		ATSendDataLen

#define BUF				bufRX2.buf
#define BUF_pos			bufRX2.index
#define BUF_clear		ClearRXBuf

// ��λ���� LED
#define LED1			PAout(3)
#define LED2			PAout(2)

// ����ȫ�ֱ���
MQTT_BaseType MQTT_userInfo = {
	(int8*)"578642",
	(int8*)"1055010844",
	(int8*)"fwsrtcTO1=EJ38ScOeW886RHBZI=",
	0
};

/*******************************************************************************
 * �� �� ��			: getIPD
 * ��������			: ��ȡ�յ�����Ϣ
 * ע������			: ��
 *******************************************************************************/
uint8_t* getIPD(uint16_t timeout)
{
	printf("MQTT get IPD start\r\n");
	
	char *pIPD = 0;
	uint8_t pos = BUF_pos;
	do {
		delay(1000);
		// ���ָ�� 1s û�������Ǿ���Ϊ����������
		if(pos == BUF_pos){
			
			// �����ٽ���Դ
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
	// �����˳�
	//giveUSART2();
	return NULL;
}

/*******************************************************************************
 * �� �� ��			: MQTT_ConnectInit
 * ��������			: ���� MQTT4
 * ע������			: ��
 *******************************************************************************/
uint8_t MQTT_ConnectInit(void)
{
	printf("MQTT connect initialization start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���
	uint8_t *dataPtr;
	uint8_t status = 1;
	
	BUF_clear();
	
	// ������������
	if (MQTT_PacketConnect(MQTT_userInfo.productID, MQTT_userInfo.API_KEY, MQTT_userInfo.deviceID, 300, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		MQTT_update(mqttPacket._data, mqttPacket._len); // �ϴ�ƽ̨
			
		dataPtr = getIPD(250);							// �ȴ�ƽ̨��Ӧ
		
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
			switch (MQTT_UnPacketConnectAck(dataPtr))
				{
				case 0:
					printf("Tips:���ӳɹ�\r\n");
					status = 0;
					break;
				case 1:
					printf("WARN:����ʧ�ܣ�Э�����\r\n");
					break;
				case 2:
					printf("WARN:����ʧ�ܣ��Ƿ��� clientid\r\n");
					break;
				case 3:
					printf("WARN:����ʧ�ܣ�������ʧ��\r\n");
					break;
				case 4:
					printf("WARN:����ʧ�ܣ��û������������\r\n");
					break;
				case 5:
					printf("WARN:����ʧ�ܣ��Ƿ�����(���� token �Ƿ�)\r\n");
					break;
				default:
					printf("ERR:����ʧ�ܣ�δ֪����\r\n");
					break;
				}
			}
		}
		else
		{
			printf("ERR:����ʧ�ܣ�getIPD ʧ��\r\n");
		}
		// �˳�����
		//giveUSART2();
		
		MQTT_DeleteBuffer(&mqttPacket); // ɾ��
	}
	else
	{
		printf("WARN: MQTT_PacketConnect Failed\r\n");
	}
	return status;
}

/*******************************************************************************
 * �� �� ��			: mallocJson
 * ��������			: �� buf ��д����Ҫ���͵� json �ִ�
 * ע������			: �������Լ�����ڴ�
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
 * �� �� ��			: MQTT_Transmit
 * ��������			: ��װ MQTT ���ݰ����ϴ� OneNet
 * ע������			: �����˼ǵù���
 *******************************************************************************/
void MQTT_Transmit(char *json)
{
	printf("MQTT transmit start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���
	short body_len = 0, i = 0;
	
	
	if (body_len)
	{
		if (MQTT_PacketSaveData(MQTT_userInfo.deviceID, body_len, NULL, 3, &mqttPacket) == 0)
		{
			for (; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = json[i];

			MQTT_update(mqttPacket._data, mqttPacket._len); // �ϴ����ݵ�ƽ̨
			printf("Tips:	Send %d bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket); // ɾ��
		}
		else
		{
			printf("WARN:	MQTT_NewBuffer Failed\r\n");
		}
	}
	// ����ͻ����ڴ�
	free(json);
}
/*******************************************************************************
 * �� �� ��			: MQTT_ReceiveDeal
 * ��������			: ������յ��� MQTT ����
 * ע������			: ��
 * p.s.
 * 	LED1 ���������led1:1����Ϩ�������led1:0��
 * 	LED2 ���������led2:1����Ϩ�������led2:0��
 *******************************************************************************/
void MQTT_ReceiveDeal(uint8_t *cmd)
{
	printf("MQTT deal receive start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���
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
	case MQTT_PKT_CMD: // �����·�
		result = MQTT_UnPacketCmd(cmd, &cmdid_topic,
								  &req_payload, &req_len); // ���topic����Ϣ��
		if (result == 0)
		{
			printf("cmdid: %s, req: %s, req_len: %d\r\n",
				   cmdid_topic, req_payload, req_len);
			if (MQTT_PacketCmdResp(cmdid_topic,
								   req_payload, &mqttPacket) == 0)
			{ // ����ظ����
				printf("Tips:	Send CmdResp\r\n");
				MQTT_update(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}
		break;

	case MQTT_PKT_PUBACK: // ����Publish��Ϣ��ƽ̨�ظ���Ack
		if (MQTT_UnPacketPublishAck(cmd) == 0)
			printf("Tips:	MQTT Publish Send OK\r\n");
		break;

	default:
		result = -1;
		break;
	}

	BUF_clear(); // ��ջ���
	if (result == -1)
		return;

	dataPtr = strchr(req_payload, ':'); // ����':'

	if (dataPtr != NULL && result != -1)
	{
		dataPtr++;
		// �ж��Ƿ����·��������������
		while (*dataPtr >= '0' && *dataPtr <= '9')
			numBuf[num++] = *dataPtr++;
		num = atoi((const char *)numBuf); // תΪ��ֵ��ʽ
		
		if (strstr((char *)req_payload, "led1"))
		{
			if (num == 1) // �����������Ϊ1������
				LED1 = 1;
			else if (num == 0) // �����������Ϊ0�������
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
	
	// �˳�����
	//giveUSART2();

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

/*******************************************************************************
 * �� �� ��			: MQTT_Disconnect
 * ��������			: MQTT �Ͽ�����
 * ע������			: ��
 *******************************************************************************/
uint1 MQTT_Disconnect(void)
{
	printf("MQTT desconnect start\r\n");
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���
	
	if(MQTT_PacketDisConnect(&mqttPacket) == 0)
	{
		MQTT_update(mqttPacket._data, mqttPacket._len);
		
		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

/*******************************************************************************
 * �� �� ��			: MQTT_Reset
 * ��������			: MQTT ��������
 * ע������			: ��
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
 * �� �� ��			: MQTT_DetectHeartBeat
 * ��������			: MQTT ��ȡ����
 * ע������			: ��
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
			printf("ERR:����ʧ�ܣ�getIPD ʧ��\r\n");
		}
		
		free(dataPtr);
		
		delay(2);
	}
	
	// �˳�����
	//giveUSART2();
	
	MQTT_DeleteBuffer(&mqttPacket);
	return flag;

}

/*******************************************************************************
 * �� �� ��			: MQTT_Test
 * ��������			: MQTT ���Դ���
 * ע������			: ��
 *******************************************************************************/
void MQTT_Test(void)
{
	uint16_t timeCount = 0;
	uint8_t *dataPtr = NULL;

	uint8_t count = 0;
	// ���� OneNet�������Ӳ��ɹ�������
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
			
		// ���ͼ�� 5s
		if (++timeCount >= 500)
		{
			char *buf;
			mallocJson(&buf);
			
			printf("already to send data\r\n");
			MQTT_Transmit(buf); // ��������
			timeCount = 0;
			BUF_clear();
		}
		
		// ֻ����һ��
		dataPtr = getIPD(0);
		
		if (dataPtr != NULL)
			MQTT_ReceiveDeal(dataPtr);
		delay(10);
	}
}