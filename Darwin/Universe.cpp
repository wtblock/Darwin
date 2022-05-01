// Universe.cpp: implementation of the CUniverse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "Universe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const char s_szFileType[] = "Darwin";
static const char s_szVersion[] = "Version::3";
static const char s_szComment[] = "Clear Brook High School::Ms. Keiffer's Class::Science Fair Project 2003/2004";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CUniverse::CUniverse()
{
	SetIsOK( false ); // derived classes need to set this to OK when created
	SetRestrictedGenerations(); // dis-allow marriage for one generation of relatives
	SetMutation(); // enable mutation
	SetMutationChance( 1000 ); // one chance in a 1000 to mutate
	SetGender(); // support genders
	SetNumberOfClones( 1 ); // allow one genius to be cloned into the next generation
	SetGeneration( 0 ); // start with generation 0
	SetPopulation( 0 ); // set the current population to be 0
	SetInitialPopulation( 100 ); // set the initial population to be 100
	SetChildren( 3 ); // each family can have 3 children

	// high score
	m_uHighGeneration = 0; // generation of high scorer
	m_nHighFitness = 0; // fitness of high scorer
	m_nAverageFitness = 0; // average fitness of current generation
}

//////////////////////////////////////////////////////////////////////
// default destructor
CUniverse::~CUniverse()
{	
	FreePopulations();
} // ~CUniverse

//////////////////////////////////////////////////////////////////////
// update high score data
void CUniverse::UpdateHighScoreData()
{	int nHighFitness = 0, nFemaleFitness = 0, nMaleFitness = 0;
	CString csHighSignature, csFemaleSignature, csMaleSignature;

	if ( GetFemalePopulation() > 0 )
	{	CCreature* pCreature = m_FemalePopulation.top();
		nFemaleFitness = pCreature->GetFitness();
		csFemaleSignature = pCreature->GetSignature();
	}
	
	if ( GetMalePopulation() > 0 )
	{	CCreature* pCreature = m_MalePopulation.top();
		nMaleFitness = pCreature->GetFitness();
		csMaleSignature = pCreature->GetSignature();
	}

	if ( nFemaleFitness > nMaleFitness )
	{	if ( nFemaleFitness > m_nHighFitness )
		{	m_nHighFitness = nFemaleFitness;
			m_csHighSignature = csFemaleSignature;
			m_uHighGeneration = GetGeneration();
		}
	} else
	{	if ( nMaleFitness > m_nHighFitness )
		{	m_nHighFitness = nMaleFitness;
			m_csHighSignature = csMaleSignature;
			m_uHighGeneration = GetGeneration();
		}
	}
} // UpdateHighScoreData

//////////////////////////////////////////////////////////////////////
// get high score data
void CUniverse::GetHighScoreData
(	int& nFitness, 
	UINT& uGeneration, 
	CString& csSignature 
)
{	
	UpdateHighScoreData();
	nFitness = m_nHighFitness;
	uGeneration = m_uHighGeneration;
	csSignature = m_csHighSignature;
} // GetHighScoreData

//////////////////////////////////////////////////////////////////////
// copy the male/female population to the enumeration population
void CUniverse::SetEnumeration( bool bMale )
{	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > tempPopulation;
	 
	// create temporary enumeration
	if ( bMale )
	{	while ( !m_MalePopulation.empty())
		{	CCreature* pCreature = m_MalePopulation.top();
			tempPopulation.push( pCreature );
			m_EnumPopulation.push( pCreature );
			m_MalePopulation.pop();
		}
		while ( !tempPopulation.empty())
		{	CCreature* pCreature = tempPopulation.top();
			m_MalePopulation.push( pCreature );
			tempPopulation.pop();
		}
	} else
	{	while ( !m_FemalePopulation.empty())
		{	CCreature* pCreature = m_FemalePopulation.top();
			tempPopulation.push( pCreature );
			m_EnumPopulation.push( pCreature );
			m_FemalePopulation.pop();
		}
		while ( !tempPopulation.empty())
		{	CCreature* pCreature = tempPopulation.top();
			m_FemalePopulation.push( pCreature );
			tempPopulation.pop();
		}
	}
} // SetEnumeration

