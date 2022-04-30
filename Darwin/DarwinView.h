
// DarwinView.h : interface of the CDarwinView class
//

#pragma once


class CDarwinView : public CFormView
{
protected: // create from serialization only
	CDarwinView();
	DECLARE_DYNCREATE(CDarwinView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_DARWIN_FORM };
#endif

// Attributes
public:
	CDarwinDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CDarwinView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DarwinView.cpp
inline CDarwinDoc* CDarwinView::GetDocument() const
   { return reinterpret_cast<CDarwinDoc*>(m_pDocument); }
#endif

