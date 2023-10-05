#include "Event.h"

#if MAX_EVENT_NUM>0
//�¼�����
EventTable EventBuffer[MAX_EVENT_NUM];

/*******************************************************************************
* �� �� ��			: EventLoop
* ��������			: ����������¼�
* ע������			: �Լ������¼��ͻص�����
* p.s.
* �п���дһ���������������ֱ�Ӳ����
*******************************************************************************/
void EventLoop(void){
	int i = 0;
	for(;i<MAX_EVENT_NUM;++i){
		if(EventBuffer[i].EventStatus == ENABLE){
			if(EventBuffer[i].DataStatus == DISABLE)
				EventBuffer[i].CB();
			else if(EventBuffer[i].DataStatus == DISABLE)
				EventBuffer[i].CBD(EventBuffer[i].Data);
			
			EventBuffer[i].EventStatus = DISABLE;
		}
	}
}

/*******************************************************************************
* �� �� ��			: EventCreate_plusData
* ��������			: �����ݷ��ֱ�Ӳ����
* ע������			: �����ݰ汾
*******************************************************************************/
void EventCreate(EVENT_TYPE EventType, Event cb){
	EventBuffer[EventType].CB = cb;
	EventBuffer[EventType].Data = NULL;
	EventBuffer[EventType].EventStatus = ENABLE;
	EventBuffer[EventType].EventType = EventType;
	EventBuffer[EventType].DataStatus = DISABLE;
}

/*******************************************************************************
* �� �� ��			: EventCreate_plusData
* ��������			: �����ݷ��ֱ�Ӳ����
* ע������			: �����ݰ汾
*******************************************************************************/
void EventCreate_plusData(EVENT_TYPE EventType, Event_pD cb, void *Data){
	EventBuffer[EventType].CBD = cb;
	EventBuffer[EventType].Data = Data;
	EventBuffer[EventType].EventStatus = ENABLE;
	EventBuffer[EventType].EventType = EventType;
	EventBuffer[EventType].DataStatus = ENABLE;
}

#endif
