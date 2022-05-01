
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "DarwinDoc.h"
#include "DarwinView.h"
#include "IniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// the run timer controls how often next generations are calculated
static const UINT s_uTimerRun = 1;
static const UINT s_uRunPeriodInMSecs = 5;
static const UINT s_uSlowPeriodInMSecs = 1000;

// the update timer is used to update the display
static const UINT s_uTimerUpdate = 2;
static const UINT s_uUpdatePeriodInMSecs = 250;

// print logical scaling is 1000 logical units per inch
static const UINT s_uMap = 1000;

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDarwinView, CFormView)

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDarwinView, CFormView)
	ON_COMMAND( IDM_RUN, OnRun )
	ON_UPDATE_COMMAND_UI( IDM_RUN, OnUpdateRun )
	ON_COMMAND( IDM_END, OnEnd )
	ON_UPDATE_COMMAND_UI( IDM_END, OnUpdateEnd )
	ON_COMMAND( IDM_PAUSE, OnPause )
	ON_UPDATE_COMMAND_UI( IDM_PAUSE, OnUpdatePause )
	ON_WM_TIMER()
	ON_COMMAND( IDM_STEP, OnSingleStep )
	ON_UPDATE_COMMAND_UI( IDM_STEP, OnUpdateSingleStep )
	ON_COMMAND( IDM_SLOW, OnSlowMotion )
	ON_UPDATE_COMMAND_UI( IDM_SLOW, OnUpdateSlowMotion )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CDarwinView::CDarwinView()
	: CFormView(IDD_DARWIN_FORM)
{
	m_bGender = FALSE;
	m_bMutation = FALSE;
	m_uPopulation = 0;
	m_uGeneration = 0;
	m_uHighGeneration = 0;
	m_csRunTime = _T( "" );
	m_nHighScore = 0;
	m_csSignature = _T( "" );
	m_uInitialPopulation = 0;
	m_uMutationChance = 0;
	m_uChildren = 0;
	m_nAvgFitness = 0;
	m_uNumClones = 0;
	m_uRestrictedGenerations = 0;
	m_uRetrys = 0;

	m_bRunning = false;
	m_bPaused = false;
	m_bSingleStep = false;
	m_bSlowMotion = false;
}

/////////////////////////////////////////////////////////////////////////////
CDarwinView::~CDarwinView()
{
}

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Check( pDX, IDC_CHECK_GENDER, m_bGender );
	DDX_Check( pDX, IDC_CHECK_MUTATION, m_bMutation );
	DDX_Text( pDX, IDC_EDIT_CURRENT_POP, m_uPopulation );
	DDX_Text( pDX, IDC_EDIT_GENERATION, m_uGeneration );
	DDX_Text( pDX, IDC_EDIT_HIGH_GEN, m_uHighGeneration );
	DDX_Text( pDX, IDC_EDIT_RUN_TIME, m_csRunTime );
	DDX_Text( pDX, IDC_EDIT_SCORE, m_nHighScore );
	DDX_Text( pDX, IDC_EDIT_SIGNATURE, m_csSignature );
	DDX_Text( pDX, IDC_EDIT_INIT_POP, m_uInitialPopulation );
	DDX_Text( pDX, IDC_EDIT_MUTATION_CHANCE, m_uMutationChance );
	DDX_Text( pDX, IDC_EDIT_CHILDREN, m_uChildren );
	DDX_Text( pDX, IDC_EDIT_AVG_FITNESS, m_nAvgFitness );
	DDX_Text( pDX, IDC_EDIT_NUM_CLONES, m_uNumClones );
	DDX_Text( pDX, IDC_EDIT_RESTRICTED, m_uRestrictedGenerations );
	DDX_Text( pDX, IDC_EDIT_RETRYS, m_uRetrys );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDarwinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	CDarwinDoc* pDoc = GetDocument();
	ASSERT( pDoc != 0 );

	m_uRestrictedGenerations = pDoc->m_Universe.GetRestrictedGenerations();
	m_uNumClones = pDoc->m_Universe.GetNumberOfClones();
	m_bGender = pDoc->m_Universe.GetGender() == true;
	m_bMutation = pDoc->m_Universe.GetMutation() == true;
	m_uPopulation = pDoc->m_Universe.GetPopulation();
	m_uGeneration = pDoc->m_Universe.GetGeneration();

	m_csRunTime = pDoc->m_Universe.GetRunTime().Format( "%H:%M:%S" );
	m_uInitialPopulation = pDoc->m_Universe.GetInitialPopulation();
	m_uMutationChance = pDoc->m_Universe.GetMutationChance();
	m_uChildren = pDoc->m_Universe.GetChildren();

	pDoc->m_Universe.GetHighScoreData
	( 
		m_nHighScore, m_uHighGeneration, m_csSignature 
	);
	m_nAvgFitness = pDoc->m_Universe.GetAverageFitness();

	UpdateData( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	UpdateData( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CDarwinView::AssertValid() const
{
	CFormView::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

/////////////////////////////////////////////////////////////////////////////
CDarwinDoc* CDarwinView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDarwinDoc)));
	return (CDarwinDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// enable or disable (gray out) setup controls to prevent changes during 
// a running experiment
void CDarwinView::EnableControls( bool bEnable )
{
	GetDlgItem( IDC_EDIT_NUM_CLONES )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_EDIT_RESTRICTED )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_CHECK_GENDER )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_CHECK_MUTATION )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_EDIT_INIT_POP )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_EDIT_MUTATION_CHANCE )->EnableWindow( bEnable == true );
	GetDlgItem( IDC_EDIT_CHILDREN )->EnableWindow( bEnable == true );
} // EnableControls

