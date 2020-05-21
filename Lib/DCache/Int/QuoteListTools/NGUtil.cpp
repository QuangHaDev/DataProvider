//$Header: /NGN2/NGN2013/Lib/DCache/Int/QuoteListTools/NGUtil.cpp 1     16-09-14 18:02 Ivan_ng $
//Copyright (C) 2000-2002 AFE Solutions Ltd

#include <afx.h>
#include "Definition.h"
#include "NGUtil.h"
#include "ssCharacterSet.h"

int CNGUtil::m_nDefaultCodePage = 950;  //traditional chinese
const int nBufferSize = 1024;

//______________________________________________________________________________

BOOL CNGUtil::ConvertUTF8ToLocalCodePage (CharArray& caUTF8, CharArray& caLocal, int nCodePage /*=0*/)
{
    if ( nCodePage == 0 ) {
        nCodePage = m_nDefaultCodePage;
    }

	char szLocal[nBufferSize];
	int nLen =  ssCharacterSet::MultiByteToMultiByte( 
        LPCTSTR(caUTF8.GetString()),    //source string
        CP_UTF8,                        //source code page
        szLocal,                        //target string buffer
        nBufferSize,                    //target string buffer length
        nCodePage );                    //target code page

	if ( nLen > 0 )	{
		caLocal.SetPos (caUTF8.GetPos());
		caLocal.SetSq (caUTF8.GetSq());
		caLocal.SetLen (nLen);
		caLocal.SetString ((byte*)szLocal);	
		return TRUE;
	}

	return FALSE;
}

