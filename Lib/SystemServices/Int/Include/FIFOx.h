#pragma once
#include <deque>
#include "JCriticalSectionEx.h"
#include "ssEvent.h"
#include <memory>

template <class T>
class CFIFOx
{
public:
	CFIFOx();
	~CFIFOx();

public:
	bool GetItem(T& item);
	void AddItem(T item);

	bool WaitForData(T& item, int mSec = 1);
	bool IsEmpty();
	unsigned long GetSize();
	ssEvent& GetAddedEvent()	{ return m_ssAdded; };
	CONDITION_VARIABLE & GetAddedCV() { return m_cvAdded; };
	void EnableCV(){ m_bUseCV = true; WakeConditionVariable(&m_cvAdded); };
	void Clear();
protected:
	//CJCriticalSectionEx m_jcs1;
	CRITICAL_SECTION cs1;
	CJCriticalSectionEx m_jcs2;

	bool m_bUseCV;
	ssEvent m_ssAdded;
	CONDITION_VARIABLE m_cvAdded;

	std::deque<T> m_lstData[2];
	std::deque<T> * m_plstCurrentOut;
	std::deque<T> * m_plstCurrentIn;
};


template <class T>
CFIFOx<T>::CFIFOx()
{
	InitializeCriticalSection(&cs1);
	InitializeConditionVariable(&m_cvAdded);
	m_plstCurrentOut = &m_lstData[0];
	m_plstCurrentIn = &m_lstData[1];
	m_bUseCV = false;
}

template <class T>
CFIFOx<T>::~CFIFOx()
{
	m_plstCurrentIn = nullptr;
	m_plstCurrentOut = nullptr;
}


template <class T>
bool CFIFOx<T>::GetItem(T& item)
{
	//CJCriticalSectionEx::Lock lock(m_jcs1);
	EnterCriticalSection(&cs1);
	if (m_plstCurrentOut->empty()) {
		m_jcs2.Enter();
		swap(m_plstCurrentIn, m_plstCurrentOut);
		m_jcs2.Leave();
		if (m_plstCurrentOut->empty()){
			LeaveCriticalSection(&cs1);
			return false;
		}
	}

	item = m_plstCurrentOut->front();
	m_plstCurrentOut->pop_front();
	LeaveCriticalSection(&cs1);
	return true;
}

template <class T>
void CFIFOx<T>::AddItem(T item)
{
	m_jcs2.Enter();
	m_plstCurrentIn->push_back(item);
	m_jcs2.Leave();

	if (!m_bUseCV)
		m_ssAdded.Set();
	else
		WakeConditionVariable(&m_cvAdded);
}

template <class T>
bool CFIFOx<T>::WaitForData(T& item, int mSec)
{
	EnterCriticalSection(&cs1);
	while (!GetItem(item))
		SleepConditionVariableCS(&m_cvAdded, &cs1, mSec);
	LeaveCriticalSection(&cs1);
	return true;
}

template <class T>
unsigned long CFIFOx<T>::GetSize()
{
	unsigned long rtn = 0;
	//CJCriticalSectionEx::Lock lock(m_jcs1);
	//CJCriticalSectionEx::Lock lock2(m_jcs2);

	EnterCriticalSection(&cs1);
	m_jcs2.Enter();

	rtn = (m_plstCurrentIn->size() + m_plstCurrentOut->size());
	m_jcs2.Leave();
	LeaveCriticalSection(&cs1);

	return rtn;

}

template <class T>
bool CFIFOx<T>::IsEmpty()
{
	bool rtn = false;
	EnterCriticalSection(&cs1);
	m_jcs2.Enter();
	rtn = (m_plstCurrentIn->empty() && m_plstCurrentOut->empty());
	m_jcs2.Leave();
	LeaveCriticalSection(&cs1);

	return rtn;

}

template <class T>
void CFIFOx<T>::Clear()
{
	EnterCriticalSection(&cs1);
	m_jcs2.Enter();
	while (!m_plstCurrentOut->empty()) {
		delete m_plstCurrentOut->front();
		m_plstCurrentOut->pop_front();
	}
	while (!m_plstCurrentIn->empty()){
		delete m_plstCurrentIn->front();
		m_plstCurrentIn->pop_front();
	}
	m_jcs2.Leave();
	LeaveCriticalSection(&cs1);
}