/////////////////////////////////////////////////////////////////////////////
// get all of the current running data and update the display
// so the user can see what is happening during the experiment
void CDarwinView::DisplayUpdate()
{
	CDarwinDoc* pDoc = GetDocument();
	ASSERT( pDoc != 0 );

	m_uPopulation = pDoc->m_Universe.GetPopulation();
	m_uGeneration = pDoc->m_Universe.GetGeneration();
	m_nAvgFitness = pDoc->m_Universe.GetAverageFitness();
	pDoc->m_Universe.AccumulateRunTime();
	COleDateTimeSpan dtSpan = pDoc->m_Universe.GetRunTime();
	long lDays = dtSpan.GetDays();
	long lHours = dtSpan.GetHours();
	long lMinutes = dtSpan.GetMinutes();
	long lSeconds = dtSpan.GetSeconds();
	m_csRunTime.Format( "%d %02d:%02d:%02d", lDays, lHours, lMinutes, lSeconds );

	// reset time accumulation
	pDoc->m_Universe.SetIncrementalStartDateTime(); // set beginning of accumulation
	UpdateData( FALSE );

} // DisplayUpdate

/////////////////////////////////////////////////////////////////////////////
// CDarwinView message handlers
/////////////////////////////////////////////////////////////////////////////
// Timers control when different operations will run. Darwin uses two timers,
// one to control how often a new generation is calculated and another to 
// update the display information.
void CDarwinView::OnTimer( UINT_PTR nIDEvent )
{
	CDarwinDoc* pDoc = GetDocument();
	ASSERT( pDoc != 0 );

	switch ( nIDEvent )
	{
		case s_uTimerRun: // running timer
			// nothing is happening
			if ( pDoc->m_Universe.GetStalledFitness() ) 
			{
				OnEnd();
				// this is not good enough			 
				if ( !pDoc->m_Universe.GetAcceptableFitness() ) 
				{
					pDoc->OnNewDocument();
					m_uRetrys++;
					OnInitialUpdate();
					Run();
				}
			}
			else
			{
				pDoc->m_Universe.NextGeneration();
				if ( m_bSingleStep )
				{
					OnPause();
				}
			}
			break;
		case s_uTimerUpdate: // display update timer
			pDoc->m_Universe.GetHighScoreData
			( 
				m_nHighScore, m_uHighGeneration, m_csSignature 
			);
			DisplayUpdate();
			break;
	}

	CFormView::OnTimer( nIDEvent );
} // OnTimer

/////////////////////////////////////////////////////////////////////////////
// run the simulation
void CDarwinView::Run()
{
	if ( m_bRunning )
		return;

	CDarwinDoc* pDoc = GetDocument();
	ASSERT( pDoc != 0 );

	// if there is no population, we are starting fresh
	// otherwise we are restarting a stopped experiment
	// or an experiment that has just been reloaded
	if ( pDoc->m_Universe.GetPopulation() == 0 )
	{
		UpdateData();
		pDoc->m_Universe.SetNumberOfClones( m_uNumClones );
		pDoc->m_Universe.SetGender( m_bGender == TRUE );
		pDoc->m_Universe.SetRestrictedGenerations( m_uRestrictedGenerations );
		pDoc->m_Universe.SetMutation( m_bMutation == TRUE );
		pDoc->m_Universe.SetInitialPopulation( m_uInitialPopulation );
		pDoc->m_Universe.SetChildren( m_uChildren );
		pDoc->m_Universe.SetMutationChance( m_uMutationChance );

		CIniFile ini( "Settings" );
		ini.Write( "Clones", (long)pDoc->m_Universe.GetNumberOfClones() );
		ini.Write( "Gender", pDoc->m_Universe.GetGender() );
		ini.Write
		( 
			"Restricted Generations", 
			(long)pDoc->m_Universe.GetRestrictedGenerations() 
		);
		ini.Write( "Mutation", pDoc->m_Universe.GetMutation() );
		ini.Write( "Population", (long)pDoc->m_Universe.GetInitialPopulation() );
		ini.Write( "Mutation Chance", (long)pDoc->m_Universe.GetMutationChance() );
		ini.Write( "Children", (long)pDoc->m_Universe.GetChildren() );

		pDoc->m_Universe.BigBang();
	}

	m_bPaused = false;
	m_bRunning = true;
	EnableControls( false );
	pDoc->m_Universe.SetIncrementalStartDateTime(); // mark beginning of run

	UINT uRunTime;
	if ( m_bSlowMotion )
	{
		uRunTime = s_uSlowPeriodInMSecs;
	}
	else
	{
		uRunTime = s_uRunPeriodInMSecs;
	}
	SetTimer( s_uTimerRun, uRunTime, NULL ); // running timer
	SetTimer( s_uTimerUpdate, s_uUpdatePeriodInMSecs, NULL ); // display update
	if ( m_bSingleStep )
	{
		pDoc->m_Universe.GetHighScoreData
		( 
			m_nHighScore, m_uHighGeneration, m_csSignature 
		);
		OnPause();
	}

} // Run

