Component   : SystemServices
Version     : 2.0.0.4
=============
Description :
=============
SystemSevices provides objects that encapsulate various of win32 system
services. The encapsulated object simplies the usage of the system 
sevices and provide better protection on the system services.
The system services provided are:
- Critical Section
- Mutex
- Event
- Timer
- MultiWait
- Thread
- Utf8String
- Socket
- Shared Memory
- BitTable
- CountedObjPtr
- COPBuffer
- ActiveCOPBuffer
- MultiActiveCOPBuffer

================
Interface/Usage:
================
1. Critical Section - Encapsulate win32 CRITICAL_SECTION.
   //   Default Constructor, destructor, 
   //   Enter()    - Enter the critical section
   //   TryEnter() - Try to Enter the critical section
   //   Leave()    - Leave the critical section

2. Mutex - Encapsulate win32 Mutex
   //   Default Constructor, destructor, copy constructor, overloaded assignment operator
   //   Optional constructor - Provides full functionality of the mutex object.
   //   Acquire()            - Blocks until the mutex is acquired with a specified timeout.
   //   Release()            - Releases the mutex.
   //	GetResult()			 - Returns a system error or one of the specified results caused by the last wait operation.
   //	HANDLE()			 - Provides compatibility with the win32 API synchronization wait functions.

3. Event - Encapsulate win32 Event
   //   Default Constructor, destructor, copy constructor, overloaded assignment operator
   //   Optional constructor - Provides full functionality of the event object.
   //   Set()                - Sets the event
   //   Reset()              - Resets the event
   //   Wait()               - Blocks until the event becomes signaled with specified timeout. 
   //	GetResult()			 - Returns a system error caused by the last wait operation.
   //	HANDLE()			 - Provides compatibility with the win32 API synchronization wait functions.

4. Timer - Encapsulate win32 Waitable Timer (in m-secs, can specify completion routine)
   //   Default Constructor, destructor
   //   Optional constructor - Provides full functionality of the event object.
   //   Set()                - Sets the timer with optional completion routine
   //   Reset()              - Resets the timer
   //   Wait()               - Blocks until the event becomes signaled with specified timeout.
   //	GetResult()			 - Returns a system error caused by the last wait operation.
   //	HANDLE()			 - Provides compatibility with the win32 API synchronization wait functions.

5. MultiWait - Encapsulate win32 WaitForMultiple object with additional fairness wait/prioritied wait feature
             - Any waitable synchronization object can be added to this class for multi wait operaion.
   //   Default Constructor, destructor
   //   Add()                - Adds a synchronization object to the synchronization array.
   //   Wait()               - Waits for one or all synchronization objects to become signaled with a specified timeout.
   //   GetResult()          - Returns a Win32 API system error due to the last wait operation.
   //	IsValid()			 - Indicates whether the handle of the internal synchronization object is valid.
   //	Clear()				 - Clears all synchronization objects from the synchronization array.
   //	SetProioritized()	 - Sets the synchronization array to prioritized or non prioritized.
   //	Entries()			 - Returns the number of configured synchronization objects.
   //	GetAbandonedObject() - Returns the enumerated value associated of the last.

6. Thread - A class provide active context that encapsulates the thread API. 
		  - In addition, it provide manageability the the thread e.g CheckAlive, ExitThread
	      - A concrete class requires to provide the Main() implementation of
			the thread which always monitor the CheckAlive and ExitThread event
		  - Refer to test program "TestThread.cpp" for detail usage.
   //   Default Constructor, destructor
   //	Start()				 - Starts execution of the thread
   //	Suspend()			 - Suspends the thread
   //	Resume()			 - Resumes the thread
   //	SetPriority()		 - Sets the thread priority
   //	GetPriority()		 - Gets the thread priority
   //	GetID()				 - Returns thread ID
   //	GetName()			 - Returns thread name
   //	SetName()			 - Sets thread name
   //	HasStarted()		 - Indicates whether the thread has started execution
   //	IsAlive()			 - Waits for the concrete implementation to AckAlive with a specific timeout in m-sec
   //	ExitThread()		 - Trigger thread to exit
   //	IsExit()			 - Returns a flag to indicate exit thread has been triggered
   //	WaitForExit()		 - Waits for the thread to exit with a specific timeout in m-sec
   //	GetResult()			 - Returns a system error caused by the last wait operation
   //	HANDLE()			 - Provides compatibility with the win32 API synchronization wait functions

