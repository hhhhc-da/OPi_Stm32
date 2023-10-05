#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f10x.h>

#define ENABLE 1
#define DISABLE 0

//步长，定时时间
#define STEP_LEN 72

typedef void (*TimerEvent)(void*);

typedef struct timer
{
	//定时器使能时钟
	uint8_t TimerCmd;
	//定时周期
	uint16_t Step;
	//当前时间
	uint16_t Now;
	//回调参数
	void *arg;
	//回调指针
	TimerEvent CB;
	//定时器next
	struct timer *pNext;
} Timer;

Timer *TimerInit(uint16_t time,TimerEvent Func);
Timer *TimerEnlarge(Timer *pHead,uint16_t time,TimerEvent Func);
Timer *TimerDelete(Timer *pHead, unsigned id);
void TimerBegin(Timer *pHead);
void TimerStop(Timer *pHead);
void TimerTask(void);

// 导出全局量 ALL,删除的时候用,不然会一直占用动态内存
extern const unsigned int ALL;
extern Timer *Head;

#endif