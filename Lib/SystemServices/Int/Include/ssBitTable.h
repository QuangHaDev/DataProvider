/////////////////////////////////////////////////////////////////////////////
//          File:: ssBitTable.h
// Create Author:: Terence Lai
// Create   Date:: 07 August 1995
//   Description::
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

#ifndef __BITTABLE_H__
#define __BITTABLE_H__

///////////////////////////////////////////////////////////////////////////////
// 
//  Include files
//
#include <afxwin.h>             // MFC core and standard components
#include <afxext.h>             // MFC extensions
//#include <math.h>
#include "ssCommon.h"

///////////////////////////////////////////////////////////////////////////////
// 
//  Forward declaration
//

#pragma pack(push, INCLUDE_BITTABLE_H, 1)

// use BYTE as bit table entity
#define BITS_IN_ENTITY 8

// operand of shift bit Multiply or Division operations
#define SHIFT_OPERAND 3

// bit mask for modulo operation with bits in entity
#define MOD_MASK BITS_IN_ENTITY-1

// Zero bit mask to set the specified bit to zero
const BYTE byZeroBitMasks[BITS_IN_ENTITY] =
{
    0xFE, 0xFD, 0xFB, 0xF7,
    0xEF, 0xDF, 0xBF, 0x7F
};


///////////////////////////////////////////////////////////////////////////////
// 
//  Description: BitTable
//
class ssBitTable
{
public :

	ssBitTable(const UINT _uMaxBits);
	virtual ~ssBitTable();
              
//   BOOL Init(const UINT _uMaxBits);
//   BOOL Shutdown();
//   BOOL isReady() const;

	void ResetAll(void);
	void SetAll(void);
	BOOL Reset(const UINT uBit);
	BOOL Set(const UINT uBit);
	BOOL SetAs(const UINT uBit, const BOOL bValue);
	BOOL IsSet(const UINT uBit) const;
	UINT GetSetBit(const UINT uOffset = SS_INVALID_INDEX) const;
	UINT GetNonSetBit(const UINT uOffset = SS_INVALID_INDEX) const;
	BOOL TestAndSet(const UINT uBit);
	BOOL TestAndReset(const UINT uBit);
	ssBitTable &operator|=(ssBitTable &btRHS);

protected :

    // overload operator - To be implemented
    BOOL    operator!   (void);                       
            operator int(void);
    BOOL    operator==  (const ssBitTable& btRHS);     
    BOOL    operator!=  (const ssBitTable& btRHS);
    ssBitTable  operator&  (const ssBitTable& btRHS);     
    ssBitTable  operator|  (const ssBitTable& btRHS);                                    
    ssBitTable  operator>> (const LONG lBit);
    ssBitTable  operator<< (const LONG lBit);
    ssBitTable  operator~  (void);                       
    ssBitTable& operator=  (const ssBitTable& btRHS);
    ssBitTable& operator&= (const ssBitTable& btRHS);     
    ssBitTable& operator>>=(const LONG lBit);
    ssBitTable& operator<<=(const LONG lBit);

