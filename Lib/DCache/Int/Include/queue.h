//////////////////////////////////////////////////////////////////////
//  File        :  Queue.h
//  Author      :  John Lee
//  Description :  FIFO Queue with output wait
//  Date        :  3 April 2001
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__B54DD1F3_1ED0_11D3_92FC_00A0C99B2780__INCLUDED_)
#define AFX_QUEUE_H__B54DD1F3_1ED0_11D3_92FC_00A0C99B2780__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

enum QUEUECODE {QUEUE_QUIT, QUEUE_EMPTY, QUEUE_SUCCESS};
#include <afxtempl.h>

// FIFO Queue
template <class T>
class CQueue : public CList<T, T&>
{
public:
	CQueue();
	virtual ~CQueue(); 

	void AddQueueItem(T &item);
	void AddQueueItemHead(T &item);

	QUEUECODE GetQueueItem(T &item);
	QUEUECODE DirectGetQueueItem(T &item);
	long GetCount();

protected:

	QUEUECODE GetItem(T &item);

private:
	long icount;
   
    CRITICAL_SECTION m_csQueue;
};

template <class T> CQueue<T>::CQueue()
{
     InitializeCriticalSection(&m_csQueue);
	 icount = 0;
}
template <class T> long CQueue<T>::GetCount()
{
	 return icount;
}

template <class T> CQueue<T>::~CQueue()
{
	DeleteCriticalSection(&m_csQueue);
}



template <class T> void CQueue<T>::AddQueueItem(T &item)
{
    EnterCriticalSection(&m_csQueue);
    if (AddTail(item))
		InterlockedIncrement(&icount);
		 //icount ++;
    LeaveCriticalSection(&m_csQueue);
}

template <class T> void CQueue<T>::AddQueueItemHead(T &item)
{
    EnterCriticalSection(&m_csQueue);
    if (AddHead(item))
		InterlockedIncrement(&icount);
		 //icount ++;
    LeaveCriticalSection(&m_csQueue);

}
template <class T> QUEUECODE CQueue<T>::GetQueueItem(T &item)
{
   	QUEUECODE qrc;

    EnterCriticalSection(&m_csQueue);
    //if (!IsEmpty())
	if (icount > 0)
    {
		InterlockedDecrement(&icount);
        item = RemoveHead();		
		//icount --;
        qrc = QUEUE_SUCCESS;
    }
    else
        qrc = QUEUE_EMPTY;
    LeaveCriticalSection(&m_csQueue);
	return qrc;

    
}

template <class T> QUEUECODE CQueue<T>::DirectGetQueueItem(T &item)
{
	QUEUECODE qrc;

    EnterCriticalSection(&m_csQueue);
//if (!IsEmpty())
	if (icount > 0)
    {
		InterlockedDecrement(&icount);
        item = RemoveHead();		
		//icount --;
        qrc = QUEUE_SUCCESS;
    }
    else
        qrc = QUEUE_EMPTY;
    
    LeaveCriticalSection(&m_csQueue);
	return qrc;
}

template <class T> QUEUECODE CQueue<T>::GetItem(T &item)
{
    QUEUECODE qrc;

    qrc = DirectGetQueueItem(item);

    if (m_bQuit)
        qrc = QUEUE_QUIT;

    return qrc;
}

#endif // !defined(AFX_QUEUE_H__B54DD1F3_1ED0_11D3_92FC_00A0C99B2780__INCLUDED_)
