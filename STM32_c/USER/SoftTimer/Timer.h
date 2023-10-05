#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f10x.h>

#define ENABLE 1
#define DISABLE 0

//��������ʱʱ��
#define STEP_LEN 72

typedef void (*TimerEvent)(void*);

typedef struct timer
{
	//��ʱ��ʹ��ʱ��
	uint8_t TimerCmd;
	//��ʱ����
	uint16_t Step;
	//��ǰʱ��
	uint16_t Now;
	//�ص�����
	void *arg;
	//�ص�ָ��
	TimerEvent CB;
	//��ʱ��next
	struct timer *pNext;
} Timer;

Timer *TimerInit(uint16_t time,TimerEvent Func);
Timer *TimerEnlarge(Timer *pHead,uint16_t time,TimerEvent Func);
Timer *TimerDelete(Timer *pHead, unsigned id);
void TimerBegin(Timer *pHead);
void TimerStop(Timer *pHead);
void TimerTask(void);

// ����ȫ���� ALL,ɾ����ʱ����,��Ȼ��һֱռ�ö�̬�ڴ�
extern const unsigned int ALL;
extern Timer *Head;

#endif