
// DarwinDoc.cpp : implementation of the CDarwinDoc class
//

#include "stdafx.h"
#include "Darwin.h"
#include "DarwinDoc.h"
#include "IniFile.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDarwinDoc, CDocument)

BEGIN_MESSAGE_MAP(CDarwinDoc, CDocument)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CDarwinDoc::CDarwinDoc()
{
	// TODO: add one-time construction code here

}

/////////////////////////////////////////////////////////////////////////////
CDarwinDoc::~CDarwinDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDarwinDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CDarwinDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CDarwinDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDarwinDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDarwinDoc commands
BOOL CDarwinDoc::OnOpenDocument( LPCTSTR pszPathName )
{
	if ( IsModified() )
		TRACE0( "Warning: OnOpenDocument replaces an unsaved document\n" );
	bool bOK = false;
	CFile file;
	CFileException fe;
	if 
	( 
		!file.Open
		( 
			pszPathName, CFile::modeRead | CFile::shareExclusive, &fe 
		) 
	)
	{
		ReportSaveLoadException
		( 
			pszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC 
		);
		return FALSE;
	}
	file.Close();

	DeleteContents();
	SetModifiedFlag( TRUE ); // dirty during deserialize

	CStdioFile f;
	TRY
	{ 
		BeginWaitCursor();
		if 
		( 
			f.Open( pszPathName, CFile::modeRead | CFile::shareExclusive ) 
		)
		{
			if ( m_Universe.Read( f ) )
			{
				bOK = true;
			}
			else
			{
				f.Close();
				EndWaitCursor();
				return FALSE;
			}
		}
		else
		{
			EndWaitCursor();
			return FALSE;
		}
	}
	CATCH_ALL( e )
	{
		f.Close();
		DeleteContents(); // remove failed contents
		UpdateAllViews( NULL );
		EndWaitCursor();

		TRY
			ReportSaveLoadException
			( 
				pszPathName, e, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC 
			);
		END_TRY
			return FALSE;
	}
	END_CATCH_ALL

		EndWaitCursor();
	UpdateAllViews( NULL );
	SetModifiedFlag( FALSE ); // start off with unmodified
	return ( bOK == true );
} // OnOpenDocument

/////////////////////////////////////////////////////////////////////////////
BOOL CDarwinDoc::OnSaveDocument( LPCTSTR pszPathName )
{
	CFile file;
	CFileException fe;
	if 
	( 
		!file.Open
		( 
			pszPathName, 
			CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, 
			&fe 
		) 
	)
	{
		ReportSaveLoadException
		( 
			pszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_SAVE_DOC 
		);
		return FALSE;
	}
	file.Close();

	CStdioFile f;
	TRY
	{ 
		BeginWaitCursor();
		if 
		( 
			f.Open
			( 
				pszPathName, 
				CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive 
			) 
		)
		{
			m_Universe.Write( f );
			f.Close();
		}
		else
		{
			AfxMessageBox( "Unable to write to file." );
			EndWaitCursor();
			return FALSE;
		}
	}
	CATCH_ALL( e )
	{
		f.Close();
		DeleteContents(); // remove failed contents
		EndWaitCursor();

		TRY
			ReportSaveLoadException
			( 
				pszPathName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC 
			);
		END_TRY
		return FALSE;
	}
	END_CATCH_ALL

	EndWaitCursor();
	SetModifiedFlag( FALSE ); // back to unmodified

	CIniFile ini( "Settings" );
	ini.Write( "Clones", (long)m_Universe.GetNumberOfClones() );
	ini.Write( "Gender", m_Universe.GetGender() );
	ini.Write
	( 
		"Restricted Generations", 
		(long)m_Universe.GetRestrictedGenerations() 
	);
	ini.Write( "Mutation", m_Universe.GetMutation() );
	ini.Write( "Population", (long)m_Universe.GetInitialPopulation() );
	ini.Write( "Mutation Chance", (long)m_Universe.GetMutationChance() );
	ini.Write( "Children", (long)m_Universe.GetChildren() );

	return TRUE; // success if TRUE
} // OnSaveDocument

/////////////////////////////////////////////////////////////////////////////
void CDarwinDoc::DeleteContents()
{
	CIniFile ini( "Settings" );

	bool bValue;
	ini.Read( "Gender", bValue, true ); 
	m_Universe.SetGender( bValue );

	ini.Read( "Mutation", bValue, true ); 
	m_Universe.SetMutation( bValue );

	long lValue;
	ini.Read( "Population", lValue, 100 ); 
	m_Universe.SetInitialPopulation( lValue );

	ini.Read( "Mutation Chance", lValue, 1000 ); 
	m_Universe.SetMutationChance( lValue );

	ini.Read( "Children", lValue, 3 ); 
	m_Universe.SetChildren( lValue );

	ini.Read( "Clones", lValue, 1 ); 
	m_Universe.SetNumberOfClones( (UINT)lValue );

	ini.Read( "Restricted Generations", lValue, 1 ); 
	m_Universe.SetRestrictedGenerations( (UINT)lValue );

	m_Universe.Reset();

	CDocument::DeleteContents();
} // DeleteContents

/////////////////////////////////////////////////////////////////////////////
