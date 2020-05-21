/////////////////////////////////////////////////////////////////////////////
//          File:: MsgObject.cpp
// Create Author:: Sammy Ha
// Create   Date:: 30 Semptember 2000
//
//   Description:: The implementation file of the MsgObject.h
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
#define _CRT_SECURE_NO_WARNINGS			1

#include "MsgObject.h"


// ************************** Decode ***************************************
// -------------------------------------------------------------------------
// Constructor - decode
MsgObject::MsgObject(const byte *pbyMsg)
{
	// initialize members
	InitMembers();

	// Get Message Length
    if (pbyMsg == 0)
    {
		m_decodedflag = F; // treat it as full decoded when error encountered
		return;
	}

/*	if (*pbyMsg & 0x01) // 2 bytes
	{
		// Get Message Length
		memcpy(&m_nMsgLen, pbyMsg, 2);
		m_nMsgLen = m_nMsgLen >> 1;
		m_nDataLen+=2;
	}
	else // 1 byte
	{
		m_nMsgLen = *pbyMsg;
		m_nMsgLen = m_nMsgLen >> 1;
		m_nDataLen++;
	}
	// copy the whole msg stream to msg buffer
/*	m_nBufferSize = m_nMsgLen+1;
	m_pbyMsgBuffer = new byte[m_nBufferSize];
	memset(m_pbyMsgBuffer, 0, m_nBufferSize);
	memcpy((char*)m_pbyMsgBuffer, (char*)pbyMsg, m_nMsgLen);
	// parse the header (not include the msg length)
	m_pbyDataBuffer = m_pbyMsgBuffer + m_nDataLen;
    ParseMsgHeader();
*/
//	m_pbyMsgBuffer = pbyMsg;
    ParseMsgHeader(pbyMsg);
	m_nBufferSize = m_nMsgLen-m_byHeaderLen+11;
	m_pbyHeader = new byte[m_nBufferSize];
	memset(m_pbyHeader, 0, m_nBufferSize);
	m_pbyMsgBuffer = m_pbyHeader-m_byHeaderLen+10;
	memcpy((char*)m_pbyMsgBuffer, (char*)pbyMsg, m_nMsgLen);
	// Update the Decode cursor to the beginning of the data
	m_pbyDataBuffer = m_pbyMsgBuffer + m_byHeaderLen;
	m_pbyDecodeCur = m_pbyDataBuffer;
}

// -------------------------------------------------------------------------
// Parse the message header, when finish parsing, the data buffer pointer 
// will points to the beginning of the data
void MsgObject::ParseMsgHeader(const byte* pbyData)
{
	// MsgLen
	if (*pbyData & 0x01) // 2 bytes
	{
		// Get Message Length
//		memcpy(&m_nMsgLen, pbyData, 2);
//		m_nMsgLen = m_nMsgLen >> 1;
		m_nMsgLen = *(unsigned short*) pbyData >> 1;			// changed by Terence must cast unsigned before shifted
		m_byHeaderLen+=2;
	}
	else // 1 byte
	{
//		m_nMsgLen = *pbyData;
//		m_nMsgLen = m_nMsgLen >> 1;
		m_nMsgLen = (unsigned short) (*pbyData >> 1);		// changed by Terence must cast unsigned before shifted
		m_byHeaderLen++;
	}
	
	// DPID & DPInsID
	short nDP;
	if (*(pbyData+m_byHeaderLen) & 0x01)			// 2 bytes
	{
		memcpy(&nDP, pbyData+m_byHeaderLen, 2);

		// move to the Sequence No
		m_byHeaderLen+=2;
	}
	else								// 1 byte
	{
		nDP = *(pbyData+m_byHeaderLen);

		// move to the Sequence No
		m_byHeaderLen++;
	}
	//m_nDPID = (short) (nDP >> 3);	// DPID
	m_nDPID = (short) ((unsigned short)nDP >> 3);	// DPID  changed by Terence must cast unsigned before shifted
	m_byDPInsID =  (nDP & 0x06) >> 1;		// DP instant ID

	// Sequence No
	m_bySeqNo = *(pbyData+m_byHeaderLen);
	m_byHeaderLen++;

	// Message Type
	m_byMsgType = *(pbyData+m_byHeaderLen);
	m_byHeaderLen++;

	// Item Class
	memcpy(&m_lItemNo, (pbyData+m_byHeaderLen), 4);
	m_byHeaderLen+=4;
	m_nDataLen = m_nMsgLen - m_byHeaderLen;
}

