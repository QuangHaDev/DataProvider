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

    BOOL Create();
	void Destroy();
	void AddQueueItem(T &item);
	QUEUECODE GetQueueItem(T &item);

    BOOL m_bQuit;

protected:
	QUEUECODE Wait();
	QUEUECODE GetItem(T &item);

private:
    HANDLE m_hEvent;
    CRITICAL_SECTION m_cs;
};

template <class T> CQueue<T>::CQueue()
{
    m_hEvent = NULL;
    m_bQuit = FALSE;
}

template <class T> CQueue<T>::~CQueue()
{
    if (m_hEvent != NULL)
    {
        SetEvent(m_hEvent);
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
        DeleteCriticalSection(&m_cs);
    }
}

template <class T> BOOL CQueue<T>::Create()
{
    if (m_hEvent == NULL)
    {
        m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        InitializeCriticalSection(&m_cs);
    }
    else
    {
        m_bQuit = FALSE;
        ResetEvent(m_hEvent);
    }

    return (m_hEvent != NULL);
}

template <class T> void CQueue<T>::Destroy()
{
    m_bQuit = TRUE;
    SetEvent(m_hEvent);
}

template <class T> QUEUECODE CQueue<T>::Wait()
{
    QUEUECODE qrc = QUEUE_EMPTY;

    WaitForSingleObject(m_hEvent, INFINITE);
    ResetEvent(m_hEvent);
    if (m_bQuit)
        qrc = QUEUE_QUIT;

    return qrc;
}

template <class T> void CQueue<T>::AddQueueItem(T &item)
{
    EnterCriticalSection(&m_cs);
    AddTail(item);
    SetEvent(m_hEvent);
    LeaveCriticalSection(&m_cs);
}

template <class T> QUEUECODE CQueue<T>::GetQueueItem(T &item)
{
    QUEUECODE qrc;

    qrc = GetItem(item);
    if (qrc == QUEUE_EMPTY)
    {
        if (Wait() == QUEUE_EMPTY)
            qrc = GetItem(item);
        else
            qrc = QUEUE_QUIT;
    }

    return qrc;
}

template <class T> QUEUECODE CQueue<T>::GetItem(T &item)
{
    QUEUECODE qrc;

    EnterCriticalSection(&m_cs);
    if (!IsEmpty())
    {
        item = RemoveHead();
        qrc = QUEUE_SUCCESS;
    }
    else
        qrc = QUEUE_EMPTY;
    LeaveCriticalSection(&m_cs);

    if (m_bQuit)
        qrc = QUEUE_QUIT;

    return qrc;
}

#endif // !defined(AFX_QUEUE_H__B54DD1F3_1ED0_11D3_92FC_00A0C99B2780__INCLUDED_)
