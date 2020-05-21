/////////////////////////////////////////////////////////////////////////////
//          File:: MSGQUEUEMANAGER.h
// Create Author:: Raymond Yung
// Create   Date:: 10 May 01
//
//   Description:: CMsgQueueManager Class Declaration
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
#ifndef __MSGQUEUEMANAGER_H__
#define __MSGQUEUEMANAGER_H__
#include "queuemgr.h"
#include "MsgObject.h"


// The size of all the field groups and the bodies of all message
// formats in byte
/*#define BLOCK_HEADER_SIZE	  3	// PAD(SYN) + SYN + SYN
#define RECORD_HEADER_SIZE	 16	// All record have the same header size		
#define TRADE_RECORD_SIZE	 35	// Trade and Quotation Details Msg Format	
#define STAT_RECORD_SIZE	 93	// Summary Statistics Message Format		
#define CONTROL_RECORD_SIZE	111	// Control/Freeform Message Format			
#define HSCUR_RECORD_SIZE	 76	// Hang Seng Current Data Message Format	
#define UNDERLY_RECORD_SIZE	 35	// Underlying Instrument Message Format		
#define TIPS_RECORD_SIZE	163	// TIPS Price Record Format		
#define CHECKSUM_SIZE		  3	// ETX + BCC + PAD(DEL)			

#define SOH         0x01    // Start Of Header
#define STX         0x02    // Start of TeXt    
#define ETX         0x03    // End of TeXt      
#define SYN         0x16    // SYNchronization  
#define LF          0x0a    // Line Feed        
#define CR          0x0d    // Carriage Return  
#define DEL         0x7f    // DELete         

#define INVALID_PRICE		LONG_MAX
#define CABINET_PRICE		-2
#define AUCTION_ORDER		-3
*/
/////////////////////////////////////////////////////////////////////////////
//
// Class      : 
// Description:
//


//////////////////////////////////////////////////////////////////////////////
// Class CMsgQueueManager
//////////////////////////////////////////////////////////////////////////////
class CQuoteListToolsDlg;
class CMsgQueueManager: public CQueueManager<COP_ITEM>
{

public :

	// -------------------------------------------------------------------------
	// Default Constructor
	CMsgQueueManager();
    virtual ~CMsgQueueManager();

	void	Init (CQuoteListToolsDlg *m_pMyself);
    void	Main(COP_ITEM& pItem);
	int		GetCount();

private:
	CQuoteListToolsDlg*	m_pMyself;
};

#endif  // __MSGQUEUEMANAGER_H__ 

