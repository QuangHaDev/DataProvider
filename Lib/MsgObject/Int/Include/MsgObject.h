/////////////////////////////////////////////////////////////////////////////
//          File:: MsgObject.h
// Create Author:: Sammy Ha
// Create   Date:: 30 September 2000
//
//   Description:: Message object class to wrap messages 
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
#ifndef __MsgObject_h__
#define __MsgObject_h__

#include "Definition.h"
#define SYSTEM_PAGE 0xFE000000

#include "ssCountedObjPtr.h"
class MsgObject;
#define BUFFER_ITEM                     MsgObject
typedef ssCountedObjPtr<BUFFER_ITEM>    COP_ITEM;

namespace NGN2{
// ParsedFid Structure
// Each fid in the Fid Array of the ParsedFidIterator is Defined by Parsed_fid structure
struct ParsedFid
{
  short nFidCode;
  unsigned short nDataLength;
  byte* pbyData;
};


static fidDatabase FidDB;
}


using namespace NGN2;
/////////////////////////////////////////////////////////////////////////////
//
// Class      : MsgObject
// Description: MsgObject to hold the message and parse the message if it is 
//				required.
//
class MsgObject
{
public:
	// -------------------------------------------------------------------------
	// Constructor - for decode
	MsgObject(const byte* pbyMsg);

	// -------------------------------------------------------------------------
	// Constructor - for encode
	MsgObject();

	// -------------------------------------------------------------------------
	// Default Destructor
	virtual ~MsgObject();

// Operations		-		Header methods (Retrievial)
	// -------------------------------------------------------------------------
	// Get the message length
	const short GetMsgLen() const	{return m_nMsgLen;}

	const byte GetHeaderLen() const	 {return m_byHeaderLen; }

	// -------------------------------------------------------------------------
	// Get the Data Provider ID
	const short GetDPID() const	{return m_nDPID;}

	// -------------------------------------------------------------------------
	// Get the Data Provider Instant ID
	const byte GetDPInsID() const	{return m_byDPInsID;}

	// -------------------------------------------------------------------------
	// Get the Sequence No
	const byte GetSeqNo() const	{return m_bySeqNo;}

	// -------------------------------------------------------------------------
	// Get the Message type
	const byte GetMsgType() const	{return m_byMsgType;}

	// -------------------------------------------------------------------------
	// Get the Item Class - 7 bits
	//const byte GetItemClass() const	{return ((byte) ((m_lItemNo & 0xFE000000) >> 25));}
	// changed by terence, must cast to unsigned long before shift
	const byte GetItemClass() const	{return ((byte) ((unsigned long)(m_lItemNo & SYSTEM_PAGE) >> 25));}

	// -------------------------------------------------------------------------
	// Get the Delay - 1 bit
	const byte GetDelay() const	{return ((byte) ((m_lItemNo & 0x01000000) >> 24));}

	// -------------------------------------------------------------------------
	// Get page number -24 bits
	const long GetPageNo() const	{return (m_lItemNo & 0x00FFFFFF);}

	// -------------------------------------------------------------------------
	// Get the Item No - 32 bits
	const long GetItemNo() const	{return m_lItemNo;}

// Operations		-		Header methods (Set)
	// -------------------------------------------------------------------------
	// Set the Data Provider ID
	void SetDPID(short nDPID);

	// -------------------------------------------------------------------------
	// Set the Data Provider Instant ID
	void SetDPInsID(byte byDPInsID);

	// -------------------------------------------------------------------------
	// Set the Sequence No
	void SetSeqNo(byte bySeqNo);

	// -------------------------------------------------------------------------
	// Set the Message type
	void SetMsgType(byte byMsgType);

	// -------------------------------------------------------------------------
	// Set the Item Class
	void SetItemClass(byte byItemClass);

	// -------------------------------------------------------------------------
	// Set the Delay
	void SetDelay(bool bDelay);

	// -------------------------------------------------------------------------
	// Set the Item No
	void SetItemNo(long lItemNo)	{m_lItemNo = lItemNo;}

	// -------------------------------------------------------------------------
	// Set Page Number
	void SetPageNo(long lPageNo);

// Operations		-		Data methods
	// -------------------------------------------------------------------------
	// Get the number of parsed fids in the Fid Array
	short GetFidsCount();

// Operations		-		Message methods
	// -------------------------------------------------------------------------
	// Get the Constructed byte stream
	const byte* GetMessage();

//protected:
// Get Data according to the fid and the datatype
	// -------------------------------------------------------------------------
	// GetData Type D1
	virtual bool GetFid(short nFidCode, char &cData);

