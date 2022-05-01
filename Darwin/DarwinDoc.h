
// DarwinDoc.h : interface of the CDarwinDoc class
//


#pragma once
#include "UniverseBoolean.h"

/////////////////////////////////////////////////////////////////////////////
class CDarwinDoc : public CDocument
{
protected: // create from serialization only
	CDarwinDoc();
	DECLARE_DYNCREATE(CDarwinDoc)

// Attributes
public:
	// the universe is where all of our creatures live
	CUniverseBoolean m_Universe;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument( LPCTSTR lpszPathName );
	virtual BOOL OnSaveDocument( LPCTSTR lpszPathName );
	virtual void DeleteContents();

// Implementation
public:
	virtual ~CDarwinDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

};
