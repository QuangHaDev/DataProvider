//////////////////////////////////////////////////////////////////////
//  File        :  QueueMgr.h
//  Author      :  John Lee
//  Description :  Queue Manager
//  Date        :  3 April 2001
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUEMGR_H__7197F492_3924_11D5_9320_00025598E5A1__INCLUDED_)
#define AFX_QUEUEMGR_H__7197F492_3924_11D5_9320_00025598E5A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <process.h>
#include "queue.h"

template <class T>
class CQueueManager  
{
public:
    CQueueManager();
    virtual ~CQueueManager();

    void ProcessThread();
    static UINT APIENTRY ThreadFunc(void* lpParam);
    virtual void Main(T& item) = 0;
    void Add(T& item);

protected:
    CQueue<T> m_queue;

    HANDLE m_hExit;
    UINT m_nThreadID;
    HANDLE m_hThread;
};


template <class T>
CQueueManager<T>::CQueueManager()
{
    m_queue.Create();
    m_hExit = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hThread = (HANDLE)_beginthreadex(NULL,1,ThreadFunc,this,1, &m_nThreadID);
}

template <class T>
CQueueManager<T>::~CQueueManager()
{
    m_queue.Destroy();
    SetEvent(m_hExit);
    WaitForSingleObject(m_hThread, INFINITE);
}

template <class T>
void CQueueManager<T>::Add(T& item)
{
    m_queue.AddQueueItem(item);
}

template <class T>
UINT CQueueManager<T>::ThreadFunc(void* lpParam)
{
    ((CQueueManager*)lpParam)->ProcessThread();
    return 0;
}

template <class T>
void CQueueManager<T>::ProcessThread()
{
    short sRc;
    T item;
    while(WaitForSingleObject(m_hExit, 0) != WAIT_OBJECT_0)
    {
        sRc = m_queue.GetQueueItem(item);
        switch(sRc)
        {
        case QUEUE_SUCCESS:
            Main(item);
        case QUEUE_EMPTY:
            break;
        case QUEUE_QUIT:
			while (!m_queue.IsEmpty())
				delete m_queue.RemoveHead();
           return;
        }
    }
}
#endif // !defined(AFX_QUEUEMGR_H__7197F492_3924_11D5_9320_00025598E5A1__INCLUDED_)
