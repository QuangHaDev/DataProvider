/////////////////////////////////////////////////////////////////////////////
//          File:: ssCountedObjPtr.h
// Create   Date:: Nov 10 95
// Create Author:: Joseph Payne
//
//   Description:: ssCountedObjPtr class definition
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
//
#ifndef __CountedObjPtr_H__
#define __CountedObjPtr_H__

//==========================================================================
//
//		This class is a reference-counted object pointer template class.
//
//		This class has been designed to be multi-thread safe.
//
//		Performing the following operations with this class affects its
//		referential-controlled aggregate in the following way.
//
//		Construct from T*			Point to heap object with count of one
//		Copy pointer				Increase reference count
//		Destroy pointer				Decrease count and destroy heap object
//									if no pointers to it remain
//		Assign pointer				Decrease count for left-hand heap object
//									and destroy heap object if no pointers to
//									it remain; increase count for heap object
//									pointed to by right-hand pointer and point
//									to the same heap object
//		Assign T*					Decrease count for left-hand heap object
//									and destroy heap object if no pointers to
//									it remain; point to right-hand heap object
//									with count of one
//
//	Changes:
//	9-Apr-2001 Terence Lai
//	-	remove copy constructor and assignment operator of CountedBody as
//		declaration to disallow these two operation
//	-	disable unique() function at CountedHandle
//	-	declare virtual destructor of CountedObjPtr
//	-	assign 0 to p_body at CountedHandle after delete or let go of the p_body
//	-	at CountedHandle, SetPointer() to reuse the body should be protected
//
//==========================================================================

#include "ssCriticalSection.h"

class ssCountedBodyStatic
{
	public:
		static DWORD GetWaitForDeleteCount()
		{
			return m_dwCountedBodyWaitForDeleteCount;
		}

		static void IncrementWaitForDeleteCount()
		{
			::InterlockedIncrement((long*)&m_dwCountedBodyWaitForDeleteCount);
		}

	private:
		static DWORD m_dwCountedBodyWaitForDeleteCount;
};

/***************************************************************************************/
/***************************************************************************************/ 

// Declare template classes for friend functions following...
template <class T> class ssCountedBody;
template <class T> class ssCountedHandle;
template <class T> class ssCountedObjPtr;

// Friend functions must be declared and implemented before they are used
// in a template class definition. - Microsoft specific rule
template <class T>
BOOL operator==(const ssCountedBody<T>& lhs, const ssCountedBody<T>& rhs) {
	return lhs.the_p == rhs.the_p; 
}

template <class T>
BOOL operator!=(const ssCountedBody<T>& lhs, const ssCountedBody<T>& rhs) {
	return lhs.the_p != rhs.the_p; 
}

template <class T>
BOOL operator==(const ssCountedHandle<T>& lhs, const ssCountedHandle<T>& rhs) {
	return lhs.p_body == rhs.p_body; 
}

template <class T>
BOOL operator!=(const ssCountedHandle<T>& lhs, const ssCountedHandle<T>& rhs) {
	return lhs.p_body != rhs.p_body; 
}

template <class T>
BOOL operator==(const ssCountedObjPtr<T>& lhs, const ssCountedObjPtr<T>& rhs) {
	return (const ssCountedHandle<T>&)lhs == (const ssCountedHandle<T>&)rhs; 
}

template <class T>
BOOL operator!=(const ssCountedObjPtr<T>& lhs, const ssCountedObjPtr<T>& rhs) {
	return (const ssCountedHandle<T>&)lhs != (const ssCountedHandle<T>&)rhs; 
}

/***************************************************************************************/
/***************************************************************************************/ 

//	CLASS DECLARATIONS

/***************************************************************************************/
/***************************************************************************************/    

template <class T>
class ssCountedBody{

friend class ssCountedHandle<T>;
friend class ssCountedObjPtr<T>;

public:

	ssCountedBody();								// Construct as null pointer
	ssCountedBody(T*);								// Construct pointing at heap object
	virtual ~ssCountedBody();						// Decrease count, destroy if 0
	BOOL unique() const;							// Is count one?
	T& operator*() const;							// Access object
	BOOL isNull() const;
	void Acquire();									// Acquire the mux
	void Release();									// Release the mux
	void AddHandle();								// Increment reference count
	void RemoveHandle();							// Decrement reference count

protected:

	void SetPointer(T*);							// Sets the pointer and reference count

private:

	T* the_p;										// The heap object
	ULONG refCount;									// Number of pointers to heap object
	ssCriticalSection m_mux;						// Mutex to wrap reference count

	ssCountedBody(const ssCountedBody<T>&);			// Only one body allowed, no copy construction
	ssCountedBody<T>& operator=						// or assignment
		(const ssCountedBody<T>&);