7. Utf8String
   - Provide an API GetPrivateProfileUTF8String () reading UNICODE (Wide Char) string from UNICODE encoded file specified 
     in parameter lpFileName.
	 DWORD GetPrivateProfileUTF8String(
              LPCTSTR lpAppName,        // points to section name
              LPCTSTR lpKeyName,        // points to key name
              LPCTSTR lpDefault,        // points to default string
              LPTSTR lpReturnedString,  // points to destination buffer
              DWORD nSize,              // size of destination buffer
              LPCTSTR lpFileName        // points to initialization filename
     )
   - Simulate Win32 API GetPrivateProfileString () to read setting from configuration file.
   - With similar parater list as GetPrivateProfileString ()
   - Return ZERO if the function fails

8. ssSocket - Encapsulate win32 Connection oriented socket
   //	Default Constructor, destructor,
   //	Create()			- Create a socket (usually as a socket server)
   //	Close()				- Close a socket
   //	CloseASocket()		- Close an accepted socket
   //	Connect()			- connect to  a specific scoket (usually as a socket client)
   //	Receive()			- Read a message to the specific buffer.
   //	Send()				- Write a message to the socket
   //	CancelBlockingCall()- Not use now
   //	Listen()			- listen for incoming connection request
   //	Accept()			- accept a connection on a socket
   //	ShutDown()			- Disable both Send and Receive calls
   //	GetLastError()		- return last socket error

9. ssSharedMemory - Encapsulate win32 shared memory
   //   Default Constructor, destructor
   //   Create()			- Create shared memory with read-write access
   //   Open()				- Open shared memory with read-only access
   //   Close()				- Close shared memory
   //   GetBuffer()			- Get a pointer to shared memory

10. ssBitTable - Manipulate the BitMap Object
   //	Default Constructor, destructor
   //	ResetAll()			- Reset all bits to 0
   //	SetAll()			- Set all bits to 1
   //	Reset()				- Reset a specified bit to 0
   //	Set()				- Set a specified bit to 1
   //	SetAs()				- Set a specified bit as true or false
   //	IsSet()				- Check if a specified bit is set
   //	GetSetBit()			- Get the bit number of the first set bit from LSB
   //	GetNonSetBit()		- Get the bit number of the first non-set bit from LSB
   //	TestAndSet()		- if the bit is not set, set it to 1
   //	TestAndReset()		- if the bit is set, reset it to 0
   //	operator|=()		- Operator Or Assignment

11. ssCountedObjPtr			- A reference counted object pointer template class
   //	Defualt Constructor, destructor
   //	operator=()			- Operator Assignment
   //	operator->()			- Operator ->
   //	operator==()			- Operator ==
   //	operator!=()			- Operator !=

12. ssCountedBody			- A reference counted object body template class 
   //	Default constructor, destructor
   //	unique()			- Check it is one or not
   //	operator*()			- Access Object
   //	isNull()			- Check it is null or not
   //	Acquire()			- Acquire the mux
   //	Release()			- Release the mux
   //	AddHandle()			- Increase reference count
   //	RemoveHandle()			- Decrease reference count

13. ssCountedHandle			- A reference counted object handle template class
   //	Default constructor, copy constructor, destructor
   //	operator=				- Adjust counts		
   //	operator=(T*)			- Decrease lhs count
   //	unique() const			- Check it is one or not
   //	operator*() const		- Access object
   //	operator T*() const		- Useful for typecasting heap object
   //	T* operator()() const		- Typecasting backward compatibility
   //	isNull() const			- Check it is null or not


14. ssCOPActiveBuffer	- A template class for active COP Buffer
   // Default Constructor, Destructor
   // IsAlive(DWORD dwTimeout)  	- Check the thread is alive
   // Pause()				- Pause the thread into wait state
   // Continue()			- Resume the paused thread from wait state
   // ExitThread()			- Terminate the thread
   // ssCOPBuffer<T>& operator()()	- Access to the buffer
   // Main()				- Override of abstract Main()
   // ProcessCOP()			- Abstract method to override which provides the stategy of 					  			
					  WHAT to do with the COP once one is retrieved from buffer.
   // ProcessDone() 			- Abstract method to override which provides the stategy of
					  WHAT to do if the COP buffer becomes empty, or active thread 
 					  going to terminate
   // OnError()				- Override to handle the error		
   // DefineUserEvent()			- User defined events - Return FALSE if defined EventMap has 
					  been used by this class
   // OnUserDefinedEvent()		- Override to execute user defined event handling
   ** Copy Constructor - not allowed to use