//////////////////////////////////////////////////////////////////////
// parse strings in the format of "key=value"
bool CUniverse::ParseKeyValueString( CString& csLine, CString& csKey, CString& csValue )
{	bool bOK = true;
	char* pBuf = csLine.GetBuffer( csLine.GetLength() + 1 );
	char* pToken = strtok( pBuf, "=\n" );
	if ( pToken )
	{	csKey = pToken;
		pToken = strtok( NULL, "=\n" );
		if ( pToken )
		{	csValue = pToken;
		} else
		{	bOK = false;
		}
	} else
	{	bOK = false;
	}
	csLine.ReleaseBuffer();
	return bOK;
} // ParseKeyValueString

//////////////////////////////////////////////////////////////////////
// read a string value
bool CUniverse::ReadKey( CStdioFile& refFile, LPCSTR pcszKey, CString& refValue )
{	CString csLine, csKey, csValue;
	if ( !refFile.ReadString( csLine ))
	{	return UnexpectedEOF();
	}
	if ( ParseKeyValueString( csLine, csKey, csValue ))
	{	if ( csKey == pcszKey )
		{	refValue = csValue;
			return true;
		} else
		{	CString csMsg;
			csMsg.Format( "Error reading key: %s, %s", pcszKey, csLine );
			AfxMessageBox( csMsg );
			return false;
		}
	}
	return true;
} // ReadKey

//////////////////////////////////////////////////////////////////////
// read a long value
bool CUniverse::ReadKey( CStdioFile& refFile, LPCSTR pcszKey, long& refValue )
{	CString csValue;
	if ( !ReadKey( refFile, pcszKey, csValue ))
		return false;
	refValue = atol( csValue );
	return true;
} // ReadKey

//////////////////////////////////////////////////////////////////////
// read a bool value
bool CUniverse::ReadKey( CStdioFile& refFile, LPCSTR pcszKey, bool& refValue )
{	long lValue;
	if ( !ReadKey( refFile, pcszKey, lValue ))
		return false;
	refValue = ( lValue != 0 );
	return true;
} // ReadKey

//////////////////////////////////////////////////////////////////////
// write the average fitness to text file
void CUniverse::ReadAverageFitness( CStdioFile& refFile )
{	
	CString csValue, csLine;

	ResetAverageFitness();
	
	if ( !refFile.ReadString( csLine ))
		return;
	
	while ( refFile.ReadString( csLine ))
	{	char* pBuf = csLine.GetBuffer( csLine.GetLength() + 1 );
		char* pToken = strtok( pBuf, "," );
		while ( pToken )
		{	int nFitness = atol( pToken );
			m_arrFitness.push_back( nFitness );
			pToken = strtok( NULL, "," );
		}
		csLine.ReleaseBuffer();
	}
	int nSize = m_arrFitness.size();
	if ( nSize > 0 )
	{	m_nAverageFitness = m_arrFitness[ nSize - 1 ];
	}
} // ReadAverageFitness

//////////////////////////////////////////////////////////////////////
// read the universe from a text file
bool CUniverse::Read( CStdioFile& refFile )
{	CString csLine, csText, csValue;
	
	// read the file identification information first
	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	csLine.TrimRight( "\n" );
	csValue = GetIdentifierString();
	csText.Format( "%s::%s", s_szFileType, csValue );
	if ( csLine != csText )
	{	AfxMessageBox( "Incorrect Darwin Identification" );
		return false;
	}
	// read the file version
	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	csLine.TrimRight( "\n" );
	if ( csLine != s_szVersion )
	{	AfxMessageBox( "Incorrect Darwin File Version" );
		return false;
	}
	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	
	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	m_dateStart.ParseDateTime( csLine );

	long lDays, lHours, lMinutes, lSeconds;
	if ( !ReadKey( refFile, "DAYS", lDays ))
		return false;
	if ( !ReadKey( refFile, "HOURS", lHours ))
		return false;
	if ( !ReadKey( refFile, "MINUTES", lMinutes ))
		return false;
	if ( !ReadKey( refFile, "SECONDS", lSeconds ))
		return false;
	m_timeSpan.SetDateTimeSpan( lDays, lHours, lMinutes, lSeconds );
	
	// boolean conditions
	bool bValue;
	if ( !ReadKey( refFile, "GENDER", bValue ))
		return false;
	SetGender( bValue );
	if ( !ReadKey( refFile, "MUTATION", bValue ))
		return false;
	SetMutation( bValue );

	// numerical conditions
	long lValue;
	if ( !ReadKey( refFile, "CHILDREN", lValue ))
		return false;
	SetChildren( lValue );
	if ( !ReadKey( refFile, "GENERATION", lValue ))
		return false;
	SetGeneration( lValue );
	if ( !ReadKey( refFile, "MUTATION CHANCE", lValue ))
		return false;
	SetMutationChance( lValue );
	if ( !ReadKey( refFile, "INITIAL POPULATION", lValue ))
		return false;
	SetInitialPopulation( lValue );
	if ( !ReadKey( refFile, "POPULATION", lValue ))
		return false;
	SetPopulation( lValue );
	if ( !ReadKey( refFile, "CLONE", lValue ))
		return false;
	SetNumberOfClones( (UINT)lValue );
	if ( !ReadKey( refFile, "RESTRICTED GENERATIONS", lValue ))
		return false;
	SetRestrictedGenerations( (UINT)lValue );

	if ( !refFile.ReadString( csLine ))
		return UnexpectedEOF();
	FreePopulations();

	// derived classes need to read creature specific data

	return true;	
} // Read

