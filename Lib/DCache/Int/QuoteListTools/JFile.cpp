/////////////////////////////////////////////////////////////////////////////
//          File:: JFile.cpp
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
#include "StdAfx.h"
#include "JFile.h"

//------------------------------------------------------------------------
static const char* modes[] = { "r", "w", "w+", "a+" };

bool JFile::Open(const char* filename, Mode mode)
{
	bool rv = true;

	if ( (m_fhandle = fopen(filename, modes[mode])) == NULL)
		rv = false;
	
	return rv;

}

//------------------------------------------------------------------------

int  JFile::Write(const char* buf, int len)
{
	int nwritten = 0;

    if (m_fhandle)
    {
        nwritten = fwrite(buf, sizeof(char), len, m_fhandle);
		fflush(m_fhandle);
    }

	return nwritten;

}

//------------------------------------------------------------------------

int JFile::Read(char* buf, int len)
{
	int nread = 0;

	if (m_fhandle)
	{
		if (!feof(m_fhandle))
			nread = fread(buf, sizeof(char), len, m_fhandle);
		else
			nread = -1;
	}

	return nread;
}

//------------------------------------------------------------------------

int JFile::ReadString(char* buf, int len)
{
	int nread = 0;
	
	if (m_fhandle)
	{
		if (!feof(m_fhandle))
		{
			fgets(buf, len, m_fhandle);
			nread = strlen(buf);
		}
		else
			nread = -1;
	}

	return nread;
}

//------------------------------------------------------------------------

bool JFile::SaveFile(char* dir, char* filename, char* newdir, char* new_filename  )
{
	// search file 
	char source_filename[128];
	char dest_filename[128];

	::sprintf(source_filename, "%s\\%s", dir, filename);
	::sprintf(dest_filename, "%s\\%s", newdir, new_filename);

	bool rv = true;
	if ( FindFile( dir, filename ) )
	{
		// copy files if file exists, overwrite it
		int status =  ::CopyFile( source_filename,
								 dest_filename,
								 0 );
								
		if ( status == 0 )
		{
			rv = false;
//			Tracer::Trace( COMPONENTS, ERROR_LEVEL, 
//					"Cannot copy from file %s to file %s", source_filename, dest_filename );
		}
	}
	else
	{
		rv = false;
//		Tracer::Trace( COMPONENTS, WARNING_LEVEL, 
//			"No such file %s in this directory %s", filename, dir );
	}
	return rv;
}

//------------------------------------------------------------------------
bool JFile::FindFile(char* dir, char* file)
{
	bool rv = false;
	if ( ::SearchPath(dir, file, NULL, NULL, NULL, NULL) )
	{
		rv = true;
	}
	return rv;
}
//------------------------------------------------------------------------
// This function only can delete the file specified
bool JFile::DeleteFile( char* dir, char* file )
{
	char    temp_delete_file[128];
	bool rv = true;
	::sprintf(temp_delete_file, "%s\\%s", dir, file);
    if (::DeleteFile(temp_delete_file) == 0 )
	{
		rv = false;
	}
	return rv;
}
