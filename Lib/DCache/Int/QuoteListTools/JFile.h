#ifndef JFile_H
#define JFile_H
/////////////////////////////////////////////////////////////////////////////
//          File:: JFile.h
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: JFile Class member function
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
#include "StdAfx.h"     // Contains headers which are precompiled.

//------------------------------------------------------------------------


class JFile
{
public:
	enum Mode
	{
		R,
		W,
		W_Plus,
		A_Plus,
	};

	JFile();
	bool Open(const char* filename, Mode mode);
	bool Close();
	int  Write(const char* buf, int len);

	// These functions return -1 indicating EOF,
	// else the number of bytes read.
	int Read(char* buf, int len);
	int ReadString(char*, int len);
    
	bool SaveFile(char* dir, char* filename, char* newdir, char* new_file  );
	bool FindFile(char* dir, char* file);
	bool DeleteFile( char* dir, char* file );

	~JFile();


private:

    FILE* m_fhandle;
};

//------------------------------------------------------------------------

inline JFile::JFile()
{
	m_fhandle = NULL;    
}

//------------------------------------------------------------------------

inline JFile::~JFile()
{
}

//------------------------------------------------------------------------

inline bool JFile::Close()
{
    return (fclose(m_fhandle)?false:true);
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------

#endif  // JFile_H