/////////////////////////////////////////////////////////////////////////////
// begin the simulation
void CDarwinView::OnRun()
{
	UpdateData();
	m_uRetrys = 0;
	UpdateData( FALSE );
	Run();
} // OnRun

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdateRun( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( int( m_bRunning == true ) );
} // OnUpdateRun

/////////////////////////////////////////////////////////////////////////////
// end the simulation
void CDarwinView::OnEnd()
{
	m_bPaused = false;
	m_bRunning = false;
	EnableControls();
	DisplayUpdate();
	KillTimer( s_uTimerRun );
	KillTimer( s_uTimerUpdate );

} // OnEnd

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdateEnd( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TRUE );

} // OnUpdateEnd

/////////////////////////////////////////////////////////////////////////////
// toggle between the paused state and the free running state
void CDarwinView::OnPause()
{
	CDarwinDoc* pDoc = GetDocument();
	ASSERT( pDoc != 0 );

	if ( m_bPaused == true )
	{
		m_bPaused = false;
		// mark beginning of run
		pDoc->m_Universe.SetIncrementalStartDateTime(); 

		UINT uRunTime;
		if ( m_bSlowMotion )
		{
			uRunTime = s_uSlowPeriodInMSecs;
		}
		else
		{
			uRunTime = s_uRunPeriodInMSecs;
		}

		// running timer
		SetTimer( s_uTimerRun, uRunTime, NULL );

		// display update
		SetTimer( s_uTimerUpdate, s_uUpdatePeriodInMSecs, NULL ); 
	}
	else
	{
		m_bPaused = true;
		DisplayUpdate();
		KillTimer( s_uTimerRun );
		KillTimer( s_uTimerUpdate );
	}

} // OnPause

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdatePause( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( int( m_bPaused == true ) );

} // OnUpdatePause

/////////////////////////////////////////////////////////////////////////////
// toggle between single stepping and normal free running state
// single stepping allows the user to step from one generation to the next
// and observe the changes
void CDarwinView::OnSingleStep()
{
	if ( m_bSingleStep == true )
	{
		m_bSingleStep = false;
	}
	else
	{
		m_bSingleStep = true;
	}

} // OnSingleStep

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdateSingleStep( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck( int( m_bSingleStep == true ) );

} // OnUpdateSingleStep

/////////////////////////////////////////////////////////////////////////////
// toggle between slow motion and normal free running state
// slow motion allows the user to observe the changes as they occur
void CDarwinView::OnSlowMotion()
{
	if ( m_bSlowMotion == true )
	{
		m_bSlowMotion = false;
	}
	else
	{
		m_bSlowMotion = true;
	}

} // OnSlowMotion

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnUpdateSlowMotion( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck( int( m_bSlowMotion == true ) );

} // OnUpdateSlowMotion

/////////////////////////////////////////////////////////////////////////////
void CDarwinView::OnPrepareDC( CDC* pDC, CPrintInfo* pInfo )
{
	if ( pInfo != NULL ) // printing
	{
		int nPhysicalWidth = pDC->GetDeviceCaps( HORZRES );
		int nPhysicalHeight = pDC->GetDeviceCaps( VERTRES );
		int nPixelsPerInchX = pDC->GetDeviceCaps( LOGPIXELSX );
		int nPixelsPerInchY = pDC->GetDeviceCaps( LOGPIXELSY );

		float fWidth = 
			(float)nPhysicalWidth * s_uMap / (float)nPixelsPerInchX;
		int nLogicalWidth = (int)( fWidth + 0.5f );

		// create custom MM_HIENGLISH mapping mode:
		// 	1. maintain constant aspect ratio
		//	2. accept coordinates in 0.001 inches
		//	3. keep entire page width visible at all times
		//	4. vertical dimensions increase from top to bottom
		pDC->SetMapMode( MM_ISOTROPIC );
		//	base horizontal AND VERTICAL extents on page WIDTH
		//                  ============                 =====
		pDC->SetWindowExt( nLogicalWidth, nLogicalWidth );
		//	client WIDTH
		//		   =====

		pDC->SetViewportExt( nPhysicalWidth, nPhysicalWidth );
	}

	CFormView::OnPrepareDC( pDC, pInfo );
}

/////////////////////////////////////////////////////////////////////////////
