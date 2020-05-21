//$Header: /NGN2/NGN2013/Lib/DCache/Int/QuoteListTools/NGUtil.h 1     16-09-14 18:03 Ivan_ng $
//Copyright (C) 2000-2002 AFE Solutions Ltd

#ifndef __NGUtil_h__
#define __NGUtil_h__
class CharArray;
class MCharArray;
class FieldLabel;
class InfoMsg;

//______________________________________________________________________________

class CNGUtil
{
public:
	
	static void SetDefaultCodePage(int nCodePage);
	static int  GetDefaultCodePage();

    static BOOL ConvertUTF8ToLocalCodePage(CharArray&  caUTF8, CharArray&  caLocal, int nCodePage=0);
	static BOOL ConvertUTF8ToLocalCodePage(MCharArray& mcUTF8, MCharArray& mcLocal, int nCodePage=0);
	static BOOL ConvertUTF8ToLocalCodePage(FieldLabel& flUTF8, FieldLabel& flLocal, int nCodePage=0);
	static BOOL ConvertUTF8ToLocalCodePage(InfoMsg&    imUTF8, InfoMsg&    imLocal, int nCodePage=0);

	static BOOL ConvertUTF8ToGB(CharArray &caUTF8, CharArray &caGB);
	static BOOL ConvertUTF8ToGB(FieldLabel &flUTF8, FieldLabel &flGB);
	static BOOL ConvertUTF8ToGB(InfoMsg &imUTF8, InfoMsg &imGB);
	static BOOL ConvertUTF8ToGB(MCharArray &mcUTF8, MCharArray &mcGB);
	static BOOL ConvertKMBFormat (double dData, int iSignificantDigits, CString& csOutput);
	static BOOL ConvertKMBFormat (__int64 lData, CString& csOutput);
	static BOOL ConvertKMBFormat (__int64 lData, int iSignificantDigits, CString& csOutput);
	//static BOOL ConvertKMBFormat (double lData, CString& csOutput);
    static BOOL ConvertKMB2Double(const CString& strInput, double& dOutput);

	// convert to hundred format
	// 10H -> 1000 - 1099
	// 11H -> 1100 - 1199
	// 19H -> 1900 - 1999
	// 20H -> 2000 - 2099 ...
	static BOOL ConvertHFormat(unsigned long lData, CString &csOutput);
	// convert to hundred format
	// 100H -> 10000 - 10099
	static BOOL Convert100HFormat(unsigned long lData, CString &csOutput);

	static BOOL ConvertUTF8ToBIG5 (CharArray& caUTF8, CharArray& caBIG5);
	static BOOL ConvertUTF8ToBIG5 (MCharArray& mcUTF8, MCharArray& mcBIG5);
	static BOOL ConvertUTF8ToBIG5 (FieldLabel& flUTF8, FieldLabel& flBIG5);
	static BOOL ConvertUTF8ToBIG5 (InfoMsg& imUTF8, InfoMsg& imBIG5);

	static BOOL IsBig5Space (LPCTSTR lpcsBig5Char);
	static void Big5TrimLeft (_TCHAR* szBig5String);

	static void FixWidthFormat(const double dData, const char *format, const int width, CString &csOutput, const bool isChop = false);
	static double Rounding(double Value, int Precision);

protected:

	static int m_nDefaultCodePage;
};

inline void CNGUtil::SetDefaultCodePage (int nCodePage) { m_nDefaultCodePage = nCodePage; }
inline int  CNGUtil::GetDefaultCodePage () { return m_nDefaultCodePage; }

//______________________________________________________________________________

#endif //__NGUtil_h__