	// Friend functions 
	friend BOOL operator==(const ssCountedBody<T>&, const ssCountedBody<T>&);
	friend BOOL operator!=(const ssCountedBody<T>&, const ssCountedBody<T>&);

};

template <class T>
class ssCountedHandle{

public:

	ssCountedHandle();							// Construct as null pointer
	ssCountedHandle(const ssCountedHandle<T>&);	// Copy constructor
	ssCountedHandle(T*);						// Construct pointing at heap object
	virtual ~ssCountedHandle();					// Decrease count, destroy if 0

	ssCountedHandle<T>& operator=				// Adjust counts
		(const ssCountedHandle<T>&);
	ssCountedHandle<T>& operator=(T*);			// Decrease lhs count
//	Do not require unique at Counted Handle
//	BOOL unique() const;						// Is count one?
	T& operator*() const;						// Access object
	operator T*() const;						// Useful for typecasting heap object
	T* operator()() const;						// Typecasting backward compatibility

	BOOL isNull() const;

protected:

	ssCountedBody<T>* p_body;						// The body of the handle

private:

	// Friend functions 
	friend BOOL operator==(const ssCountedHandle<T>&, const ssCountedHandle<T>&);
	friend BOOL operator!=(const ssCountedHandle<T>&, const ssCountedHandle<T>&);

};  

template <class T>
class ssCountedObjPtr : public ssCountedHandle<T> {

public:

	ssCountedObjPtr();
	ssCountedObjPtr( const ssCountedObjPtr<T>& );
	ssCountedObjPtr( T* );
	virtual ~ssCountedObjPtr();

	ssCountedObjPtr<T>& operator=(const ssCountedObjPtr<T>&);
	ssCountedObjPtr<T>& operator=(T*);
	T* operator->() const;

	// Friend functions
	friend BOOL operator==(const ssCountedObjPtr<T>&, const ssCountedObjPtr<T>&);
	friend BOOL operator!=(const ssCountedObjPtr<T>&, const ssCountedObjPtr<T>&);

};

/***************************************************************************************/
/***************************************************************************************/

//	INLINES	- ssCountedBody

/***************************************************************************************/
/***************************************************************************************/

template <class T>
inline ssCountedBody<T>::ssCountedBody() : the_p(0), 
	refCount(1) {}

/***************************************************************************************/
// Counted Body copy constructor not allowed
//template <class T>
//inline ssCountedBody<T>::ssCountedBody(const ssCountedBody<T>& that) {}
//
/***************************************************************************************/

template <class T>
inline ssCountedBody<T>::ssCountedBody(T* just_newed) : the_p(just_newed),
	refCount(1) {}

/***************************************************************************************/	
// Counted Body assignment operator not allowed
//template <class T>
//inline ssCountedBody<T>&
//ssCountedBody<T>::operator=(const ssCountedBody<T>& rhs) {
//  return *this;
//}
//
/***************************************************************************************/

template <class T>
inline ssCountedBody<T>::~ssCountedBody() {

	m_mux.Enter();
	while (!unique())
	{
		m_mux.Leave();
		Sleep(0);
		ssCountedBodyStatic::IncrementWaitForDeleteCount();
		m_mux.Enter();
	}
	delete the_p;
	m_mux.Leave();
	the_p = 0;
}

/***************************************************************************************/

template <class T>
inline BOOL ssCountedBody<T>::unique() const { return refCount == 1; } 

/***************************************************************************************/

template <class T>
inline T& ssCountedBody<T>::operator*() const { return *the_p; }  

/***************************************************************************************/

template <class T>
inline BOOL ssCountedBody<T>::isNull() const { return the_p == 0; }

/***************************************************************************************/

template <class T>
inline void ssCountedBody<T>::Acquire() { 
	 m_mux.Enter(); }

/***************************************************************************************/

template <class T>
inline void ssCountedBody<T>::Release() { 
	 m_mux.Leave(); }	

/***************************************************************************************/

template <class T>
inline void ssCountedBody<T>::AddHandle() { 
	++refCount; }	

/***************************************************************************************/

template <class T>
inline void ssCountedBody<T>::RemoveHandle() { 
	--refCount; }

/***************************************************************************************/

template <class T>
inline void ssCountedBody<T>::SetPointer(T* just_newed) { 
	delete the_p;
	the_p = just_newed;
	refCount = 1; 
}
	
/***************************************************************************************/
/***************************************************************************************/

//	INLINES	- ssCountedHandle

/***************************************************************************************/
/***************************************************************************************/

template <class T>
inline ssCountedHandle<T>::ssCountedHandle() : p_body(0) {}

/***************************************************************************************/