// -------------------------------------------------------------------------
// Find the index of the requested FID in the Fid Array
short MsgObject::SearchFid(short nFid)
{
    for (short i = 0; i < m_nFidsCount; i++)
    {
        if (nFid == (m_pFidArray+i)->nFidCode)
           return i;
    }
    return -1; //(Fid not found)
}

// -------------------------------------------------------------------------
// If it is not the end of the data buffer, go through the data stream to 
// find the passing FID, stop and return the found position in the fid array,
// otherwise, return not found
short MsgObject::Decode(short nFid)
{
	while ((unsigned long) (m_pbyDecodeCur - m_pbyDataBuffer) < m_nDataLen)
	{
		ParsedFid tmpFid;

		// Decode the Fid
		memcpy(&tmpFid.nFidCode, m_pbyDecodeCur, 2);		// copy the FID
		m_pbyDecodeCur+=2;

		unsigned short rtnLen = GetDataLength(tmpFid.nFidCode, m_pbyDecodeCur);	// get the data length;
		if (rtnLen == 0 || (m_pbyDecodeCur - m_pbyDataBuffer) + rtnLen > m_nDataLen)
		{
			// Error Fid
			// treat it as full decoded when error encountered
			m_decodedflag = F;
			//if ( rtnLen != 0)
			//	CApplManClient::Log(LOG_ERROR, "[MsgObject] Decode Error: %u(0x%x):%d, Fid LEN %d", m_lItemNo, m_lItemNo, tmpFid.nFidCode, rtnLen);
			//else 
			//	CApplManClient::Log(LOG_ERROR, "[MsgObject] Decode Error: %u(0x%x):%d, FID cannot be recognized>>", m_lItemNo, m_lItemNo, tmpFid.nFidCode, rtnLen);
			return -1;
		}		
		tmpFid.nDataLength = rtnLen;

		tmpFid.pbyData = m_pbyDecodeCur;					// get the data pointer of that fid

		// Add the fid to the Parsed Fid Array
		ExpandArrayIfNeeded();
		*(m_pFidArray + m_nFidsCount) = tmpFid;
		
		m_pbyDecodeCur += tmpFid.nDataLength;
		m_decodedflag = P;
		m_nFidsCount++;

		// found the fid
		if (tmpFid.nFidCode == nFid)
			return m_nFidsCount;
	}

	// Not found the FID
	m_decodedflag = F;
	return -1;
}

// -------------------------------------------------------------------------
// Define the size of the Fid Array
void MsgObject::DefineSizeOfArray()
{
	m_nArraySize = 20;
    m_pFidArray = new ParsedFid[m_nArraySize];
    memset(m_pFidArray, 0, sizeof(ParsedFid)*(m_nArraySize));

    m_bDefineArray = TRUE;
}

// -------------------------------------------------------------------------
// Get the Fid data by FID
ParsedFid* MsgObject::GetFidData(short nFid)
{
	short i;

	// Search the Fid data from the internal fid array
	if ((m_decodedflag == P) || (m_decodedflag == F))
	{
		if (m_pFidArray)
		{
			i = SearchFid(nFid);
			if (i != -1)
				return m_pFidArray+i;			// found, return the Parsed Fid
			else 
			{
				if (m_decodedflag == F)			// not found, if Fully decoded, return NULL
					return NULL;
			}
		}
		else
			return NULL;
	}

	// Decode the byte stream
	i = Decode(nFid);
	if (i != -1)
		return m_pFidArray + i -1;				// found, return the Parsed Fid
    else 
        return NULL;
}

