/////////////////////////////////////////////////////////////////////////////
//          File:: fidDatabase.cpp
// Create Author:: Sammy Ha
// Create   Date:: 15 Semptember 2000
//
//   Description:: The implementation file of the Definition.h
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

//#include <afx.h>
#include <fstream>
#include "Definition.h"
using namespace std;
using namespace NGN2;


Tfid* fidDatabase::fidArray = NULL;
bool fidDatabase::_okay = TRUE;
int fidDatabase::numFidIds = 0;
int fidDatabase::maxFidId = 0;
int fidDatabase::minFidId = 1;
long fidDatabase::rArrayCount = 0;

// -------------------------------------------------------------------------
// Default Constructor
fidDatabase::fidDatabase()
{
 if (! fidArray)
 {
	// Create the file stream of "DefTable.txt"
	char* cFidFileName = "Data\\DefTable.txt";
	ifstream gfifFile(cFidFileName, ifstream::in);
	if (!gfifFile.is_open())
	{
		char* cAnotherPath = "c:\\ngn2\\data\\DefTable.txt";
		gfifFile.clear();
		gfifFile.open(cAnotherPath, ifstream::in);
		if (!gfifFile.is_open())
		{
			_okay = FALSE;
			return;
		}
	}

	// Determine the DefTable File Stats (Min and Max Fid) - 1st Pass
	char buffer[256];
	char* bufPtr;
	Tfid tmpStore;
	int retVal;

	int		iFIDCode = 0;					// Fid Code
	int		iTypeCode;						// Data Type
	int		iPriority;						// priority
//	char	cGp[4];							// Field group
//	int		iVector;						// Vector position

	while(!gfifFile.eof())
	{
		// Skip the comment lines
		gfifFile.getline(buffer, sizeof(buffer));
		if (*buffer == '!')
			continue;
	
		bufPtr = buffer;

		// Read the FID Code and FID Name

		retVal = sscanf_s(bufPtr, " %d %s", &(iFIDCode),
											tmpStore.cFIDName,sizeof(tmpStore.cFIDName));
		if (retVal != 2)
			continue;

		// step over the FID Description field
		bufPtr = strstr(bufPtr, "\"");
		ASSERT(bufPtr != NULL);
			
		bufPtr = strstr(++bufPtr, "\"");
		ASSERT(bufPtr != NULL);

		retVal = sscanf(++bufPtr,"%d%d%", &(iTypeCode),
												&(iPriority));//,
//												(tmpStore.cRippleTo),
//												&(iVector));
		if (retVal == 1)
			iPriority = 0;
//		ASSERT(retVal == 2);
		if (retVal == EOF)
			break;

		// Cast those larger structure to the smaller one
		tmpStore.nFIDCode = (short) iFIDCode;
		tmpStore.nTypeCode = (short) iTypeCode;
		tmpStore.nPriority = (short) iPriority;
//		tmpStore.cGroup = cGp[0];
//		tmpStore.nVector = (short) iVector;

		// Get number of FIDs, max Fid, and min Fid
		numFidIds++;
		if (maxFidId < tmpStore.nFIDCode)
			maxFidId = tmpStore.nFIDCode;	

		if (minFidId > tmpStore.nFIDCode)
			minFidId = tmpStore.nFIDCode;	
	}

	if (numFidIds == 0)
	{
		_okay = FALSE;
		gfifFile.close();
		return;
	}

	// Allocate TFid Structure Array for the Database
	// The size of the array is according to the value of the minFidId
	fidArray = new Tfid[maxFidId+1];

	InterlockedIncrement(&rArrayCount);
	//rArrayCount++;      
    if (!fidArray)
	{
		_okay = FALSE;
		gfifFile.close();
		return;
	}
	memset(fidArray, 0, sizeof(Tfid)*(maxFidId+1));
	gfifFile.close();

	// Read the appendix_a File into the Database - 2nd Pass
	gfifFile.open(cFidFileName,ifstream::in);
	if (!gfifFile.is_open())
	{
		char* cAnotherPath = "c:\\ngn2\\data\\DefTable.txt";
		gfifFile.clear();
		gfifFile.open(cAnotherPath, ios::in);
		if (!gfifFile.is_open())
		{
			_okay = FALSE;
			return;
		}
	}
	
	while (!gfifFile.eof())
	{
		// Skip the comment lines
		gfifFile.getline(buffer, sizeof(buffer));
		if (*buffer == '!')
			continue;
	
		bufPtr = buffer;

		// Read the FID Code and FID Name
		retVal = sscanf(bufPtr, " %d %s", &(iFIDCode),
											tmpStore.cFIDName);
		if (retVal != 2)
			continue;

		// step over the FID Description field
		bufPtr = strstr(bufPtr, "\"");
		ASSERT(bufPtr != NULL);
			
		bufPtr = strstr(++bufPtr, "\"");
		ASSERT(bufPtr != NULL);

		retVal = sscanf(++bufPtr,"%d%d", &(iTypeCode),
												&(iPriority));
//												(cGp),
//												(tmpStore.cRippleTo),
//												&(iVector));
		if (retVal == 1)
			iPriority = 0;
//		ASSERT(retVal == 2);
		if (retVal == EOF)
			break;

		// Cast those larger structure to the smaller one
		tmpStore.nFIDCode = (short) iFIDCode;
		tmpStore.nTypeCode = (short) iTypeCode;
		tmpStore.nPriority = (short) iPriority;
//		tmpStore.cGroup = cGp[0];
//		tmpStore.nVector = (short) iVector;

		// Load Fid Info Struct
		ASSERT(tmpStore.nFIDCode <= maxFidId);
		
		Tfid* pfid;
		pfid = fidArray+tmpStore.nFIDCode;

		pfid->nFIDCode		= tmpStore.nFIDCode;		
		strcpy(pfid->cFIDName, tmpStore.cFIDName);
		pfid->nTypeCode 	= tmpStore.nTypeCode;
		pfid->nPriority		= tmpStore.nPriority;
//		pfid->cGroup		= tmpStore.cGroup;
//		strcpy(pfid->cRippleTo, tmpStore.cRippleTo);
//		pfid->nVector		= tmpStore.nVector;
	}

	gfifFile.close();
  }
  else
  {
	  InterlockedIncrement(&rArrayCount);
      //rArrayCount++;
  } 
}


