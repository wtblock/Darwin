
// DarwinView.h : interface of the CDarwinView class
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
class CDarwinView : public CFormView
{
protected: // create from serialization only
	CDarwinView();
	DECLARE_DYNCREATE(CDarwinView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_DARWIN_FORM };
#endif
	BOOL	m_bGender;
	BOOL	m_bMutation;
	UINT	m_uPopulation;
	UINT	m_uGeneration;
	UINT	m_uHighGeneration;
	CString	m_csRunTime;
	int		m_nHighScore;
	CString	m_csSignature;
	UINT	m_uInitialPopulation;
	UINT	m_uMutationChance;
	UINT	m_uChildren;
	int		m_nAvgFitness;
	UINT	m_uNumClones;
	UINT	m_uRestrictedGenerations;
	UINT	m_uRetrys;

// Attributes
public:
	CDarwinDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC( CDC* pDC, CPrintInfo* pInfo = NULL );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );

// Implementation
public:
	// run the simulation
	void Run();

	// default destructor
	virtual ~CDarwinView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void EnableControls( bool bEnable = true );
	void DisplayUpdate();
	bool m_bRunning;
	bool m_bPaused;
	bool m_bSingleStep;
	bool m_bSlowMotion;

// Generated message map functions
protected:
	afx_msg void OnRun();
	afx_msg void OnUpdateRun( CCmdUI* pCmdUI );
	afx_msg void OnEnd();
	afx_msg void OnUpdateEnd( CCmdUI* pCmdUI );
	afx_msg void OnPause();
	afx_msg void OnUpdatePause( CCmdUI* pCmdUI );
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnSingleStep();
	afx_msg void OnUpdateSingleStep( CCmdUI* pCmdUI );
	afx_msg void OnSlowMotion();
	afx_msg void OnUpdateSlowMotion( CCmdUI* pCmdUI );
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DarwinView.cpp
inline CDarwinDoc* CDarwinView::GetDocument() const
   { return reinterpret_cast<CDarwinDoc*>(m_pDocument); }
#endif