template <class T>
inline ssCountedHandle<T>::ssCountedHandle(const ssCountedHandle<T>& that) {

	if(that.isNull())
		p_body = 0;
	else 
	{
		((ssCountedHandle<T>&)(that)).p_body->Acquire();
		p_body = that.p_body;
		p_body->AddHandle();
		p_body->Release();
	}
}

/***************************************************************************************/

template <class T>
inline ssCountedHandle<T>::ssCountedHandle(T* just_newed) : 
	p_body( (just_newed)?new ssCountedBody<T>(just_newed):0 ) {}

/***************************************************************************************/

template <class T>
inline ssCountedHandle<T>::~ssCountedHandle() {

	// If there is a body, let it go
	if(!isNull())
	{
		p_body->Acquire();
		if(p_body->unique())
		{
			p_body->Release();
			delete p_body;
			p_body = 0;
		}
		else
		{
			p_body->RemoveHandle();
			p_body->Release();
			p_body = 0;
		}
	}
}

/***************************************************************************************/	

template <class T>
inline ssCountedHandle<T>&
ssCountedHandle<T>::operator=(const ssCountedHandle<T>& rhs) {
	
	// Check for self assignment
	if (this == &rhs)
		return *this;

	// If there is a body, let it go
	if(!isNull())
	{
		p_body->Acquire();
		if(p_body->unique())
		{
			p_body->Release();
			delete p_body;
			p_body = 0;
		}
		else
		{
			p_body->RemoveHandle();
			p_body->Release();
			p_body = 0;
		}
	}

	// Set the handle to the new body
	if(rhs.isNull())
		p_body = 0;
	else
	{
		((ssCountedHandle<T>&)(rhs)).p_body->Acquire();
		p_body = rhs.p_body;
		p_body->AddHandle();
		p_body->Release();
	}

	return *this;
}

/***************************************************************************************/	

template <class T>
inline ssCountedHandle<T>&
ssCountedHandle<T>::operator=(T* just_newed) {

	// If there is a body, let it go
	// if someone else is still using it.
	// Otherwise, reuse the body for the new pointer.
	if(!isNull())
	{
		p_body->Acquire();
		if(p_body->unique())
		{
			// reuse the body
			if(just_newed)
			{
				p_body->SetPointer(just_newed);
				p_body->Release();
			}
			else
			{
				p_body->Release();
				delete p_body; 
				p_body = 0;
			}
			return *this;
		}
		else
		{
			// let go the body
			p_body->RemoveHandle();
			p_body->Release();
			p_body = 0;
		}
	}

	// Create new body for the pointer 
	p_body = (just_newed) ? new ssCountedBody<T>(just_newed) : 0; 

	return *this;
}

/***************************************************************************************/
// Do not require unique at CountedHandle
//template <class T>
//inline BOOL ssCountedHandle<T>::unique() const { return p_body->unique(); } 
//
/***************************************************************************************/

template <class T>
inline T& ssCountedHandle<T>::operator*() const { return *(*p_body); }  

/***************************************************************************************/

template <class T>
inline ssCountedHandle<T>::operator T*() const { 
	T* p = (p_body) ? p_body->the_p : 0; 
	return p;
}

/***************************************************************************************/

template <class T>
inline T* ssCountedHandle<T>::operator()() const { 
	T* p = (p_body) ? p_body->the_p : 0; 
	return p;
}

/***************************************************************************************/

template <class T>
inline BOOL ssCountedHandle<T>::isNull() const { return p_body == 0; }

/***************************************************************************************/
/***************************************************************************************/

//	INLINES	- ssCountedObjPtr

/***************************************************************************************/
/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>::ssCountedObjPtr() : ssCountedHandle<T>() {}

/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>::ssCountedObjPtr(const ssCountedObjPtr<T>& that) :
	ssCountedHandle<T>(that) {}

/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>::ssCountedObjPtr( T* just_newed ) :
	ssCountedHandle<T>(just_newed) {}

/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>::~ssCountedObjPtr() {}

/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>& ssCountedObjPtr<T>::operator=(const ssCountedObjPtr<T>& rhs) {
	ssCountedHandle<T>::operator=(rhs);
	return *this;
}

/***************************************************************************************/

template <class T>
inline ssCountedObjPtr<T>& ssCountedObjPtr<T>::operator=(T* rhs) {
	ssCountedHandle<T>::operator=(rhs);
	return *this;
}

/***************************************************************************************/

template <class T>
inline T* ssCountedObjPtr<T>::operator->() const { 
	T* p = (p_body) ? p_body->the_p : 0; 
	return p;
}

/***************************************************************************************/
/***************************************************************************************/

#endif  // __CountedObjPtr_H__