// -------------------------------------------------------------------------
// Destructor
fidDatabase::~fidDatabase()
{
	long  rtn = InterlockedDecrement(&rArrayCount);
    //rArrayCount--; 
	
    if ((rtn==0)&&(fidArray))
	{
		delete[] fidArray;
		fidArray = NULL;
	}
}

// -------------------------------------------------------------------------
// Get Fid
const Tfid* fidDatabase::GetFid(short nFidCode) const
{
	ASSERT(isOkay());

	if ( nFidCode < minFidId || nFidCode > maxFidId ) 
		return NULL;
	
	
	if (nFidCode < 0) 
	{
		nFidCode = maxFidId+abs(nFidCode);
	}
	
	if ((*(fidArray+nFidCode)->cFIDName) == '\0')
	return NULL;
		
	return (fidArray+nFidCode);
}


// -------------------------------------------------------------------------
// Get Fid Name
const char* fidDatabase::GetFidName(short nFidCode) const
{
	ASSERT(isOkay());

	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{ 
		return (fidArray)->cFIDName;
	}
	
	if (nFidCode < 0) 
	{
		nFidCode = maxFidId+abs(nFidCode);
	}

	return (fidArray+nFidCode)->cFIDName;
}

// -------------------------------------------------------------------------
// Get Fid Ripple
/*const char* fidDatabase::GetFidRipple(short nFidCode) const
{
	ASSERT(isOkay());

	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{ 
		return (fidArray)->cRippleTo;
	}
	
	if (nFidCode < 0) 
	{
		nFidCode = maxFidId+abs(nFidCode);
	}

	return (fidArray+nFidCode)->cRippleTo;
}*/

// -------------------------------------------------------------------------
// Get Fid Type
short fidDatabase::GetFidType(short nFidCode) const
{
	ASSERT(isOkay());
	
	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{ 
		return (fidArray)->nTypeCode;
	}
	
	if (nFidCode < 0)
	{
		nFidCode = maxFidId+abs(nFidCode);
	}

	return (fidArray+nFidCode)->nTypeCode;
}

// -------------------------------------------------------------------------
// Get Fid Priority
short fidDatabase::GetFidPriority(short nFidCode) const
{
	ASSERT(isOkay());
	
	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{ 
		return (fidArray)->nPriority;
	}
	
	if (nFidCode < 0)
	{
		nFidCode = maxFidId+abs(nFidCode);
	}

	return (fidArray+nFidCode)->nPriority;
}

// -------------------------------------------------------------------------
// Get Fid Vector
/*short fidDatabase::GetFidVector(short nFidCode) const
{
	ASSERT(isOkay());
	
	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{
		return (fidArray)->nVector;
	}
	
	if (nFidCode < 0) 
	{
		nFidCode = maxFidId+abs(nFidCode);
	}
	
	return (fidArray+nFidCode)->nVector;
}

// -------------------------------------------------------------------------
// Get Fid Group
const char fidDatabase::GetFidGroup(short nFidCode) const
{
	ASSERT(isOkay());
	
	if (nFidCode < minFidId || nFidCode > maxFidId) 
	{
		return (fidArray)->cGroup;
	}
	
	if (nFidCode < 0) 
	{
		nFidCode = maxFidId+abs(nFidCode);
	}
	
	return (fidArray+nFidCode)->cGroup;
}

*/
