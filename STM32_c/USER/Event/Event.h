#ifndef EVENT_H
#define EVENT_H

//事件机制，需要自己挂载函数指针和数据
//在执行完一遍while循环后处理积累的事件

#include <stm32f10x.h>
#include <stdlib.h>

//开启事件
#define EventMode 				1
//事件处理最大值
#define MAX_EVENT_NUM 			32

#define ENABLE 					1
#define DISABLE 				0

//回调函数类型
typedef void (*Event)(void);
typedef void (*Event_pD)(void*);

//事件子类型
typedef enum {
	TIMER_EVENT = 0x00, 		//定时器
	USART1_RX_EVENT, 			//USART1
	USART2_RX_EVENT
} EVENT_TYPE;

typedef struct
{
	//状态时钟
	uint8_t EventStatus;
	uint8_t DataStatus;
	//数据指针
	void *Data;
	//数据子类型
	EVENT_TYPE EventType;
	//回调指针
	Event CB;
	Event_pD CBD;
} EventTable;

void EventLoop(void);
void EventCreate(EVENT_TYPE EventType, Event cb);
void EventCreate_plusData(EVENT_TYPE EventType, Event_pD cb, void *Data);

extern EventTable EventBuffer[MAX_EVENT_NUM];

#endif
