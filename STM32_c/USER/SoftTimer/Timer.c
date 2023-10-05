#include "Timer.h"

Timer *Head = 0;

const unsigned int ALL = 0;
/*******************************************************************************
 * 函 数 名			: TimerInit(Timer *t,uint16_t time,Event Func)
 * 函数功能			: 构造一个新的定时器
 * 注意事项			: 记得存储好
 *******************************************************************************/
Timer *TimerInit(uint16_t time,TimerEvent Func)
{
    Head = (Timer *)malloc(sizeof(Timer));

    Head->Step = time * STEP_LEN;
	Head->Now = time * STEP_LEN;
	Head->CB = Func;
	Head->TimerCmd = DISABLE;
	Head->pNext = NULL;

    return Head;
}

/*******************************************************************************
 * 函 数 名			: TimerEnlarge(Timer *pHead,uint16_t time,Event Func)
 * 函数功能			: 往老链表里挂新的
 * 注意事项			: 记得存储好
 *******************************************************************************/
Timer *TimerEnlarge(Timer *pHead,uint16_t time,TimerEvent Func)
{
    Timer *pEnd = pHead, *pNew = pHead->pNext;

    while (pNew)
    {
        pEnd = pNew;
        pNew = pNew->pNext;
    }
    // pNew已经到需要构造的位置了

    pNew = (Timer *)malloc(sizeof(Timer));
    
	pHead->Step = time * STEP_LEN;
	pHead->Now = time * STEP_LEN;
	pHead->CB = Func;
	pHead->TimerCmd = DISABLE;

    // 挂在上一个的尾巴上
    pEnd->pNext = pNew;
    // pEnd向后推一个
    pEnd = pEnd->pNext;
    // pEnd->pNext置空
    pEnd->pNext = NULL;

    return pHead;
}

/*******************************************************************************
 * 函 数 名			: TimerDelete(Timer *pHead, unsigned id)
 * 函数功能		    : 删除链表某一节点的数据
 * 注意事项			: 必须保证在范围内，已经设置了abort
 * p.s.
 * 你可以试试 Delete(pHead,ALL) 删除全部数据
 *******************************************************************************/
Timer *TimerDelete(Timer *pHead, unsigned id)
{
    Timer *pAim = pHead;

    if (id == 1)
    {
        pAim = pHead;
        pHead = pAim->pNext;
        pAim->pNext = NULL;
        free(pAim);
    }
    else
    {
        Timer *pTemp = pAim->pNext;
        if (id == ALL)
        {
            while (pTemp)
            {
                pAim->pNext = NULL;
                free(pAim);
                pAim = pTemp;
                pTemp = pTemp->pNext;
            }
            free(pAim);
        }
        else
        {
            // 从节点2开始计数
            unsigned lcount = 2;
            while (lcount != id)
            {
                pAim = pTemp;
                pTemp = pTemp->pNext;
                if (!pAim)
                    abort();
                ++lcount;
            }
            // 已经到目标位置，pAim是上一个，pTemp是要删除的
            if (pTemp->pNext == NULL)
            {
                pAim->pNext = NULL;
                free(pTemp);
            }
            else
            {
                pAim->pNext = pTemp->pNext;
                free(pTemp);
            }
        }
    }
    return pHead;
}

/*******************************************************************************
 * 函 数 名			: TimerBegin(Timer *t)
 * 函数功能			: 开启定时器
 * 注意事项			: 无
 *******************************************************************************/
void TimerBegin(Timer *pHead){
	pHead->TimerCmd = ENABLE;
}

/*******************************************************************************
 * 函 数 名			: TimerStop(Timer *t)
 * 函数功能			: 关闭定时器
 * 注意事项			: 无
 *******************************************************************************/
void TimerStop(Timer *pHead){
	pHead->TimerCmd = DISABLE;
}

/*******************************************************************************
 * 函 数 名			: TimerTask(void)
 * 函数功能			: 执行回调任务
 * 注意事项			: 参数自己调
 *******************************************************************************/
void TimerTask(void){
	if(Head){
		Timer *Temp = Head;
		while(Temp){
			if(Temp->TimerCmd == ENABLE){
				if(--(Temp->Now)<=0){
					Temp->Now = Temp->Step;
					Temp->CB(Temp->arg);
				}
			}
			Temp = Temp->pNext;
		}
	}
}
