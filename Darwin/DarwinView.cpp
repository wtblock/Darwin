
// DarwinView.cpp : implementation of the CDarwinView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Darwin.h"
#endif

#include "DarwinDoc.h"
#include "DarwinView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDarwinView

IMPLEMENT_DYNCREATE(CDarwinView, CFormView)

BEGIN_MESSAGE_MAP(CDarwinView, CFormView)
END_MESSAGE_MAP()

// CDarwinView construction/destruction

CDarwinView::CDarwinView()
	: CFormView(IDD_DARWIN_FORM)
{
	// TODO: add construction code here

}

CDarwinView::~CDarwinView()
{
}

void CDarwinView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CDarwinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CDarwinView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CDarwinView diagnostics

#ifdef _DEBUG
void CDarwinView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDarwinView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDarwinDoc* CDarwinView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDarwinDoc)));
	return (CDarwinDoc*)m_pDocument;
}
#endif //_DEBUG


// CDarwinView message handlers