// -------------------------------------------------------------------------
// Get the Fid Type - used by the iterator (it will call decode when the it
// is not parsed)
short MsgObject::GetFidType(short nIndex, short &nFid)
{
	if (nIndex < 0)
		return 0;

	if ((nIndex < m_nFidsCount) && (nIndex != 0))
		nFid = (m_pFidArray+nIndex)->nFidCode;
	else
	{
		if ((unsigned long) (m_pbyDecodeCur - m_pbyDataBuffer) >= m_nDataLen)
			return 0;
		memcpy(&nFid, m_pbyDecodeCur, 2);
		Decode(nFid);
	}
	return FidDB.GetFidType(nFid);
}

// -------------------------------------------------------------------------
// Get the count of fids
short MsgObject::GetFidsCount()
{
	if (m_decodedflag == F)
		return m_nFidsCount;

	return 0;
}

// -------------------------------------------------------------------------
// Expand the Array size if needed
void MsgObject::ExpandArrayIfNeeded()
{
	if (!IsDefineArray())
	{
		DefineSizeOfArray();
	}

    if (m_nFidsCount + 1 >= m_nArraySize)
    {
        ParsedFid *pTmpBuffer;
        m_nArraySize = m_nArraySize+10;
        pTmpBuffer = new ParsedFid[m_nArraySize];
        memset(pTmpBuffer, 0, sizeof(ParsedFid)*(m_nArraySize));
        memcpy(pTmpBuffer, m_pFidArray, sizeof(ParsedFid)*m_nFidsCount);
        delete [] m_pFidArray;
        m_pFidArray = pTmpBuffer;
    }
}

