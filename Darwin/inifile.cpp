/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IniFile.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIniFile
IMPLEMENT_DYNAMIC(CIniFile, CObject)

/////////////////////////////////////////////////////////////////////////////
CIniFile::CIniFile(LPCTSTR szSectionName)
{
	SetSectionName(szSectionName);
}

/////////////////////////////////////////////////////////////////////////////
CIniFile::CIniFile()
{
	BOOL m_bEntrySet = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CIniFile::~CIniFile()
{
}

/////////////////////////////////////////////////////////////////////////////
CString &CIniFile::GetSectionName()
{
	return csSectionName;
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::SetSectionName(LPCTSTR szSectionName)
{
	m_bEntrySet   = TRUE;
	csSectionName = szSectionName;
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, short & nData, short nDefault)
{
	ASSERT(m_bEntrySet);

	nData = AfxGetApp()->GetProfileInt(GetSectionName(), lpszEntry, nDefault);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, short nData)
{
	ASSERT(m_bEntrySet);

	AfxGetApp()->WriteProfileInt(GetSectionName(), lpszEntry, nData);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, bool & bData, bool bDefault)
{
	short nData;

	Read( lpszEntry, nData );
	if ( nData == -1 ) 
	{	bData = bDefault;
	} else
	{	bData = ( nData != 0 );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, bool bData)
{
	short nData = bData ? 1 : 0;
	Write( lpszEntry, nData );
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, CString &csData)
{
	CString csDefault;

	Read(lpszEntry, csData, csDefault);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, CString &csData, CString &csDefault)
{
	ASSERT(m_bEntrySet);

	csData = AfxGetApp()->GetProfileString(GetSectionName(), lpszEntry, csDefault);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, CString csData)
{
	ASSERT(m_bEntrySet);

	AfxGetApp()->WriteProfileString(GetSectionName(), lpszEntry, csData);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, long &lData, long lDefault)
{
	ASSERT(m_bEntrySet);

	CString csData = AfxGetApp()->GetProfileString(GetSectionName(), lpszEntry, UseDefault());

	if (csData == UseDefault())
	{
		lData = lDefault;
		return;
	}
	lData = ConvertHexStringToLong(csData);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, long lData)
{
	ASSERT(m_bEntrySet);

	CString csWriteBuffer = ConvertLongToHexString(lData);
	
	AfxGetApp()->WriteProfileString(GetSectionName(), lpszEntry, csWriteBuffer);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, float &fData, float fDefault)
{
	ASSERT(m_bEntrySet);

	CString csData = AfxGetApp()->GetProfileString(GetSectionName(), lpszEntry, UseDefault());

	if (csData == UseDefault())
	{
		fData = fDefault;
		return;
	}
	fData = ConvertHexStringToFloat(csData);
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, float fData)
{
	ASSERT(m_bEntrySet);

	CString csWriteBuffer = ConvertFloatToHexString(fData);
	
	AfxGetApp()->WriteProfileString(GetSectionName(), lpszEntry, csWriteBuffer);
}

/////////////////////////////////////////////////////////////////////////////
CString CIniFile::UseDefault()
{
	return "Use default";
}

/////////////////////////////////////////////////////////////////////////////
long CIniFile::ConvertHexStringToLong(CString & csInput)
{
	CString csHex;
	long	lRet = 0L;
	char   *pt   = (char *) &lRet;

	ASSERT(csInput.GetLength() == sizeof(long) * 2);

	for (short nByte = 0; nByte < sizeof(long) * 2; nByte += 2)
	{
		csHex   = csInput.GetAt(nByte);
		csHex  += csInput.GetAt(nByte + 1);
		*pt		= ConvertHexStringToChar(csHex);
		++pt;
	}
	return lRet;
}

/////////////////////////////////////////////////////////////////////////////
CString CIniFile::ConvertLongToHexString(long lInput)
{
	char	*pt = (char *) &lInput;
	CString  csRet;

	for (short nByte = 0; nByte < sizeof(long); nByte++)
	{
		csRet += ConvertCharToHexString(*pt);
		++pt;
	}
	return csRet;
}

/////////////////////////////////////////////////////////////////////////////
float CIniFile::ConvertHexStringToFloat(CString &csInput)
{
	CString csHex;
	float	fRet = 0.0f;
	char   *pt = (char *) &fRet;
	int nPos = csInput.FindOneOf( ".+-" );

	if ( csInput.GetLength() != sizeof(float) * 2 || nPos != -1 )
		return (float)atof( csInput );	// assume floating point string

	for (short nByte = 0; nByte < sizeof(float) * 2; nByte += 2)
	{
		csHex   = csInput.GetAt(nByte);
		csHex  += csInput.GetAt(nByte + 1);
		*pt		= ConvertHexStringToChar(csHex);
		++pt;
	}
	return fRet;
}

/////////////////////////////////////////////////////////////////////////////
CString CIniFile::ConvertFloatToHexString(float fInput)
{
	char	*pt = (char *) &fInput;
	CString  csRet;

	for (short nByte = 0; nByte < sizeof(float); nByte++)
	{
		csRet += ConvertCharToHexString(*pt);
		++pt;
	}
	return csRet;
}
	
/////////////////////////////////////////////////////////////////////////////
CString CIniFile::ConvertCharToHexString(UCHAR ch)
{
	CString csHexString;

	csHexString.Format("%02x", ch);

	return csHexString;
}

/////////////////////////////////////////////////////////////////////////////
UCHAR CIniFile::ConvertHexStringToChar(CString csHexString)
{
	ASSERT(2 == csHexString.GetLength());
	
	char chRet = 0;
	
	csHexString.MakeUpper();

	UCHAR chFirst  = csHexString.GetAt(0);

	if (chFirst >  0x3F) // abcdef
		chFirst -= 0x37;
	else				 // 0-9
		chFirst -= 0x30;

	chRet = chFirst * 16;

	char chSecond = csHexString.GetAt(1);

	if (chSecond >  0x3F) // abcdef
		chSecond -= 0x37;
	else				 // 0-9
		chSecond -= 0x30;

	chRet += chSecond;
	return chRet;
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, COLORREF &color, COLORREF colorDefault)
{
	long lTemp;

	Read(lpszEntry, lTemp, long(colorDefault));

	color = lTemp;
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, COLORREF color)
{
	Write(lpszEntry, long(color));
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Read(LPCTSTR lpszEntry, CRect &rect, CRect rectDefault)
{
	CString csTemp, csSub;
	int nLast, nLen;
	int nRect[ 4 ] = { rectDefault.left, rectDefault.top, rectDefault.right, rectDefault.bottom };
	RECT* pRect = (RECT*)nRect;

	Read( lpszEntry, csTemp );
	if ( csTemp != "" )
	{	int i = 0;
		do
		{	nLast = csTemp.Find( ',' );
			if ( nLast == -1 ) // did not find a comma
			{	nLen = csTemp.GetLength();
				if ( nLen > 0 )
				{	nRect[ i++ ] = atoi( csTemp );
					csTemp = "";
				} else
				{	nRect[ i++ ] = 0;
				}
			} else // found a comma
			{	if ( nLast == 0 ) // leading comma
				{	csSub = "0";
				} else
				{	csSub = csTemp.Left( nLast );
				}
				nRect[ i++ ] = atoi( csSub );
				nLen = csTemp.GetLength() - nLast - 1;
				csTemp = csTemp.Right( nLen );
			}
		} while ( i < 4 && nLast != -1  );
	}
	rect = *pRect;	
}

/////////////////////////////////////////////////////////////////////////////
void CIniFile::Write(LPCTSTR lpszEntry, CRect rect)
{
	CString csTemp;
	
	csTemp.Format( "%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom );
	Write( lpszEntry, csTemp );
}

/////////////////////////////////////////////////////////////////////////////
// read a window's default rectangle and move/resize the window to that location
void CIniFile::Read( CWnd* pWnd, LPCTSTR lpszEntry /* = 0 */)
{	CRect rectDefault, rect;
	
	ASSERT( pWnd );
	ASSERT( pWnd->m_hWnd );

	pWnd->GetWindowRect( &rectDefault );
	CString csKey( "Window Rectangle" ); // default for window rectangles

	if ( lpszEntry != 0 )  // override the default
	{	csKey = lpszEntry;
	}
	Read( csKey, rect, rectDefault );
	pWnd->MoveWindow( rect );		
}

/////////////////////////////////////////////////////////////////////////////
// write a window's rectangle
void CIniFile::Write(CWnd* pWnd, LPCTSTR lpszEntry /* = 0 */)
{	CRect rect;
	
	ASSERT( pWnd );
	ASSERT( pWnd->m_hWnd );

	pWnd->GetWindowRect( &rect );
	CString csKey( "Window Rectangle" ); // default for window rectangles

	if ( lpszEntry != 0 )  // override the default
	{	csKey = lpszEntry;
	}
	Write( csKey, rect );
}

/////////////////////////////////////////////////////////////////////////////
