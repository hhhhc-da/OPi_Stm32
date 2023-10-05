#ifndef EVENT_H
#define EVENT_H

//�¼����ƣ���Ҫ�Լ����غ���ָ�������
//��ִ����һ��whileѭ��������۵��¼�

#include <stm32f10x.h>
#include <stdlib.h>

//�����¼�
#define EventMode 				1
//�¼��������ֵ
#define MAX_EVENT_NUM 			32

#define ENABLE 					1
#define DISABLE 				0

//�ص���������
typedef void (*Event)(void);
typedef void (*Event_pD)(void*);

//�¼�������
typedef enum {
	TIMER_EVENT = 0x00, 		//��ʱ��
	USART1_RX_EVENT, 			//USART1
	USART2_RX_EVENT
} EVENT_TYPE;

typedef struct
{
	//״̬ʱ��
	uint8_t EventStatus;
	uint8_t DataStatus;
	//����ָ��
	void *Data;
	//����������
	EVENT_TYPE EventType;
	//�ص�ָ��
	Event CB;
	Event_pD CBD;
} EventTable;

void EventLoop(void);
void EventCreate(EVENT_TYPE EventType, Event cb);
void EventCreate_plusData(EVENT_TYPE EventType, Event_pD cb, void *Data);

extern EventTable EventBuffer[MAX_EVENT_NUM];

#endif