//////////////////////////////////////////////////////////////////////
// print a line of text and return new y co-ordinate
int CUniverse::PrintLine( CDC* pDC, int nX, int nY, const CString& csLine )
{
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	int nLineHeight = tm.tmHeight + tm.tmExternalLeading;
	
	pDC->TextOut( nX, nY, csLine );
	nY += nLineHeight;

	return nY;
} // PrintLine

//////////////////////////////////////////////////////////////////////
// print the state of the universe
void CUniverse::Print( CDC* pDC )
{
	// Populate logical font with defaults
 	LOGFONT lf;
	::GetObject( GetStockObject( SYSTEM_FONT ), sizeof( LOGFONT ), &lf );

	// customize our font and create it
	lf.lfHeight = 150; // 0.15 inches tall
	lf.lfWidth = 0;
	lf.lfUnderline = TRUE;
	strcpy( lf.lfFaceName, "Courier New" );
	CFont fontTitle, fontData;
	fontTitle.CreateFontIndirect( &lf );

	lf.lfHeight = 100;
	lf.lfWidth = 0;
	lf.lfUnderline = FALSE;
	fontData.CreateFontIndirect( &lf );
	
	// select the font
	CFont* pFontOld = pDC->SelectObject( &fontTitle );

	int nMargin = 500;
	int nX = nMargin;
	int nY = 0;

	CString csText, csValue, csBlank;

	// write the identification information first
	csValue = GetIdentifierString();
	csText.Format( "%s::%s", s_szFileType, csValue );
	nY = PrintLine( pDC, nX / 2, nY, csText );
	pDC->SelectObject( &fontData );
	nY = PrintLine( pDC, nX, nY, csBlank );

	csText.Format( "%s", s_szVersion );
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );
	csText.Format( "%s", s_szComment );
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );

	// write the universe settings
	// write the start time of the experiment
	COleDateTime dt = GetStartDateTime();
	csValue = dt.Format();
	csText.Format( "DATE=%s", csValue );
	nY = PrintLine( pDC, nX, nY, csText );

	// write the duration of the experiment
	long lDays, lHours, lMinutes, lSeconds;
	COleDateTimeSpan dtSpan = GetRunTime();
	lDays = dtSpan.GetDays();
	lHours = dtSpan.GetHours();
	lMinutes = dtSpan.GetMinutes();
	lSeconds = dtSpan.GetSeconds();
	csText.Format( "DAYS=%d", lDays );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "HOURS=%d", lHours );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "MINUTES=%d", lMinutes );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "SECONDS=%d", lSeconds );
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );

	// boolean conditions
	pDC->SelectObject( &fontTitle );
	csText = "Setup Conditions";
	nY = PrintLine( pDC, nX / 2, nY, csText );
	pDC->SelectObject( &fontData );
	nY = PrintLine( pDC, nX, nY, csBlank );

	bool bValue = GetGender();
	csText.Format( "GENDER=%d", bValue ? 1 : 0 );
	nY = PrintLine( pDC, nX, nY, csText );
	bValue = GetMutation();
	csText.Format( "MUTATION=%d", bValue ? 1 : 0 );
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );

	// numerical conditions
	UINT uValue = GetChildren();
	csText.Format( "CHILDREN=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	uValue = GetGeneration();
	csText.Format( "GENERATION=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	uValue = GetMutationChance();
	csText.Format( "MUTATION CHANCE=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	uValue = GetInitialPopulation();
	csText.Format( "INITIAL POPULATION=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	uValue = GetPopulation();
	csText.Format( "POPULATION=%d", uValue );
	uValue = GetNumberOfClones();
	csText.Format( "CLONE=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	uValue = GetRestrictedGenerations();
	csText.Format( "RESTRICTED GENERATIONS=%d", uValue );
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );

	// results
	pDC->SelectObject( &fontTitle );
	csText = "Results";
	nY = PrintLine( pDC, nX / 2, nY, csText );
	pDC->SelectObject( &fontData );
	nY = PrintLine( pDC, nX, nY, csBlank );

	int nFitness;
	UINT uGeneration;
	CString csSignature;
	GetHighScoreData( nFitness, uGeneration, csSignature );
	csText.Format( "FITNESS=%d", nFitness );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "GENERATION=%u", uGeneration );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "SIGNATURE=%s", csSignature );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "AVERAGE FITNESS=%d", m_nAverageFitness );
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "FEMALES=%u", GetFemalePopulation());
	nY = PrintLine( pDC, nX, nY, csText );
	csText.Format( "MALES=%u", GetMalePopulation());
	nY = PrintLine( pDC, nX, nY, csText );
	nY = PrintLine( pDC, nX, nY, csBlank );

	// graph
	pDC->SelectObject( &fontTitle );
	csText = "Graph of Average Fitness per Generation";
	nY = PrintLine( pDC, nX / 2, nY, csText );
	pDC->SelectObject( &fontData );
	nY = PrintLine( pDC, nX, nY, csBlank );

	int nTop = nY, nBottom = nY + 2000, nLeft = nX, nRight = nLeft + 4000;
	int nWidth = nRight - nLeft;
	int nHeight = nBottom - nTop;
	CPen penAxis( PS_SOLID, 20, RGB( 0, 0, 0 ));
	CPen penData( PS_SOLID, 20, RGB( 0, 0, 0 ));
	CPen penGridHeavy( PS_INSIDEFRAME, 15, RGB( 0x80, 0x80, 0x80 ));
	CPen penGridLight( PS_INSIDEFRAME, 10, RGB( 0xC0, 0xC0, 0xC0 ));
	CPen* pPenOld = pDC->SelectObject( &penAxis );

	// draw axis
	pDC->MoveTo( nLeft, nTop );
	pDC->LineTo( nLeft, nBottom );
	pDC->LineTo( nRight, nBottom );

	// draw light grid
	int nLine, nLines = 8, nSpacing = nHeight / nLines;
	pDC->SelectObject( &penGridLight );
	nY = nTop;
	for ( nLine = 0; nLine < nLines; nLine++ )
	{	pDC->MoveTo( nLeft, nY );
		pDC->LineTo( nRight, nY );
		nY += nSpacing;
	}
	
	nX = nLeft + nSpacing;
	while ( nX <= nRight )
	{	pDC->MoveTo( nX, nTop );
		pDC->LineTo( nX, nBottom );
		nX += nSpacing;	
	}

	// draw heavy grid
	nLines = 4, nSpacing = nHeight / nLines;
	pDC->SelectObject( &penGridHeavy );
	nY = nBottom - nSpacing;
	for ( nLine = 0; nLine < nLines; nLine++ )
	{	pDC->MoveTo( nLeft, nY );
		pDC->LineTo( nRight, nY );
		nY -= nSpacing;
	}
	
	nX = nLeft + nSpacing;
	while ( nX <= nRight )
	{	pDC->MoveTo( nX, nTop );
		pDC->LineTo( nX, nBottom );
		nX += nSpacing;	
	}

	// vertical axis label
	pDC->SelectObject( &penGridHeavy );
	nY = nBottom - nSpacing;
	nX = nMargin / 2;
	int nValue = 25;
	for ( nLine = 0; nLine < nLines; nLine++ )
	{	csValue.Format( "%3d%%", nValue );
		pDC->TextOut( nX, nY, csValue );
		nY -= nSpacing;
		nValue += 25;
	}

	// horizontal label
	csValue = "-- Generations -->";
	nY = nBottom + 100;
	nX = nLeft + nMargin;
	pDC->TextOut( nX, nY, csValue );

	vector<CPoint> arrPoints;

	int nPoint, nPoints = m_arrFitness.size();
	double dFitness = nFitness;
	if ( nPoints > 0 )
	{
		nSpacing = nWidth / nPoints;
		nX = nLeft;
		for ( nPoint = 0; nPoint < nPoints; nPoint++ )
		{	double dValue = m_arrFitness[ nPoint ];
			double dRatio = dValue / dFitness;
			double dHeight = nHeight * dRatio;
			CPoint pt( nX, nBottom - int( dHeight ));
			arrPoints.push_back( pt );
			nX += nSpacing;
		}
		pDC->SelectObject( &penData );
		pDC->Polyline( &arrPoints[ 0 ], nPoints );
	}

	// clean up the device context
	pDC->SelectObject( pFontOld );
	pDC->SelectObject( pPenOld );
} // Print

