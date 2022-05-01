/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
class CIniFile : public CObject
{
	DECLARE_DYNAMIC(CIniFile)
public:
	CIniFile();
	CIniFile(LPCTSTR szSectionName);
	~CIniFile();
	
	CString &GetSectionName();
	void	 SetSectionName(LPCTSTR szSectionName);

	void	 Read (LPCTSTR lpszEntry, short &nData, short nDefault = -1);
	void	 Write(LPCTSTR lpszEntry, short  nData);
	void	 Read (LPCTSTR lpszEntry, bool &bData, bool bDefault = false);
	void	 Write(LPCTSTR lpszEntry, bool  bData);
	void	 Read (LPCTSTR lpszEntry, long  &lData, long  lDefault = -1L);	
	void	 Write(LPCTSTR lpszEntry, long   lData);
	void	 Read (LPCTSTR lpszEntry, float &fData, float fDefault = -1.0f);	
	void	 Write(LPCTSTR lpszEntry, float  fData);
	void	 Read (LPCTSTR lpszEntry, CString &csData, CString &csDefault);
	void	 Read (LPCTSTR lpszEntry, CString &csData);
	void	 Write(LPCTSTR lpszEntry, CString  csData);
	void	 Read (LPCTSTR lpszEntry, COLORREF &color, COLORREF colorDefault = 0L);
	void	 Write(LPCTSTR lpszEntry, COLORREF color);
	void	 Read (LPCTSTR lpszEntry, CRect &rect, CRect rectDefault = CRect( 0, 0, 0, 0));
	void	 Write(LPCTSTR lpszEntry, CRect rect);
	// read a window's default rectangle and move/resize the window to that location
	void	 Read (CWnd* pWnd, LPCTSTR lpszEntry = 0);
	// write a window's rectangle
	void	 Write(CWnd* pWnd, LPCTSTR lpszEntry = 0);
private:
	long	 ConvertHexStringToLong(CString &csInput);
	CString  ConvertLongToHexString(long lInput);
	float	 ConvertHexStringToFloat(CString &csInput);
	CString  ConvertFloatToHexString(float fInput);
	UCHAR	 ConvertHexStringToChar(CString csHexString);
	CString  ConvertCharToHexString(UCHAR ch);
	CString	 UseDefault();
	
	CString	 csSectionName;
	BOOL	 m_bEntrySet;
};

/////////////////////////////////////////////////////////////////////////////