	// protected members
	UINT  uMaxBits;
	UINT  uBitMapSize;
	PBYTE pBitMap;

};

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::ssBitTable()
//               - ssBitTable default constructor
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline ssBitTable::ssBitTable(const UINT _uMaxBits)
{
   uMaxBits = 0;
   uBitMapSize = 0;
   pBitMap = NULL;

   uMaxBits = _uMaxBits;

   if(uMaxBits < BITS_IN_ENTITY)
      uBitMapSize = 1;
   else
      uBitMapSize = (uMaxBits/BITS_IN_ENTITY) + ((uMaxBits%BITS_IN_ENTITY) ? 1 : 0);

   pBitMap = new BYTE[uBitMapSize];
   if(pBitMap)
   {
      memset(pBitMap, 0x00, uBitMapSize);
 //     return(BOOL);
   }
   else
   {
      uMaxBits = 0;
      uBitMapSize = 0;
      pBitMap = NULL;
 //     return(FALSE);
   }
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::ssBitTable()
//               - ssBitTable destructor
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline ssBitTable::~ssBitTable()
{
   uMaxBits = 0;
   uBitMapSize = 0;

   if(pBitMap)
   {
      delete pBitMap;
      pBitMap = NULL;
   }

//   return(BOOL);
}

/*//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::Init()
//               -
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline BOOL ssBitTable::Init(const UINT _uMaxBits)
{
   uMaxBits = _uMaxBits;

   if(uMaxBits < BITS_IN_ENTITY)
      uBitMapSize = 1;
   else
      uBitMapSize = (uMaxBits/BITS_IN_ENTITY) + ((uMaxBits%BITS_IN_ENTITY) ? 1 : 0);

   pBitMap = new BYTE[uBitMapSize];
   if(pBitMap)
   {
      memset(pBitMap, 0x00, uBitMapSize);
      return(BOOL);
   }
   else
   {
      uMaxBits = 0;
      uBitMapSize = 0;
      pBitMap = NULL;
      return(FALSE);
   }
}*/

/*//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::isReady()
//               -
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline BOOL ssBitTable::isReady() const
{
   if(pBitMap)
      return(TRUE);
   else
      return(FALSE);
}*/
 
/*//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::Shutdown()
//               -
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline BOOL ssBitTable::Shutdown()
{
   uMaxBits = 0;
   uBitMapSize = 0;

   if(pBitMap)
   {
      delete pBitMap;
      pBitMap = NULL;
   }

   return(BOOL);
}*/

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::ResetAll()
//               - reset all bits to zero
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline void ssBitTable::ResetAll(void)
{
   memset(pBitMap, 0x00, uBitMapSize);
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::SetAll()
//               - set all bits to one
//
//   Parameters: Nil
//
//      Returns: Nil
//
inline void ssBitTable::SetAll(void)
{
   memset(pBitMap, 0xFF, uBitMapSize);
}       

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::Reset()
//               - reset a specified bit zero
//
//   Parameters: uBit   - bit to reset
//
//      Returns: TRUE   - if uBit is a valid bit, the bit will be reset
//               FLASE  - if uBit is an invalid bit
//
inline BOOL ssBitTable::Reset(const UINT uBit)
{
   if (uBit >= uMaxBits)
       return FALSE;
   else
   {
      *(pBitMap+(uBit >> SHIFT_OPERAND)) &= byZeroBitMasks[uBit & MOD_MASK];
      return TRUE;
   }
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::Set()
//               - set a specified bit one
//
//   Parameters: uBit   - bit to set
//
//      Returns: TRUE   - if uBit is a valid bit, the bit will be set
//               FLASE  - if uBit is an invalid bit
//
inline BOOL ssBitTable::Set(const UINT uBit)
{
   if (uBit >= uMaxBits)
       return FALSE;
   else
   {
      *(pBitMap+(uBit >> SHIFT_OPERAND)) |= (0x1 << (uBit & MOD_MASK));
      return TRUE;
   }
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::SetAs()
//               - set a specified bit  as specified
//
//   Parameters: 
//
//      Returns: 
//               
//
inline BOOL ssBitTable::SetAs(const UINT uBit, const BOOL bValue)
{
   if (uBit >= uMaxBits)
       return FALSE;
   else
   {
      if(bValue)
         return(Set(uBit));
      else
         return(Reset(uBit));
   }
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::IsSet()
//               - check a specified bit si set
//
//   Parameters: uBit   - bit to check
//
//      Returns: FALSE  - if uBit is an invalid bit
//               TRUE   - if uBit is a valid bit and the bit is set
//               FALSE  - if uBit is a valid bit and the bit is reset
//               
inline BOOL ssBitTable::IsSet(const UINT uBit) const
{
   if (uBit >= uMaxBits)
      return FALSE;
   else
      if ((*(pBitMap+(uBit >> SHIFT_OPERAND)) & 
          ( 0x1 << (uBit & MOD_MASK))) == 0)
         return FALSE;
      else
         return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::GetSetBit()
//               - get the bit number of the first set bit from LSB
//
//   Parameters: uOffset - start searching from the one after uOffset
//                         uOffset = SS_INVALID_INDEX => start from index 0
//
//      Returns: uBit                 - first set bit number
//               SS_INVALID_INDEX - if no bit is set
//
inline UINT ssBitTable::GetSetBit(const UINT uOffset) const
{
   if(uOffset != SS_INVALID_INDEX && uOffset >= (uMaxBits-1))
      // end of bitmap or out of range
      return(SS_INVALID_INDEX);

   UINT uBitOffset;
   if(uOffset == SS_INVALID_INDEX)
      // start from very beginning
      uBitOffset = 0;
   else
      uBitOffset = uOffset + 1;

   UINT uBit;
   BOOL bIncompleteByte = TRUE;

   for (UINT lEntity = uBitOffset/BITS_IN_ENTITY; lEntity < uBitMapSize; lEntity++)
   {
      if (*(pBitMap+(lEntity)) != 0x0)
      {
         for (uBit = ((bIncompleteByte--) ? uBitOffset%BITS_IN_ENTITY : 0); uBit < BITS_IN_ENTITY; uBit++)
         {
            if (((*(pBitMap+(lEntity)) >> uBit) & 0x1) != 0x0)
            {
               //return (uBit + (lEntity << SHIFT_OPERAND));
               UINT uRetBitNum = uBit + (lEntity << SHIFT_OPERAND);
               if(uRetBitNum >= uMaxBits)
                  // out of range
                  return(SS_INVALID_INDEX);
               else
                  return(uRetBitNum);
            }
         }
      }
   }
   return(SS_INVALID_INDEX);
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::GetNonSetBit()
//               - get the bit number of the first non-set bit from LSB
//
//   Parameters: uOffset - start searching from the one after uOffset
//                         uOffset = SS_INVALID_INDEX => start from index 0
//
//      Returns: uBit                 - first non-set bit number
//               SS_INVALID_INDEX - if no bit is set
//
inline UINT ssBitTable::GetNonSetBit(const UINT uOffset) const
{
   if(uOffset != SS_INVALID_INDEX && uOffset >= (uMaxBits-1))
      // end of bitmap or out of range
      return(SS_INVALID_INDEX);

   UINT uBitOffset;
   if(uOffset == SS_INVALID_INDEX)
      // start from very beginning
      uBitOffset = 0;
   else
      uBitOffset = uOffset + 1;

   UINT uBit;
   BOOL bIncompleteByte = TRUE;

   for (UINT lEntity = uBitOffset/BITS_IN_ENTITY; lEntity < uBitMapSize; lEntity++)
   {
      if (*(pBitMap+(lEntity)) != 0xFF)
      {
         for (uBit = ((bIncompleteByte--) ? uBitOffset%BITS_IN_ENTITY : 0); uBit < BITS_IN_ENTITY; uBit++)
         {
            if (((*(pBitMap+(lEntity)) >> uBit) & 0x1) == 0x0)
            {
               //return (uBit + (lEntity << SHIFT_OPERAND));
               UINT uRetBitNum = uBit + (lEntity << SHIFT_OPERAND);
               if(uRetBitNum >= uMaxBits)
                  // out of range
                  return(SS_INVALID_INDEX);
               else
                  return(uRetBitNum);
            }
         }
      }
   }
   return(SS_INVALID_INDEX);
}
/*
//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::GetBitMap()
//               - get a copy of the current bitmap in the object
//
//   Parameters: rBitMap - reference of the bitmap copy to be stored
//
//      Returns: Nil
//
inline void ssBitTable::GetBitMap(PBYTE pBuffer) const
{
   memcpy(pBuffer, pBitMap, uBitMapSize);
}
*/
//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::TestAndSet()
//               - 
//
//   Parameters: 
//
//      Returns: 
//
inline BOOL ssBitTable::TestAndSet(const UINT uBits)
{
   if(!IsSet(uBits))
   {
      Set(uBits);
      return(TRUE);
   }
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::TestAndReset()
//               - 
//
//   Parameters: 
//
//      Returns: 
//
inline BOOL ssBitTable::TestAndReset(const UINT uBits)
{
   if(IsSet(uBits))
   {
      Reset(uBits);
      return(TRUE);
   }
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////
//
//  Description: ssBitTable::operator|=()
//               - 
//
//   Parameters: 
//
//      Returns: 
//
inline ssBitTable& ssBitTable::operator|=(ssBitTable &btRHS)
{
    PBYTE pRBitMap = btRHS.pBitMap;
    PBYTE pLBitMap = pBitMap;
    for (UINT i = 0; i < uBitMapSize; ++i, pLBitMap++, pRBitMap++)
        //Copy globalbits to privatebits
        (*pLBitMap) |= (*pRBitMap);
    return *this;
}

#pragma pack(pop, INCLUDE_BITTABLE_H)
#endif