// -------------------------------------------------------------------------
// Get Data length
unsigned short MsgObject::GetDataLength(short nFid, byte* pbyDecodeCur)
{
	unsigned short nDataLen = 0;
	short nDataType = FidDB.GetFidType(nFid);

	// calculate the data length according to its data type
	switch (nDataType)
	{
		case NGN_CHAR:
			nDataLen = 1;
			break;
		case NGN_UNICODE:
		case NGN_SHORT:
		case NGN_USHORT:
			nDataLen = 2;
			break;
		case NGN_LONG:
		case NGN_ULONG:
		case NGN_FLOAT:
		case NGN_UFLOAT:
		case NGN_KMULTIPLE:
		case NGN_10KMULTIPLE:
			nDataLen = 4;
			break;
		case NGN_DOUBLE:
		case NGN_UDOUBLE:
			nDataLen = 8;
			break;
		case NGN_CHARARRAY:
			nDataLen = 4+(*(pbyDecodeCur+2));
			break;
		case NGN_MCHARARRAY:{
			//MCharArray MCA(pbyDecodeCur);
			//nDataLen = MCA.GetDataLen();
			nDataLen = MCharArray::GetByteLen(m_pbyDecodeCur);
			break;}
		case NGN_BDATEYEAR:
			nDataLen = 4;
			break;
		case NGN_BDATEMONTH:
			nDataLen = 2;
			break;
		case NGN_BTIMEMIN:
			nDataLen = 2;
			break;
		case NGN_BTIMESEC:
			nDataLen = 3;
			break;
		case NGN_BTIMERANGES:
			nDataLen = 1+*(pbyDecodeCur)*4;
			break;
		case NGN_PRCVOL:
			nDataLen = 3+*(pbyDecodeCur+1)*12;
			break;
		case NGN_TRAN:
			nDataLen = 1+(*pbyDecodeCur)*19;
			break;
		case NGN_TXN:
			nDataLen = 9+*(pbyDecodeCur+4)*15;
			break;
		case NGN_SHRORDLST:
			nDataLen = 4;
			break;
		case NGN_BKRQUE:{
			/*BkrQue bkrque(pbyDecodeCur);
			nDataLen = bkrque.GetDataLen();*/
			nDataLen = BkrQue::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_LINK:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen*4;
			break;}
		case NGN_WLINK:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen*12;
			break;}
		case NGN_YLABEL:
			nDataLen = 12;
			break;
		case NGN_XMTCLABEL:
		case NGN_XDTCLABEL:
			nDataLen = 1+*(pbyDecodeCur)*4;
			break;
		case NGN_MTCPTS:
		case NGN_MTCPOINTS:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen;
			break;}
		case NGN_MTCVAL:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen*4;
			break;}
		case NGN_DTCPOINTS:
			nDataLen = 1+*(pbyDecodeCur)*3;
			break;
		case NGN_HISVAL:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 8+nDataLen*16;
			break;}
		case NGN_FIELDLIST:
			nDataLen = 1+*(pbyDecodeCur)*2;
			break;
		case NGN_FIELDLABEL:{
			/*FieldLabel fieldlabel(pbyDecodeCur);
			nDataLen = fieldlabel.GetDataLen();*/
			nDataLen = FieldLabel::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_INFOMSG:{
			/*InfoMsg infomsg(pbyDecodeCur);
			unsigned short nDataLen2 = infomsg.GetDataLen();*/
			nDataLen = InfoMsg::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_HEADLINE:{
			/*HeadLine headline(pbyDecodeCur);
			nDataLen = headline.GetDataLen();*/
			nDataLen = HeadLine::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_YHEADLINE:{
			/*YHeadLine yheadline(pbyDecodeCur);
			nDataLen = yheadline.GetDataLen();*/
			nDataLen = YHeadLine::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_HLINK:
			nDataLen = 3+*(pbyDecodeCur+1)*8;
			break;
		case NGN_MSHRORDLST:
			nDataLen = 1+*(pbyDecodeCur)*4;
			break;
		case NGN_SPRTBL:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen*12;
			break;}
		case NGN_QUOTELIST:{
			/*QuoteList quoteList;
			quoteList.Init (pbyDecodeCur);
			nDataLen = quoteList.GetDataLen();*/
			nDataLen = QuoteList::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_MTCTICKS:{
			/*MTCTicks mtcTicks (pbyDecodeCur);
			nDataLen = mtcTicks.GetDataLen();*/
			nDataLen = MTCTicks::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_MTCV:{
			/*unsigned long rem = m_nDataLen- (m_pbyDecodeCur - m_pbyDataBuffer);
			MTCV mtcv (pbyDecodeCur,rem);
			nDataLen = mtcv.GetDataLen();*/
			nDataLen = MTCV::GetByteLength(pbyDecodeCur);
			break;}
		case NGN_OBKRQUE:{
			/*OBkrQue obkrque(pbyDecodeCur);
			nDataLen = obkrque.GetDataLen();*/
			nDataLen = OBkrQue::GetByteLength(pbyDecodeCur);
			break;}
        case NGN_BQUEVOSN:{
            /*BQueVOSN bquevosn(pbyDecodeCur);
            nDataLen = bquevosn.GetDataLen();*/
			nDataLen = BQueVOSN::GetByteLength(pbyDecodeCur);

            break;}
		case NGN_MTCF:{
			memcpy(&nDataLen, pbyDecodeCur+2, 2);
			nDataLen = 6+nDataLen*24;
			break;}
        case NGN_PRCVOLWSEQ:{
			nDataLen = 7+*(pbyDecodeCur+1)*12;
            break;}
		case NGN_TRAN_D:
			nDataLen = 1+(*pbyDecodeCur)*20;
			break;
		case NGN_TXN_D:
			nDataLen = 9+*(pbyDecodeCur+4)*16;
			break;
		default:
			break;
	}
	return nDataLen;
}

// -------------------------------------------------------------------------
// Get Data - char
bool MsgObject::GetFid(short nFid, char &cData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&cData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - unicode
bool MsgObject::GetFid(short nFid, WCHAR &uniData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&uniData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - short
bool MsgObject::GetFid(short nFid, short &nData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&nData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - unsigned short
bool MsgObject::GetFid(short nFid, unsigned short &unData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&unData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - long
bool MsgObject::GetFid(short nFid, long &lData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&lData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - unsigned long
bool MsgObject::GetFid(short nFid, unsigned long &ulData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&ulData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - float
bool MsgObject::GetFid(short nFid, float &fData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&fData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - double
bool MsgObject::GetFid(short nFid, double &dData)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	memcpy(&dData, tmpFid->pbyData, tmpFid->nDataLength);
	return TRUE;
}

// -------------------------------------------------------------------------
// Get Data - D11 CharArray
/*bool MsgObject::GetFid(short nFid, CharArray &CA)
{
	return GetData(nFid, CA);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	CA.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D12 MCharArray
//bool MsgObject::GetFid(short nFid, MCharArray &MCA)
//{
//	return GetData(nFid, MCA);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	MCA.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D13 BDateYear
//bool MsgObject::GetFid(short nFid, BDateYear &DY)
//{
//	return GetData(nFid, DY);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	DY.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D14 BDateMonth
//bool MsgObject::GetFid(short nFid, BDateMonth &DM)
//{
//	return GetData(nFid, DM);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	DM.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D15 BTimeMin
//bool MsgObject::GetFid(short nFid, BTimeMin &TM)
//{
//	return GetData(nFid, TM);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	TM.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D16 BTimeSec
//bool MsgObject::GetFid(short nFid, BTimeSec &TS)
//{
//	return GetData(nFid, TS);
/*	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	TS.Init(tmpFid->pbyData);
	return TRUE;
*///}

// -------------------------------------------------------------------------
// Get Data - D17 BTimeRanges
//bool MsgObject::GetFid(short nFid, BTimeRanges &TR)
//{
//	return GetData(nFid, TR);
//}

// -------------------------------------------------------------------------
// Get Data - call by other get fid functions which the type are inherit 
// from datatype
bool MsgObject::GetFid(short nFid, datatype &d)
{
	ParsedFid* tmpFid = GetFidData(nFid);
	if (!tmpFid)
		return FALSE;

	d.Init(tmpFid->pbyData);
	return TRUE;
}

// ************************** Encode ***************************************
// -------------------------------------------------------------------------
// Constructor - encode
MsgObject::MsgObject()
{
	// initialize members
	InitMembers();

	// Set the encode flag
	m_bIsEncode = TRUE;

	// Create a message buffer for the message object
	m_nBufferSize = 512;
//	m_pbyMsgBuffer = new byte[m_nBufferSize];
//	memset(m_pbyMsgBuffer, 0, m_nBufferSize);

	// Preset 10 bytes for the header information
//	m_pbyDataBuffer = m_pbyMsgBuffer + 10;

	m_pbyHeader = new byte[m_nBufferSize];
	memset(m_pbyHeader, 0, m_nBufferSize);

	// Preset 10 bytes for the header information
	m_pbyMsgBuffer = m_pbyHeader;
	m_pbyDataBuffer = m_pbyHeader + 10;
}

// -------------------------------------------------------------------------
// Set the Data Provider ID
void MsgObject::SetDPID(short nDPID)
{
	m_nDPID = nDPID;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set the Data Provider Instant ID
void MsgObject::SetDPInsID(byte byDPInsID)
{
	m_byDPInsID = byDPInsID;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set the Sequence No
void MsgObject::SetSeqNo(byte bySeqNo)
{
	m_bySeqNo = bySeqNo;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set the Message type
void MsgObject::SetMsgType(byte byMsgType)	
{
	m_byMsgType = byMsgType;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set the Item Class
void MsgObject::SetItemClass(byte byItemClass)	
{
	m_lItemNo = (((long) byItemClass)<<24) | m_lItemNo;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set the Delay
void MsgObject::SetDelay(bool bDelay)	
{
	m_lItemNo = (((long) (bDelay & 0x01))<<24) | m_lItemNo;
//	ConstructMsgHeader();
}

// -------------------------------------------------------------------------
// Set Whole Page No
void MsgObject::SetPageNo(long lPageNo)	
{
	m_lItemNo = (lPageNo & 0x00FFFFFF) | m_lItemNo;
//	ConstructMsgHeader();
}


// -------------------------------------------------------------------------
// Construct the Message Header
bool MsgObject::ConstructMsgHeader()
{
	ASSERT(m_nDataLen < 32758);
	bool bSuccess = FALSE;

    short sMsgLen;
    sMsgLen = (short) (6 + m_nDataLen);

	// Item Class, Delay Bit
	m_pbyMsgBuffer = m_pbyDataBuffer;
	m_pbyMsgBuffer -= 4;		// To the position of Item Number

	// Item Number
	memcpy(m_pbyMsgBuffer, &m_lItemNo, 4);

	// Message Type
	m_pbyMsgBuffer -= 1;
	*m_pbyMsgBuffer = m_byMsgType;

	// Sequence No
	m_pbyMsgBuffer -= 1;
	*m_pbyMsgBuffer = m_bySeqNo;

	// DPID and DPInsID
	if (m_nDPID > 31)
	{
        sMsgLen += 2;
		m_pbyMsgBuffer -= 2;
		short nDP = ((m_nDPID << 3) | 0x01) | ((m_byDPInsID << 1) & 0x06);
		memcpy(m_pbyMsgBuffer, &nDP, 2);
	}
	else
	{
        sMsgLen++;
		m_pbyMsgBuffer--;
		*m_pbyMsgBuffer = (((byte) m_nDPID) << 3) | ((m_byDPInsID << 1) & 0x06);
	}

	// msg length
    if (sMsgLen+1 > 127)
	{
        sMsgLen += 2;
        if (m_nMsgLen != sMsgLen)
            m_nMsgLen = sMsgLen;
		m_pbyMsgBuffer -= 2;
		short nMsgLen = m_nMsgLen << 1 | 0x01;
		memcpy(m_pbyMsgBuffer, &nMsgLen, 2);
	}
	else
	{
        sMsgLen++;
        if (m_nMsgLen != sMsgLen)
            m_nMsgLen = sMsgLen;
		m_pbyMsgBuffer--;
		*m_pbyMsgBuffer = ((byte) (m_nMsgLen << 1));
	}

	if (m_pbyMsgBuffer)
	{
		m_byHeaderLen = (byte) (m_nMsgLen - m_nDataLen);
		bSuccess = TRUE;
	}
	return bSuccess;
}
// -------------------------------------------------------------------------
// Get the constructed byte stream
const byte* MsgObject::GetMessage() 
{
	if (ConstructMsgHeader())
		return m_pbyMsgBuffer;
	else
		return NULL;
}

// -------------------------------------------------------------------------
// Expand the buffer size if needed
void MsgObject::ExpandBufferIfNeeded(unsigned short nLen)
{
	if (m_nDataLen + nLen + 10 >= m_nBufferSize)
	{
		byte* pbyNewBuffer = NULL;
		m_nBufferSize = m_nBufferSize + ((nLen+10)/128+1)*128;
		pbyNewBuffer = new byte[m_nBufferSize];
		memset(pbyNewBuffer, 0, m_nBufferSize);
/*		if (m_bIsEncode)
			memcpy(pbyNewBuffer, m_pbyDataBuffer-10, m_nBufferSize - ((nLen+10)/128+1)*128);
		else
			memcpy(pbyNewBuffer+10-m_nMsgLen+m_nDataLen, m_pbyMsgBuffer, m_nBufferSize - ((nLen+10)/128+1)*128);

		delete [] m_pbyMsgBuffer;
		m_pbyMsgBuffer = pbyNewBuffer;
		m_pbyDataBuffer = m_pbyMsgBuffer + 10;
*/
		memcpy(pbyNewBuffer, m_pbyHeader, m_nBufferSize - ((nLen+10)/128+1)*128);
		delete [] m_pbyHeader;
		m_pbyHeader = pbyNewBuffer;
		m_pbyMsgBuffer = m_pbyHeader;
		m_pbyDataBuffer = m_pbyHeader + 10;
	}
}

// -------------------------------------------------------------------------
// Add Fid for D1-UChar
bool MsgObject::AddFid(short nFidCode, unsigned char* pucData)
{
	if ((!m_bIsEncode) || (!pucData))
		return FALSE;

	byte byLen = 1;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pucData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, unsigned short* punData)
{
	if ((!m_bIsEncode) || (!punData))
		return FALSE;

	byte byLen = 2;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, punData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, short* pnData)
{
	if ((!m_bIsEncode) || (!pnData))
		return FALSE;

	byte byLen = 2;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pnData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, long* plData)
{
	if ((!m_bIsEncode) || (!plData))
		return FALSE;

	byte byLen = 4;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, plData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, unsigned long* pulData)
{
	if ((!m_bIsEncode) || (!pulData))
		return FALSE;

	byte byLen = 4;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pulData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, float* pfData)
{
	if ((!m_bIsEncode) || (!pfData))
		return FALSE;

	byte byLen = 4;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pfData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

bool MsgObject::AddFid(short nFidCode, double* pdData)
{
	if ((!m_bIsEncode) || (!pdData))
		return FALSE;

	byte byLen = 8;

	// expand the buffer size if needed
	ExpandBufferIfNeeded(byLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pdData, byLen);
	m_nDataLen = m_nDataLen + byLen + 2;
	return TRUE;
}

// -------------------------------------------------------------------------
// Add Fid for other types
bool MsgObject::AddFid(short nFidCode, datatype* pData)
{
	if ((!m_bIsEncode) || (!pData->GetData()))
		return FALSE;

	// expand the buffer size if needed
	unsigned short nDataLen = pData->GetDataLen();
	ExpandBufferIfNeeded(nDataLen+2);

	// add the FID
	memcpy(m_pbyDataBuffer+m_nDataLen, &nFidCode, 2);

	// add the data to the buffer stream
	memcpy(m_pbyDataBuffer+m_nDataLen+2, pData->GetData(), nDataLen);
	m_nDataLen = m_nDataLen + nDataLen + 2;
	return TRUE;
}

// -------------------------------------------------------------------------
// Add Fid with datatype converstion - added by Louisa
bool MsgObject::AddFidString(short nFidCode, char* pData)
{

   short nFidType= FidDB.GetFidType(nFidCode);

   switch (nFidType)
   {
	
	case NGN_CHAR :
		{
			int num= atoi(pData);
			char CA=num;
			AddFid(nFidCode, (unsigned char*)&CA);
		}
		break;
    case NGN_UNICODE:
	
		AddFid(nFidCode,(unsigned short*)pData);
		break;
	case NGN_SHORT:
		{
			short nShortBuf;
			nShortBuf=(short)atol(pData);
			AddFid(nFidCode,(short*)&nShortBuf);
		}
		break;
	case NGN_USHORT:
		{
			unsigned short nUShortBuf;	
			nUShortBuf= (unsigned short)atol(pData);
			AddFid(nFidCode,(unsigned short*)&nUShortBuf);
		}
		break;
	case NGN_LONG:
		{
			long lLongBuf;	
			lLongBuf = atol(pData);
			AddFid(nFidCode,(long*)&lLongBuf);
		}
		break;
	case NGN_ULONG:
		{
			unsigned long lULongBuf;
			lULongBuf = atol(pData);
			AddFid(nFidCode,(unsigned long*)&lULongBuf);
		}
		break;
	case NGN_FLOAT:
		{
			float fFloatBuf;
			fFloatBuf = (float)atof(pData);
			AddFid(nFidCode,(float*)&fFloatBuf);
		}
		break;
	case NGN_UFLOAT:
		{
			float ufUFloatBuf;	
			ufUFloatBuf = (float)atof(pData);
			AddFid(nFidCode,(float*)&ufUFloatBuf);
		}
		break;
	case NGN_DOUBLE:
		{
			double dbDoubleBuf;	
			dbDoubleBuf= atof(pData);
			AddFid(nFidCode,(double*)&dbDoubleBuf);
		}
		break;
	case NGN_UDOUBLE:
		{
			double dbUDoubleBuf;
			dbUDoubleBuf =atof(pData);
			AddFid(nFidCode,(double*)&dbUDoubleBuf);
		}
		break;

	case NGN_CHARARRAY:
		{
			CharArray CA;
			CA.SetSq(1);
			CA.SetPos(255);
			CA.SetLen(strlen(pData)+1);
			CA.SetString((byte*)pData);
    		AddFid(nFidCode,&CA);
		}
		break;

	case NGN_BDATEYEAR:
		{
				
			WORD wYear = 0;
			WORD wMonth = 0;
			WORD wDay = 0;
			char szMonth[20];	
			char MonthArray[] = "JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC ";    	
			int nConvResult=sscanf_s(pData, "%2hu %3s %4hu", &wDay, &szMonth[0],sizeof(szMonth), &wYear);
			szMonth[3] = 0;
			if (nConvResult != 0 && nConvResult != EOF)
			{
				char *pFound;

				if ((pFound = strstr(MonthArray, szMonth)) != NULL)
				{
				wMonth = ((WORD)(pFound - MonthArray) / 4) + 1;
				}
			}
		
			BDateYear bDate(wYear, (byte)wMonth, (byte)wDay); 
			AddFid(nFidCode,&bDate);
		}
		break;
	
	case NGN_BTIMEMIN:
		{
			WORD wHour = 0;
			WORD wMinute = 0;
			char MonthArray[] = "JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC ";    
			sscanf_s(pData, "%2hu:%2hu", &wHour, &wMinute);
			BTimeMin	bTimeMin((byte)wHour, (byte)wMinute);
			AddFid(nFidCode, &bTimeMin);
		}
		break;
    case NGN_BTIMESEC:
        {
                WORD wHour = 0;
                WORD wMinute = 0;
                WORD wSecond = 0;  
                sscanf(pData, "%2hu:%2hu:%2hu", &wHour, &wMinute, &wSecond);
                BTimeSec        bTimeSec((byte)wHour, (byte)wMinute, (byte)wSecond);
                AddFid(nFidCode, &bTimeSec);
        }
        break;


	default:
		return FALSE;
		break;
  }	

	return TRUE;

}

// ****************************** Share functions **************************************
// -------------------------------------------------------------------------
// Initialize data members 
void MsgObject::InitMembers()
{
	// initialize flags
	m_decodedflag = N;
//    m_bValidMsgObject = FALSE;
	m_bIsEncode = FALSE;

    // initialize header fields
	m_nMsgLen = 0;							// Message Length
	m_nDPID = 0;							// DP ID
	m_byDPInsID = 0;						// DP instance ID
	m_bySeqNo = 0;							// Sequence No
	m_byMsgType = 0;						// Message Type
	m_lItemNo = 0;							// Item Number
    
	// initialize internal data
	m_pFidArray = NULL;
	m_pbyHeader = NULL;
	m_pbyMsgBuffer = NULL;
	m_pbyDataBuffer = NULL;
	m_pbyDecodeCur = NULL;
	m_byHeaderLen = 0;
	m_nDataLen = 0;
	m_nBufferSize = 0;
	m_nArraySize = 0;
	m_nFidsCount = 0;
	m_bDefineArray = FALSE;
}

// -------------------------------------------------------------------------
// Destructor
MsgObject::~MsgObject()
{
	// remove the parsed fid array
    if (m_pFidArray)
	{
		delete[] m_pFidArray;
		m_pFidArray = NULL;
	}

	// remove the byte stream
    if (m_pbyHeader)
	{
		delete [] m_pbyHeader;
		m_pbyHeader = NULL;
	}

	if (m_pbyMsgBuffer)
		m_pbyMsgBuffer = NULL;

	if (m_pbyDataBuffer)
		m_pbyDataBuffer = NULL;
}