15. ssCOPBuffer	- A template class for COP Buffer
  // Default Constructor, Destructor
  // Insert()				- Inserts a COP into the buffer. Returns TRUE if successful.
  // Insert()				- Inserts an element directly into the COP buffer. When it is 
					  removed later, it is received in a COP of that element. 
  // Remove()				- Removes a COP from the buffer. If the buffer was empty, 
					  method returns an empty COP.
  // Remove()				- Removes a COP from the buffer and places it into the COP 
					  passed in as a parameter. Returns TRUE if successful.
  // Peek ()				- Get a reference to the 1st COP without actually removing it 
					  from the Buffer; If the buffer was empty, method returns an 
					  empty COP
  // Peek()				- Get a reference to the 1st COP without actually removing it 
					  from the Buffer; Returns TRUE if successful
  // GetEvent()				- Returns a reference to the event. Signals when the buffer is 
					  not empty.
  // GetCount()				- Returns the number of elements in the buffer.
  // GetUpperBound()			- Returns the current upper bound value.
  // GetIncrement()			- Returns the current increment value.
  // GetUpperLimit()			- Returns the current upper limit value.
  // SetUpperBound()			- Changes the upper bound to the new value. New value must be 
					  greater than the current upper bound. If the new upper bound 
					  is greater than the current upper limit, then the upper limit 
					  is also automatically changed to same value as upper bound.
  // SetIncrement()			- Changes the increment to the new value. If the new value is 
					  zero, the expanding buffer is disabled.
  // SetUpperLimit()			- Changes the upper limit to the new value. The new value must 
					  be greater than the current upper limit.

16. ssCOPMultiActiveBuffer	- A template class for multiple active COP Buffer
   //	Defualt Constructor, destructor
   //	AddBuffer()			- Set up the active buffers
   //	Insert()			- Send COP to specified buffer
   //	Insert()			- Send COP to all buffers - broadcast
   //	GetBuffer()			- Get the specified buffer
   //	RemoveBuffer()			- Removes the specified buffer
   //	RemoveAll()			- Removes all buffers
   //	Start()				- Start all active cop buffers
   //	CheckAlive()			- Check all active cop buffers are alive
   //	Pause()				- Pause all active cop buffers thread to wait state
   //	Continue()			- Resume all active cop buffers paused threads from wait state
   //	GetStartPosition()		- Get the position of the first buffer to remove
   //	GetNextBuffer()			- Get the next buffer specified by the position
==================
Known Deficiencies:
==================
* Noted by: Terence Lai    Date:19/08/98
  With Critical Section
  - if a thread calls Leave() when it does not have ownership of the specified 
    critical section object, an error occurs that may cause another thread 
    using Enter to wait indefinitely.
  - Copy constructor and assignment operatior is not allowed

* Noted by: Raymond Leung	Date:02/11/1998
  With Utf8String
  - Maximum length for the following parameters, lpAppName, lpKeyName and lpFileName,
    were 256 bytes; hence, 'section name', 'key name', 'initialzation filename' should
	be less than 255 characters.
  - Not able to serve as a function to get the length of the returned string when 
    'nSize' is specified as ZERO (It is a functionality provided by GetPrivateProfile-
	String())

===================
Future Enhancement:
===================
* Noted by: your name       Date:dd/mm/yy


===============
Change History:
===============
Version: 0.3			Date: 12-04-1999			Author:	Calvin Tam
Description
--------------------------------------------------------
SystemServices GetPrivateProfileUTF8String has memory leak (SPR RTDS0029), 
Add ssSocket (SPR RTDS0004.1)


Version: 1.0			Date: 09-06-1999			Author:	
Description
--------------------------------------------------------
allow ssSocket::Connect to try another times if connect failure.


Version: 1.0.0.12		Date: 20-10-1999			Author:	
Description
--------------------------------------------------------
Add ssSharedMemory component


Version: 1.0.0.12		Date: 18-02-2000			Author:	Joseph Yan
Description
--------------------------------------------------------
onRecv() assignment should be m_hASocket = INVALID_SOCKET;


Version: 1.0.0.12		Date: 31-08-2000			Author:	Sammy Ha
Description
--------------------------------------------------------
Add ssBitTable component


Version: 2.0.0.0		Date: 07-09-2000			Author:	Raymond Yung
Description
--------------------------------------------------------
Add ssCountedObjPtr component
Add ssCOPBuffer	component
Add ssCOPActiveBuffer component
Add ssCOPMultiActiveBuffer component

Version: 2.0.0.1		Date: 27-11-2000			Author:	MK Ching
Description
--------------------------------------------------------
Add addition constructor at ssTimer to allow Due Time to be signalled

Version: 2.0.0.2		Date: 26-09-2001			Author:	Terence Lai
Description
--------------------------------------------------------
Add multimedia timer ssMMTimer for real-time timer event

Version: 2.0.0.3		Date: 06-03-2002			Author:	Sargen Yung
Description
--------------------------------------------------------
Add UTF8ToGB conversion at ssCharacterSet

Version: 2.0.0.4		Date: 03-05-2002			Author:	Terence Lai
Description
--------------------------------------------------------
Add peak buffer cound GetMaxCount() at COPBuffer

Version: 2.0.0.5        Date: 09-09-2003            Author: John Lee
Description
--------------------------------------------------------
Add LeaveAll() for ssCriticalSection.