	// -------------------------------------------------------------------------
	// GetData Type D2 - This function is not implemented since the WCHAR is the
	// unsigned short, user can call the GetFid for Data Type D4 to get the Unicode
	virtual bool GetFid(short nFidCode, WCHAR &uniData);

	// -------------------------------------------------------------------------
	// GetData Type D3
	virtual bool GetFid(short nFidCode, short &nData);

	// -------------------------------------------------------------------------
	// GetData Type D4
	virtual bool GetFid(short nFidCode, unsigned short &unData);

	// -------------------------------------------------------------------------
	// GetData Type D5
	virtual bool GetFid(short nFidCode, long &lData);

	// -------------------------------------------------------------------------
	// GetData Type D6
	virtual bool GetFid(short nFidCode, unsigned long &ulData);

	// -------------------------------------------------------------------------
	// GetData Type D7 & D8
	virtual bool GetFid(short nFidCode, float &fData);

	// -------------------------------------------------------------------------
	// GetData Type D9 & D10
	virtual bool GetFid(short nFidCode, double &dData);

	// -------------------------------------------------------------------------
	// GetData Type D11 - CharArray
/*	virtual bool GetFid(short nFidCode, CharArray &CA);

	// -------------------------------------------------------------------------
	// GetData Type D12 - MCharArray
	virtual bool GetFid(short nFidCode, MCharArray &MCA);

	// -------------------------------------------------------------------------
	// GetData Type D13 - BDateYear
	virtual bool GetFid(short nFidCode, BDateYear &DY);

	// -------------------------------------------------------------------------
	// GetData Type D14 - BDateMonth
	virtual bool GetFid(short nFidCode, BDateMonth &DM);

	// -------------------------------------------------------------------------
	// GetData Type D15 - BTimeMin
	virtual bool GetFid(short nFidCode, BTimeMin &TM);

	// -------------------------------------------------------------------------
	// GetData Type D16 - BTimeSec
	virtual bool GetFid(short nFidCode, BTimeSec &TS);

	// -------------------------------------------------------------------------
	// GetData Type D17 - BTimeRanges
	virtual bool GetFid(short nFidCode, BTimeRanges &TR);

	// -------------------------------------------------------------------------
	// GetData Type D18 - Tran
	virtual bool GetFid(short nFidCode, Tran &Trn);

	// -------------------------------------------------------------------------
	// GetData Type D19 - Txn
	virtual bool GetFid(short nFidCode, Txn &Tx);

	// -------------------------------------------------------------------------
	// GetData Type D20 - ShrOrdLst
	virtual bool GetFid(short nFidCode, ShrOrdLst &SOL);

	// -------------------------------------------------------------------------
	// GetData Type D21 - RepeatUFloat
	virtual bool GetFid(short nFidCode, RepeatUFloat &ReUF);

	// -------------------------------------------------------------------------
	// GetData Type D22 - BkrQue
	virtual bool GetFid(short nFidCode, BkrQue &BkQ);

	// -------------------------------------------------------------------------
	// GetData Type D23 - Link
	virtual bool GetFid(short nFidCode, Link &Lnk);

	// -------------------------------------------------------------------------
	// GetData Type D24 - Wlink
	virtual bool GetFid(short nFidCode, Wlink &Wlnk);

	// -------------------------------------------------------------------------
	// GetData Type D25 - YLabel
	virtual bool GetFid(short nFidCode, YLabel &YLbl);

	// GetData Type D26 - XMtcLabel
	virtual bool GetFid(short nFidCode, XMtcLabel &XMtcLbl);

	// -------------------------------------------------------------------------
	// GetData Type D27 - XDtcLabel
	virtual bool GetFid(short nFidCode, XDtcLabel &XDtcLbl);

	// -------------------------------------------------------------------------
	// GetData Type D28 - MtcPoints
	virtual bool GetFid(short nFidCode, MtcPoints &MtcPts);

	// -------------------------------------------------------------------------
	// GetData Type D29 - MtcVal
	virtual bool GetFid(short nFidCode, MtcVal &MVal);

	// -------------------------------------------------------------------------
	// GetData Type D30 - DtcPoints
	virtual bool GetFid(short nFidCode, DtcPoints &DtcPts);

	// -------------------------------------------------------------------------
	// GetData Type D31 - HisVal
	virtual bool GetFid(short nFidCode, HisVal &HVal);

	// -------------------------------------------------------------------------
	// GetData Type D32 - FieldList
	virtual bool GetFid(short nFidCode, FieldList &FldLst);

	// -------------------------------------------------------------------------
	// GetData Type D33 - FieldLabel
	virtual bool GetFid(short nFidCode, FieldLabel &FldLbl);

	// -------------------------------------------------------------------------
	// GetData Type D34 - InfoMsg
	virtual bool GetFid(short nFidCode, InfoMsg &IMsg);

	// -------------------------------------------------------------------------
	// GetData Type D35 - HeadLine
	virtual bool GetFid(short nFidCode, HeadLine &HL);
*/

