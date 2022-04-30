
// Darwin.h : main header file for the Darwin application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDarwinApp:
// See Darwin.cpp for the implementation of this class
//

class CDarwinApp : public CWinApp
{
public:
	CDarwinApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDarwinApp theApp;
