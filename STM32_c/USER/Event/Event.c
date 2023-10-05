#include "Event.h"

#if MAX_EVENT_NUM>0
//事件队列
EventTable EventBuffer[MAX_EVENT_NUM];

/*******************************************************************************
* 函 数 名			: EventLoop
* 函数功能			: 处理待处理事件
* 注意事项			: 自己插入事件和回调函数
* p.s.
* 有空再写一个函数，给他封好直接插进来
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
* 函 数 名			: EventCreate_plusData
* 函数功能			: 给数据封好直接插进来
* 注意事项			: 无数据版本
*******************************************************************************/
void EventCreate(EVENT_TYPE EventType, Event cb){
	EventBuffer[EventType].CB = cb;
	EventBuffer[EventType].Data = NULL;
	EventBuffer[EventType].EventStatus = ENABLE;
	EventBuffer[EventType].EventType = EventType;
	EventBuffer[EventType].DataStatus = DISABLE;
}

/*******************************************************************************
* 函 数 名			: EventCreate_plusData
* 函数功能			: 给数据封好直接插进来
* 注意事项			: 有数据版本
*******************************************************************************/
void EventCreate_plusData(EVENT_TYPE EventType, Event_pD cb, void *Data){
	EventBuffer[EventType].CBD = cb;
	EventBuffer[EventType].Data = Data;
	EventBuffer[EventType].EventStatus = ENABLE;
	EventBuffer[EventType].EventType = EventType;
	EventBuffer[EventType].DataStatus = ENABLE;
}

#endif