	// -------------------------------------------------------------------------
    // Get Fid from different datatype
	bool GetFid(short nFid, datatype &d);

// Add Data according to the fid and the datatype
	// -------------------------------------------------------------------------
	// Add D1 - Uchar
	bool AddFid(short nFidCode, unsigned char* pucData);

	// -------------------------------------------------------------------------
	// Add D2, D4 - UShort
	bool AddFid(short nFidCode, unsigned short* punData);

	// -------------------------------------------------------------------------
	// Add D3 - short
	bool AddFid(short nFidCode, short* pnData);

	// -------------------------------------------------------------------------
	// Add D5 - long
	bool AddFid(short nFidCode, long* plData);

	// -------------------------------------------------------------------------
	// Add D6 - unsigned long
	bool AddFid(short nFidCode, unsigned long* pulData);

	// -------------------------------------------------------------------------
	// Add D7 - float
	bool AddFid(short nFidCode, float* pfData);

	// -------------------------------------------------------------------------
	// Add D9 - double
	bool AddFid(short nFidCode, double* pdData);

	// -------------------------------------------------------------------------
	// Add Other DataType
	bool AddFid(short nFidCode, datatype* pData);


	// -------------------------------------------------------------------------
	// Add Fid with datatype converstion 
	bool AddFidString(short nFidCode, char* pData);

	// -------------------------------------------------------------------------
	// Get next fid data
	short GetFidType(short nIndex, short &nFid);

private:
// Attributes
	// -------------------------------------------------------------------------
	// Flags
	enum	Decoded {N, P, F} m_decodedflag;	// (N = Not decoded, P = Partial Decoded, F = Full Decoded) 
//	bool	m_bValidMsgObject;					// Check mesg object is valid or not
	bool	m_bIsEncode;						// Check the message object is used for encode or not

	// -------------------------------------------------------------------------
	// Headers
	short	m_nMsgLen;							// Message Length
	short	m_nDPID;							// DP ID
	byte	m_byDPInsID;						// DP instance ID
	byte	m_bySeqNo;							// Sequence No
	byte	m_byMsgType;						// Message Type
	long	m_lItemNo;							// Item Number

	// -------------------------------------------------------------------------
	// Internal Data
	ParsedFid* m_pFidArray;						// Parsed Array
	byte*	m_pbyHeader;						// Header pointer of the internal byte stream
	byte*	m_pbyMsgBuffer;						// Message Byte stream
	byte*	m_pbyDataBuffer;					// Data Byte stream
	byte*	m_pbyDecodeCur;						// Pointer which point to the current decoding part
	byte	m_byHeaderLen;						// Length of the header
	unsigned long	m_nDataLen;					// Data Length
	unsigned long	m_nBufferSize;				// Buffer size of the msg buffer
	short	m_nArraySize;						// Total length of the Fid Array
	short	m_nFidsCount;						// Total number of Fids in the Fid Array
	bool	m_bDefineArray;						// Boolean flag to check whether the array is define

// Operation
	// -------------------------------------------------------------------------
	// Initialize data members
	void InitMembers();

	// -------------------------------------------------------------------------
	// Parse the header and return the data pointer
	void ParseMsgHeader(const byte* pbyData);

	// -------------------------------------------------------------------------
	// Construct the message header
	bool ConstructMsgHeader();

	// -------------------------------------------------------------------------
	// Explain the Message Buffer size if needed
	void ExpandBufferIfNeeded(unsigned short nLen);

	// -------------------------------------------------------------------------
	// Explain the Fid Array size if needed
	void ExpandArrayIfNeeded();

	// -------------------------------------------------------------------------
    // Decode FID-Data, stop if error encountered
    short Decode(short nFid);

	// -------------------------------------------------------------------------
	// Find the index of the requested FID in the Fid Array
    short SearchFid(short nFid);

	// -------------------------------------------------------------------------
    // Get the Parsed Fid
	ParsedFid* GetFidData(short nFid);

	// -------------------------------------------------------------------------
    // Get the Data Length
	unsigned short GetDataLength(short nFid, byte* pbyDecodeCur);

	// -------------------------------------------------------------------------
    // Check whether the Array is defined
	BOOL IsDefineArray() {return(m_bDefineArray);}

	// -------------------------------------------------------------------------
    // Check whether the Array is defined
	void DefineSizeOfArray();

	// -------------------------------------------------------------------------
    // Get Fid from different datatype
//	bool GetData(short nFid, datatype &d);

};

#endif
