#include "Timer.h"

Timer *Head = 0;

const unsigned int ALL = 0;
/*******************************************************************************
 * �� �� ��			: TimerInit(Timer *t,uint16_t time,Event Func)
 * ��������			: ����һ���µĶ�ʱ��
 * ע������			: �ǵô洢��
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
 * �� �� ��			: TimerEnlarge(Timer *pHead,uint16_t time,Event Func)
 * ��������			: ������������µ�
 * ע������			: �ǵô洢��
 *******************************************************************************/
Timer *TimerEnlarge(Timer *pHead,uint16_t time,TimerEvent Func)
{
    Timer *pEnd = pHead, *pNew = pHead->pNext;

    while (pNew)
    {
        pEnd = pNew;
        pNew = pNew->pNext;
    }
    // pNew�Ѿ�����Ҫ�����λ����

    pNew = (Timer *)malloc(sizeof(Timer));
    
	pHead->Step = time * STEP_LEN;
	pHead->Now = time * STEP_LEN;
	pHead->CB = Func;
	pHead->TimerCmd = DISABLE;

    // ������һ����β����
    pEnd->pNext = pNew;
    // pEnd�����һ��
    pEnd = pEnd->pNext;
    // pEnd->pNext�ÿ�
    pEnd->pNext = NULL;

    return pHead;
}

/*******************************************************************************
 * �� �� ��			: TimerDelete(Timer *pHead, unsigned id)
 * ��������		    : ɾ������ĳһ�ڵ������
 * ע������			: ���뱣֤�ڷ�Χ�ڣ��Ѿ�������abort
 * p.s.
 * ��������� Delete(pHead,ALL) ɾ��ȫ������
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
            // �ӽڵ�2��ʼ����
            unsigned lcount = 2;
            while (lcount != id)
            {
                pAim = pTemp;
                pTemp = pTemp->pNext;
                if (!pAim)
                    abort();
                ++lcount;
            }
            // �Ѿ���Ŀ��λ�ã�pAim����һ����pTemp��Ҫɾ����
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
 * �� �� ��			: TimerBegin(Timer *t)
 * ��������			: ������ʱ��
 * ע������			: ��
 *******************************************************************************/
void TimerBegin(Timer *pHead){
	pHead->TimerCmd = ENABLE;
}

/*******************************************************************************
 * �� �� ��			: TimerStop(Timer *t)
 * ��������			: �رն�ʱ��
 * ע������			: ��
 *******************************************************************************/
void TimerStop(Timer *pHead){
	pHead->TimerCmd = DISABLE;
}

/*******************************************************************************
 * �� �� ��			: TimerTask(void)
 * ��������			: ִ�лص�����
 * ע������			: �����Լ���
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