//////////////////////////////////////////////////////////////////////
// write the average fitness to text file
void CUniverse::WriteAverageFitness( CStdioFile& refFile )
{
	CString csValue, csLine;
	int nFitness, nGeneration, nGenerations = m_arrFitness.size();

	refFile.WriteString( "AVERAGE FITNESS\n" );

	for ( nGeneration = 0; nGeneration < nGenerations; nGeneration++ )
	{	nFitness = m_arrFitness[ nGeneration ];
		
		if (( nGeneration + 1 ) % 10 == 0 )
		{	csValue.Format( "%6d\n", nFitness );
			csLine += csValue;
			refFile.WriteString( csLine );	
			csLine.Empty();
		} else
		{	csValue.Format( "%6d,", nFitness );
			csLine += csValue;
		}	
	}
	if ( !csLine.IsEmpty())
	{	csLine.TrimRight( "," );
		csLine += "\n";
		refFile.WriteString( csLine );
	}

} // WriteAverageFitness

//////////////////////////////////////////////////////////////////////
// write the universe to a text file
void CUniverse::Write( CStdioFile& refFile )
{
	CString csText, csValue;

	// write the file identification information first
	csValue = GetIdentifierString();
	csText.Format( "%s::%s\n", s_szFileType, csValue );
	refFile.WriteString( csText );
	csText.Format( "%s\n", s_szVersion );
	refFile.WriteString( csText );
	csText.Format( "%s\n", s_szComment );
	refFile.WriteString( csText );
	refFile.WriteString( "\n" );

	// write the universe settings
	// write the start time of the experiment
	COleDateTime dt = GetStartDateTime();
	csValue = dt.Format();
	csText.Format( "DATE=%s\n", csValue );
	refFile.WriteString( csText );

	// write the duration of the experiment
	long lDays, lHours, lMinutes, lSeconds;
	COleDateTimeSpan dtSpan = GetRunTime();
	lDays = dtSpan.GetDays();
	lHours = dtSpan.GetHours();
	lMinutes = dtSpan.GetMinutes();
	lSeconds = dtSpan.GetSeconds();
	csText.Format( "DAYS=%d\n", lDays );
	refFile.WriteString( csText );
	csText.Format( "HOURS=%d\n", lHours );
	refFile.WriteString( csText );
	csText.Format( "MINUTES=%d\n", lMinutes );
	refFile.WriteString( csText );
	csText.Format( "SECONDS=%d\n", lSeconds );
	refFile.WriteString( csText );

	// boolean conditions
	bool bValue = GetGender();
	csText.Format( "GENDER=%d\n", bValue ? 1 : 0 );
	refFile.WriteString( csText );
	bValue = GetMutation();
	csText.Format( "MUTATION=%d\n", bValue ? 1 : 0 );
	refFile.WriteString( csText );

	// numerical conditions
	UINT uValue = GetChildren();
	csText.Format( "CHILDREN=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetGeneration();
	csText.Format( "GENERATION=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetMutationChance();
	csText.Format( "MUTATION CHANCE=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetInitialPopulation();
	csText.Format( "INITIAL POPULATION=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetPopulation();
	csText.Format( "POPULATION=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetNumberOfClones();
	csText.Format( "CLONE=%d\n", uValue );
	refFile.WriteString( csText );
	uValue = GetRestrictedGenerations();
	csText.Format( "RESTRICTED GENERATIONS=%d\n", uValue );
	refFile.WriteString( csText );

	refFile.WriteString( "\n" );

	// derived classes need to write creature specific data

} // Write

//////////////////////////////////////////////////////////////////////