BOOL CNGUtil::ConvertUTF8ToLocalCodePage (MCharArray& mcUTF8, MCharArray& mcLocal, int nCodePage /*=0*/)
{
    if ( nCodePage == 0 ) {
        nCodePage = m_nDefaultCodePage;
    }

	int ne = mcUTF8.GetNe();
	int tl = mcUTF8.GetTL();
	char szLocal[nBufferSize];
	mcLocal.SetTL( tl );
	CharArray* pCurCA;
	for (int i=0; i<ne; i++)
	{
		pCurCA = mcUTF8.GetCA(i);
	    int nLen =  ssCharacterSet::MultiByteToMultiByte( 
            LPCTSTR(pCurCA->GetString()),   //source string
            CP_UTF8,                        //source code page
            szLocal,                        //target string buffer
            nBufferSize,                    //target string buffer length
            nCodePage );                    //target code page

		if ( nLen > 0)
		{
			CharArray caLocal( szLocal, pCurCA->GetPos(), pCurCA->GetSq() );
			mcLocal.AddLine (mcUTF8.GetLn(i), &caLocal);
		}
	}

	return (mcLocal.GetNe() == mcUTF8.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToLocalCodePage (FieldLabel& flUTF8, FieldLabel& flLocal, int nCodePage /*=0*/)
{
    if ( nCodePage == 0 ) {
        nCodePage = m_nDefaultCodePage;
    }

	const int nLabelLength = 255;
	char szLabel[nLabelLength+1];
	char szLocal[nLabelLength+1];
    szLabel[nLabelLength] = 0;
    szLocal[nLabelLength] = 0;
	for (int i=0; i<flUTF8.GetNe(); i++)
	{
		if (flUTF8.GetLabel(i, szLabel, nLabelLength))
		{
	        int nLen =  ssCharacterSet::MultiByteToMultiByte( 
                szLabel,        //source string
                CP_UTF8,        //source code page
                szLocal,        //target string buffer
                nLabelLength,   //target string buffer length
                nCodePage );    //target code page

			if ( nLen > 0 )
			{
				flLocal.AddLabel (szLocal);
			}
		}
	}

	return (flUTF8.GetNe() == flLocal.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToLocalCodePage (InfoMsg& imUTF8, InfoMsg& imLocal, int nCodePage /*=0*/)
{
    if ( nCodePage == 0 ) {
        nCodePage = m_nDefaultCodePage;
    }

	imLocal.SetTL ( imUTF8.GetTL() );

	const int nLabelLength = 128;
	char szLabel[nLabelLength];
	char szLocal[nLabelLength];
    szLabel[nLabelLength] = 0;
    szLocal[nLabelLength] = 0;
	unsigned long ulPg;
	unsigned short unFd;
	for (int i=0; i<imUTF8.GetNe(); i++)
	{
		if (imUTF8.GetInfo(i, szLabel, nLabelLength, ulPg, unFd))
		{
	        int nLen =  ssCharacterSet::MultiByteToMultiByte( 
                szLabel,        //source string
                CP_UTF8,        //source code page
                szLocal,        //target string buffer
                nLabelLength,   //target string buffer length
                nCodePage );    //target code page

			if ( nLen > 0 )
			{
				imLocal.AddInfo (szLocal, ulPg, unFd);
			}
		}
	}

	return (imUTF8.GetNe() == imLocal.GetNe());
}

//______________________________________________________________________________

BOOL CNGUtil::ConvertKMBFormat (double dData, int iSignificantDigits, CString& csOutput)
{
//	double dData2 = Rounding(dData, 0);
	CString szTemp;
	szTemp.Format("%.*g",iSignificantDigits,dData);
	char * stop = NULL;
	double dData2 = strtod(szTemp.GetBuffer(szTemp.GetLength()),&stop);
	double dData3 = fabs(dData2);

	if (dData3 >= 1000000000)
	{
		csOutput.Format ("%.*gB", iSignificantDigits, dData2 / 1000000000);
	}
	else if (dData3 >= 1000000)
	{
		csOutput.Format ("%.*gM", iSignificantDigits, dData2 / 1000000);
	}
	else if (dData3 >= 10000)
	{
		csOutput.Format ("%.*gK", iSignificantDigits, dData2 / 1000);
	}
	else
	{
		csOutput.Format ("%.4g", dData);
	}

	return TRUE;
}


// OMD changed to have int64 for share/TO
BOOL CNGUtil::ConvertKMBFormat (__int64 lData, int iSignificantDigits, CString& csOutput)
{

	if( lData >= 1000000000000)
	{
		csOutput.Format ("%.*gT", iSignificantDigits,lData / pow(10.0f,12));
	} 
	else if (lData >= 1000000000)
	{
		csOutput.Format ("%.*gB", iSignificantDigits,lData / pow(10.0f,9));
	}
	else if (lData >= 1000000)
	{
		csOutput.Format ("%.*gM", iSignificantDigits,lData / pow(10.0f,6));
	}
	else if (lData >= 10000)
	{
		csOutput.Format ("%.*gK", iSignificantDigits,lData / pow(10.0f, 3));
	}
	else
	{
		csOutput.Format ("%d", lData);
	}

	return TRUE;
}



//1Mar2013 - this function only used for transaction log.
BOOL CNGUtil::ConvertKMBFormat (__int64 lData, CString& csOutput)
{
	if( lData >= 1000000000000)
	{
		csOutput.Format ("%.6gT", lData / pow(10.0f,12));
	} 
	else if (lData >= 1000000000)
	{
		csOutput.Format ("%.6gB", lData / pow(10.0f,9));
	}
	else if (lData >= 1000000)
	{
		csOutput.Format ("%.6gM", lData / pow(10.0f,6));
	}
	else if (lData >= 10000)
	{
		csOutput.Format ("%.6gK", lData / pow(10.0f, 3));
	}
	else
	{
		csOutput.Format ("%d", lData);
	}

	return TRUE;
}

//4window share order list only
BOOL CNGUtil::ConvertHFormat(unsigned long lData, CString &csOutput)
{
	if( lData >= 10000)
		csOutput.Format("99H");
	else if(lData >= 1000)
		csOutput.Format("%dH", lData/100);
	else
		csOutput.Format("%d", lData);

	return TRUE;
}

BOOL CNGUtil::Convert100HFormat(unsigned long lData, CString &csOutput)
{
	if(lData >= 100000)
		csOutput.Format("999H");
	else if(lData >= 10000)
		csOutput.Format("%dH", lData/100);
	else
		csOutput.Format("%d", lData);

	return TRUE;
}
BOOL CNGUtil::ConvertUTF8ToBIG5 (CharArray& caUTF8, CharArray& caBIG5)
{
	char szBig5[nBufferSize];
	int nBig5Length = ssCharacterSet::UTF8ToBig5 ((LPCTSTR) caUTF8.GetString(), szBig5, nBufferSize);
	if (nBig5Length > 0)
	{
		caBIG5.SetPos (caUTF8.GetPos());
		caBIG5.SetSq (caUTF8.GetSq());
		caBIG5.SetLen (nBig5Length);
		caBIG5.SetString ((byte*)szBig5);	
		return TRUE;
	}

	return FALSE;
}


BOOL CNGUtil::ConvertUTF8ToGB(CharArray &caUTF8, CharArray &caGB)
{
	char szGB[nBufferSize];
	int nGBLength = ssCharacterSet::UTF8ToGB ((LPCTSTR) caUTF8.GetString(), szGB, nBufferSize);
	if (nGBLength > 0)
	{
		caGB.SetPos (caUTF8.GetPos());
		caGB.SetSq (caUTF8.GetSq());
		caGB.SetLen (nGBLength);
		caGB.SetString ((byte*)szGB);	
		return TRUE;
	}

	return FALSE;

}

BOOL CNGUtil::ConvertUTF8ToBIG5 (MCharArray& mcUTF8, MCharArray& mcBIG5)
{
	int ne = mcUTF8.GetNe();
	int tl = mcUTF8.GetTL();
	char szBig5[nBufferSize];
	mcBIG5.SetTL (tl);
	CharArray* pCurCA;
	for (int i=0; i<ne; i++)
	{
		pCurCA = mcUTF8.GetCA(i);
		if (ssCharacterSet::UTF8ToBig5 ((LPCTSTR) pCurCA->GetString() , szBig5, nBufferSize) > 0)
		{
			CharArray caBig5 ( szBig5, pCurCA->GetPos(), pCurCA->GetSq() );
			mcBIG5.AddLine (mcUTF8.GetLn(i), &caBig5);
		}
	}

	return (mcBIG5.GetNe() == mcUTF8.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToGB(MCharArray &mcUTF8, MCharArray &mcGB)
{
	int ne = mcUTF8.GetNe();
	int tl = mcUTF8.GetTL();
	char szGB[nBufferSize];
	mcGB.SetTL (tl);
	CharArray* pCurCA;
	for (int i=0; i<ne; i++)
	{
		pCurCA = mcUTF8.GetCA(i);
		if (ssCharacterSet::UTF8ToGB ((LPCTSTR) pCurCA->GetString() , szGB, nBufferSize) > 0)
		{
			CharArray caGB ( szGB, pCurCA->GetPos(), pCurCA->GetSq() );
			mcGB.AddLine (mcUTF8.GetLn(i), &caGB);
		}
	}

	return (mcGB.GetNe() == mcUTF8.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToBIG5 (FieldLabel& flUTF8, FieldLabel& flBIG5)
{
	const int LabelLength = 255;
	char szLabel[LabelLength+1];
	char szBig5[LabelLength+1];
	for (int i=0; i<flUTF8.GetNe(); i++)
	{
		if (flUTF8.GetLabel(i, szLabel, LabelLength))
		{
			if (ssCharacterSet::UTF8ToBig5 (szLabel, szBig5, LabelLength))
			{
				flBIG5.AddLabel (szBig5);
			}
		}
	}

	return (flUTF8.GetNe() == flBIG5.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToGB(FieldLabel &flUTF8, FieldLabel &flGB)
{
	const int LabelLength = 255;
	char szLabel[LabelLength+1];
	char szGB[LabelLength+1];
	for (int i=0; i<flUTF8.GetNe(); i++)
	{
		if (flUTF8.GetLabel(i, szLabel, LabelLength))
		{
			if (ssCharacterSet::UTF8ToGB (szLabel, szGB, LabelLength))
			{
				flGB.AddLabel (szGB);
			}
		}
	}

	return (flUTF8.GetNe() == flGB.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToBIG5 (InfoMsg& imUTF8, InfoMsg& imBIG5)
{
	imBIG5.SetTL ( imUTF8.GetTL() );

	const int nLabelLength = 128;
	char pcLabel[nLabelLength];
	char szBig5[nLabelLength];
	unsigned long ulPg;
	unsigned short unFd;
	for (int i=0; i<imUTF8.GetNe(); i++)
	{
		if (imUTF8.GetInfo(i, pcLabel, nLabelLength, ulPg, unFd))
		{
			if (ssCharacterSet::UTF8ToBig5 (pcLabel, szBig5, nLabelLength))
			{
				imBIG5.AddInfo (szBig5, ulPg, unFd);
			}
		}
	}

	return (imUTF8.GetNe() == imBIG5.GetNe());
}

BOOL CNGUtil::ConvertUTF8ToGB(InfoMsg &imUTF8, InfoMsg &imGB)
{
	imGB.SetTL ( imUTF8.GetTL() );

	const int nLabelLength = 128;
	char pcLabel[nLabelLength];
	char szGB[nLabelLength];
	unsigned long ulPg;
	unsigned short unFd;
	for (int i=0; i<imUTF8.GetNe(); i++)
	{
		if (imUTF8.GetInfo(i, pcLabel, nLabelLength, ulPg, unFd))
		{
			if (ssCharacterSet::UTF8ToGB (pcLabel, szGB, nLabelLength))
			{
				imGB.AddInfo (szGB, ulPg, unFd);
			}
		}
	}

	return (imUTF8.GetNe() == imGB.GetNe());
}


BOOL CNGUtil::IsBig5Space (LPCTSTR lpcsBig5Char)
{
	return ((unsigned char) (lpcsBig5Char[0]) == 161 && lpcsBig5Char[1] == 64);
}

void CNGUtil::Big5TrimLeft (_TCHAR* szBig5String)
{
	int nStrLength = strlen (szBig5String);

	// if we're not trimming anything, we're not doing any work
	// if it is not a valid Big5 string, we're not doing any work
	if (nStrLength == 0 || (nStrLength % 2 != 0))
		return;

	_TCHAR* pNewString = new _TCHAR[nStrLength+1];
	_tcscpy (pNewString, szBig5String);

	LPCTSTR lpsz = pNewString;

	while (*lpsz != '\0')
	{
		if (!IsBig5Space (lpsz))
			break;
		lpsz += 2;
	}

	if (_tcscmp (lpsz, szBig5String))
	{
		// fix up data and length
		_tcscpy (szBig5String, lpsz);
	}

	delete pNewString;
}

//______________________________________________________________________________

BOOL CNGUtil::ConvertKMB2Double (const CString& strInput, double& dOutput)
{
    if ( strInput.IsEmpty() ) {
        return FALSE;
    }
    double dMultiplier = 1;
    int nLen = strInput.GetLength();
    char cKMB = strInput[nLen-1];
    CString strDigits = strInput.Left( nLen - 1 );
    switch ( cKMB ) {
    case 'K':
    case 'k':
        dMultiplier = 1000;
        break;
    case 'M':
    case 'm':
        dMultiplier = 1000000;
        break;
    case 'B':
    case 'b':
        dMultiplier = 1000000000;
        break;
    default:
        ASSERT( isdigit(cKMB) );
        if ( isdigit(cKMB) ) {
            strDigits = strInput;
        } else {
            return FALSE;
        }
    }
    double dBaseValue = atof( strDigits );
    dOutput = dBaseValue * dMultiplier;
    return TRUE;
}

void CNGUtil::FixWidthFormat(const double dData, const char *format, const int width, CString &csOutput, const bool isChop)
{
	CString strContentFormat;

	csOutput.Format(format, dData);

	int len = csOutput.GetLength();
	

	// find the last digit
	int last = 0;
	for(last = len-1; last >= 0; --last)
		if(isdigit(csOutput[last]))
			break;

		// find the first digit
		int first = 0;
	for(first = 0; first < len; ++first)
		if(isdigit(csOutput[first]))
			break;

	int decimal = csOutput.Find('.');

	//
	// adjsut the len
	//
	if(len > width)
	{
		// find the length for number only
		int numLen = width - (first) - (len - last - 1);
		if(decimal - first <= numLen)
		{
			CString value;
			int decimalPoint = numLen - (decimal - first) - 1;
			if(isChop || decimalPoint < 0)
				value.Format("%d", (int) dData);
			else
			{
				strContentFormat.Format("%%.%df", decimalPoint);
				
				value.Format(strContentFormat, fabs(dData));
			}

			csOutput = csOutput.Left(first) + value + csOutput.Right(len - last - 1);

			// update the last
			last = first + value.GetLength() - 1;
		}
	}

	//
	// remove sign '+' if the value is zero
	//
	if(first > 0 && csOutput[first-1] == '+')
	{
		// check whether the value is zero
		int i = 0;
		for(i = first; i <= last; ++i)
			if(csOutput[i] != '0' && csOutput[i] != '.')
				break;

		// remove the '+'
		if(i > last)
			csOutput.SetAt(first-1, ' ');
	}
}

double CNGUtil::Rounding(double Value, int Precision) {
/*	double ten_based_power = pow( 10.0, (double)(Precision) );    
	double result = Value * ten_based_power;
    
	if ( Value < 0.0f ) {
		result = fabs(result) + 0.5f;
		result = -(double)((__int64)result);	    
	} else {
		result += 0.5f;
		//result = (double)((__int64)result);	    
		result = ceil(result);
	}
	return result / ten_based_power;	*/

	CString szTemp,szFormat;

	szFormat.Format("%%.%dlf",Precision);
	szTemp.Format(szFormat,Value);
	char *buffer,*stop;
	buffer = szTemp.GetBuffer(szTemp.GetLength());
	return strtod(buffer,&stop);
}
