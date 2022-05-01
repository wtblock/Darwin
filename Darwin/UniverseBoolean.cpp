// UniverseBoolean.cpp: implementation of the CUniverseBoolean class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "darwin.h"
#include "UniverseBoolean.h"
#include "CreatureBoolean.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CUniverseBoolean::CUniverseBoolean()
{
	// fate of this universe depends on the successful creation of
	// the embedded environment
	SetIsOK( m_env.GetIsOK());
}

//////////////////////////////////////////////////////////////////////
// default destructor
CUniverseBoolean::~CUniverseBoolean()
{

}

//////////////////////////////////////////////////////////////////////
// build the next generation
// this is a pure virtual function because it depends on the
// type of creatures living in this universe and the type of 
// environment they are required to survive
void CUniverseBoolean::NextGeneration()
{	// these queues will become the new generation
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > NextFemaleGeneration;
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > NextMaleGeneration;
	
	// temporary holding for sisters who were found to be related to their selected spouse
	// and were therefore rejected until an unrelated spouse becomes available.
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > SisterQueue;
	UINT uNewPopulation = 0;
	
	int nMales = GetMalePopulation();
	int nFemales = GetFemalePopulation();
	ResetAverageFitness();

	// clone the geniuses
	UINT uClone, uClones = GetNumberOfClones();
	std::vector<CCreature*> arrMales;
	std::vector<CCreature*> arrFemales;
	for ( uClone = 0; uClone < uClones; uClone++ )
	{	CCreatureBoolean* pMother = 0;
		CCreatureBoolean* pFather = 0;

		if ( nFemales > 0 )
		{	pMother = (CCreatureBoolean*)m_FemalePopulation.top();
			m_FemalePopulation.pop(); // remove from female population
			nFemales--;
		} else
		{	break;
		}
		
		if ( m_bGender ) // gender is supported?
		{	if ( nMales > 0 )
			{	pFather = (CCreatureBoolean*)m_MalePopulation.top();	
				m_MalePopulation.pop(); // remove from male population
				nMales--;
			} else
			{	delete pMother;
				break;
			}
		} else // all parents are female
		{	if ( nFemales > 0 )
			{	pFather = (CCreatureBoolean*)m_FemalePopulation.top();
				m_FemalePopulation.pop(); // remove from female population
				nFemales--;
			} else
			{	delete pMother;
				break;
			}
		}

		// determine if mother or father is most fit
		if ( pFather->GetFitness() > pMother->GetFitness())	// clone the father
		{	CCreatureBoolean* pClone = new CCreatureBoolean( pFather );
			int nFitness = pClone->GetFitness();
			UpdateAverageFitness( nFitness, uNewPopulation );

			if ( m_bGender )
			{	NextMaleGeneration.push( pClone );
				arrMales.push_back( pFather );
			} else // all female population
			{	NextFemaleGeneration.push( pClone );
				arrFemales.push_back( pFather );
			}
			m_FemalePopulation.push( pMother );	// put the mother back into the female pop
			nFemales++;
		} else // clone the mother
		{	CCreatureBoolean* pClone = new CCreatureBoolean( pMother );
			int nFitness = pClone->GetFitness();
			UpdateAverageFitness( nFitness, uNewPopulation );
			NextFemaleGeneration.push( pClone );
			arrFemales.push_back( pMother );
			if ( m_bGender )
			{	m_MalePopulation.push( pFather ); // put the father back into the male pop
				nMales++;
			} else // all females
			{	m_FemalePopulation.push( pFather );	// put the father back into the female pop
				nFemales++;
			}
		}
	}

	// put any clones into the general population
	if ( uClones > 0 )
	{	if ( m_bGender )
		{	for ( uClone = 0; uClone < arrMales.size(); uClone++ )
			{	m_MalePopulation.push( arrMales[ uClone ] );		
			}
			arrMales.clear();
		}
		for ( uClone = 0; uClone < arrFemales.size(); uClone++ )
		{	m_FemalePopulation.push( arrFemales[ uClone ] );		
		}
		arrFemales.clear();
	}

	// get the most fit pairs of parents and mate them until
	// the population level is met
	nMales = GetMalePopulation();
	nFemales = GetFemalePopulation();
	while ( uNewPopulation < m_uInitialPopulation )
	{	CCreatureBoolean* pMother = 0;
		CCreatureBoolean* pFather = 0;

		// get the most fit female to be the mother
		// sisters who were related to an earlier potential spouce have
		// been queued up for a future spouce--use them for the mother
		// first if they are available
		if ( SisterQueue.size() > 0 ) // any sisters waiting?
		{	pMother = (CCreatureBoolean*)SisterQueue.top();
			SisterQueue.pop(); // remove from sister population
		} else // general population
		{	if ( nFemales > 0 )
			{	pMother = (CCreatureBoolean*)m_FemalePopulation.top();
				m_FemalePopulation.pop(); // remove from female population
				nFemales--;
			} else
			{	break;
			}
		}

		// get the most fit male (or female if gender is not supported)
		// to be the father
		if ( m_bGender ) // gender is supported?
		{	if ( nMales > 0 )
			{	pFather = (CCreatureBoolean*)m_MalePopulation.top();	
				m_MalePopulation.pop(); // remove from male population
				nMales--;
			} else
			{	delete pMother;
				break;
			}
		} else // all parents are female
		{	// sisters who were related to an earlier potential spouce have
			// been queued up for a future spouce--use them for the father
			// first if they are available
			if ( SisterQueue.size() > 0 ) // any sisters waiting?
			{	pFather = (CCreatureBoolean*)SisterQueue.top();
				SisterQueue.pop(); // remove from sister population
			} else // general population
			{	if ( nFemales > 0 )
				{	pFather = (CCreatureBoolean*)m_FemalePopulation.top();
					m_FemalePopulation.pop(); // remove from female population
					nFemales--;
				} else
				{	delete pMother;
					break;
				}
			}
		}

		bool bNoFemales = false;
		// check to see if new mother and father are related
		if ( m_uRestrictedGenerations > 0 ) // we dont allow relatives to marry
		{	while ( pFather->GetIsRelated( *pMother ))
			{	SisterQueue.push( pMother ); // let the sister wait here
				if ( nFemales > 0 )
				{	pMother = (CCreatureBoolean*)m_FemalePopulation.top();
					m_FemalePopulation.pop(); // remove from female population
					nFemales--;
				} else
				{	bNoFemales = true;
					delete pFather;
					break;
				}
			}
		}
		
		// should not happen, but there is nothing else to do but get out
		if ( bNoFemales )
		{	break;
		}

		// The number of children is an important factor in improving the speed
		// of the genetic algorithm--if every pair can have two children, then all
		// of the population is allowed to mate (because population is fixed) and
		// there is therefore no advantage of being the most fit. On the other hand,
		// if each pair can have 8 children, only the top 25% of the population 
		// can mate--survival of the fittest at work.
		UINT uChildren = GetChildren(); // how many children can each family have
		while ( uChildren > 0 && uNewPopulation < m_uInitialPopulation )
		{	// create a new child
			CCreatureBoolean* pChild = (CCreatureBoolean*)pMother->Reproduce( *pFather );	
			
			if ( GetMutation()) // is mutation happening?
			{	UINT uChance = GetMutationChance();
				pChild->Mutate( uChance );
			}

			// see how well the creature can survive the given environment
			int nFitness = pChild->Survive( m_env );
			// keep running average of the population's fitness
			// the fitness value is averaged in and population is
			// incremented
			UpdateAverageFitness( nFitness, uNewPopulation );

			if ( m_bGender )
			{	if ( pChild->GetIsMale())
				{	NextMaleGeneration.push( pChild );
				} else
				{	NextFemaleGeneration.push( pChild );
				}
			} else // all female society
			{	NextFemaleGeneration.push( pChild );
			}
			uChildren--;
		}

		// parents are no longer needed--its a harsh universe
		delete pMother;
		delete pFather;
	}
	
	// update the new population
	SetPopulation( uNewPopulation );

	// update the new generation
	SetGeneration( GetGeneration() + 1 );

	// remove the remainder of the current population
	FreePopulations();

	// clean up sister population
	while ( !SisterQueue.empty())
	{	CCreature* pCreature = SisterQueue.top();
		delete [] pCreature;
		SisterQueue.pop();
	}

	// copy the next female generation
	while ( !NextFemaleGeneration.empty())
	{	CCreature* pCreature = NextFemaleGeneration.top();
		NextFemaleGeneration.pop();
		m_FemalePopulation.push( pCreature );
	}

	if ( m_bGender )
	{	// copy the next male generation
		while ( !NextMaleGeneration.empty())
		{	CCreature* pCreature = NextMaleGeneration.top();
			NextMaleGeneration.pop();
			m_MalePopulation.push( pCreature );
		}
	}

	// keep a record of the average fitness for each generation
	m_arrFitness.push_back( m_nAverageFitness );
} // NextGeneration

//////////////////////////////////////////////////////////////////////
// the beginning of the universe initializes the population
// this is a pure virtual function, because it depends on the 
// type of creatures living in this universe and the type of 
// environment they are required to survive
void CUniverseBoolean::BigBang()
{	COleDateTime dt( COleDateTime::GetCurrentTime());
	// set the start time
	SetStartDateTime( dt );
	m_timeSpan.SetDateTimeSpan( 0, 0, 0, 0 );
	m_timeSpan.SetStatus( COleDateTimeSpan::valid ); 

	m_nMaxCorrectAnswers = 0;
	m_uMaxCorrectAnswersGeneration = 0;

	ResetAverageFitness();
	const int nGenerations = GetRestrictedGenerations();
	const int nVariables = m_env.GetNumberOfVariables();
	const bool bInvertedOutput = m_env.GetInvertedOutput();
	const int nOutput = m_env.GetOutput();
	const int nTerms = m_env.GetMaximumNumberOfTerms();
	const int nTypesOfDNA = 2;
	const int nRungs = nVariables * nTerms * nTypesOfDNA;
	while ( m_uPopulation < m_uInitialPopulation )
	{	// spontaneously create a new creature with a nRungs of DNA and
		// nGenerations of ancestors in their geneology
		CCreatureBoolean* pCreature = new CCreatureBoolean( nRungs, nGenerations );
		pCreature->SetNumberOfVariables( nVariables );
		pCreature->SetMaximumNumberOfTerms( nTerms );
		pCreature->SetInvertedOutput( bInvertedOutput );
		pCreature->SetOutput( nOutput );
		
		// see how well the creature can survive the given environment
		int nFitness = pCreature->Survive( m_env );
		// keep running average of the population's fitness
		// the fitness value is averaged in and population is
		// incremented
		UpdateAverageFitness( nFitness, m_uPopulation );
		// add the new creature to the population
		if ( m_bGender )
		{	bool bMale = pCreature->GetIsMale();
			if ( bMale )
			{	m_MalePopulation.push( pCreature );	
			} else
			{	m_FemalePopulation.push( pCreature );
			}	
		} else // all female universe
		{	m_FemalePopulation.push( pCreature );
		} 
	}

	// keep a record of the average fitness for each generation
	m_arrFitness.push_back( m_nAverageFitness );
} // BigBang

//////////////////////////////////////////////////////////////////////
// update the average fitness given a current population and a new fitness
// since fitness is specific to the type of creature
// living in the universe, this is a pure virtual function
// that must be handled by each specific universe
// this is a running average that is recalculated after each
// new creature is born -- the new fitness value is averaged in 
// and the current population is incremented
void CUniverseBoolean::UpdateAverageFitness( int nFitness, UINT& nPopulation )
{	int nCorrectAnswerWeight = m_env.GetCorrectAnswerWeight();
	int nNumberOfTermsWeight = m_env.GetNumberOfTermsWeight();
	// correct answers are multiplied by nCorrectAnswerWeight
	// to position the value to the left of the fitness value digits as follows: 
	// # ##x xxx xxx, where the ### is the correct answers
	int nCorrectAnswers = ( nFitness / nCorrectAnswerWeight );
	int nAverageCorrectAnswers = ( m_nAverageFitness / nCorrectAnswerWeight ) * 
		nPopulation + nCorrectAnswers;
	// in an equation (f=#### + #### + ##) a term is defined as  each element 
	// between the plus signs the inverse number of terms is multiplied by 
	// nNumberOfTermsWeight to provide a higher fitness for fewer values
	int nTerms = ( nFitness % nCorrectAnswerWeight / nNumberOfTermsWeight );	
	int nAverageTerms = ( m_nAverageFitness % nCorrectAnswerWeight / 
		nNumberOfTermsWeight ) * nPopulation + nTerms;	
	// the number of inputs is unweighted and is represented as an 
	// inverse value (Max inputs - inputs used)
	int nInputs = nFitness % nNumberOfTermsWeight;
	int nAverageInputs = ( m_nAverageFitness % nNumberOfTermsWeight ) * 
		nPopulation + nInputs;
	
	// increment the new population for the new fitness value being averaged in 
	nPopulation++;

	// calculate the new running average of each component
	nAverageCorrectAnswers /= nPopulation;
	nAverageTerms /= nPopulation;
	nAverageInputs /= nPopulation;

	// re-assemble the components into the total running average
	m_nAverageFitness = nCorrectAnswerWeight * nAverageCorrectAnswers + 
		nNumberOfTermsWeight * nAverageTerms + 
		nAverageInputs;

} // UpdateAverageFitness

//////////////////////////////////////////////////////////////////////
// read the universe from a text file
bool CUniverseBoolean::Read( CStdioFile& refFile )
{	
	// do the common reads first
	if ( !CUniverse::Read( refFile ))
		return false;

	CString csValue, csLine;
	// female population
	long lFemales;
	if ( !ReadKey( refFile, "FEMALES", lFemales ))
		return false;
	while ( lFemales > 0 )
	{	CCreatureBoolean* pCreature = new CCreatureBoolean;
		pCreature->SetNumberOfVariables( m_env.GetNumberOfVariables());
		pCreature->SetMaximumNumberOfTerms( m_env.GetMaximumNumberOfTerms());
		pCreature->SetInvertedOutput( m_env.GetInvertedOutput());
		pCreature->SetOutput( m_env.GetOutput());

		CString csValue, csKey;
			
		if ( !ReadKey( refFile, "NAME", csValue ))
			return false;
		pCreature->SetName( csValue );

		long lAncestor, lAncestors;
		if ( !ReadKey( refFile, "ANCESTORS", lAncestors ))
			return false;
		pCreature->ClearAncestors();
		for ( lAncestor = 0; lAncestor < lAncestors; lAncestor++ )
		{	csKey.Format( "%03d", lAncestor );
			if ( !ReadKey( refFile, csKey, csValue ))
				return false;
			pCreature->AddAncestor( csValue );
		}

		if ( !ReadKey( refFile, "DNA", csValue ))
			return false;
		pCreature->SetDnaFromString( csValue );
		if ( !ReadKey( refFile, "SIGNATURE", csValue ))
			return false;
		long lFitness;
		if ( !ReadKey( refFile, "FITNESS", lFitness ))
			return false;
		pCreature->SetFitness( lFitness );
		pCreature->SetIsMale( false );
		pCreature->SetGeneration( GetGeneration());

		m_FemalePopulation.push( pCreature );

		if ( !refFile.ReadString( csLine ))
			return UnexpectedEOF();
		lFemales--;
	}

	// male population
	long lMales;
	if ( !ReadKey( refFile, "MALES", lMales ))
		return false;
	while ( lMales > 0 )
	{	CCreatureBoolean* pCreature = new CCreatureBoolean;
		pCreature->SetNumberOfVariables( m_env.GetNumberOfVariables());
		pCreature->SetMaximumNumberOfTerms( m_env.GetMaximumNumberOfTerms());
		pCreature->SetInvertedOutput( m_env.GetInvertedOutput());
		pCreature->SetOutput( m_env.GetOutput());

		CString csValue, csKey;
			
		if ( !ReadKey( refFile, "NAME", csValue ))
			return false;
		pCreature->SetName( csValue );
		
		long lAncestor, lAncestors;
		if ( !ReadKey( refFile, "ANCESTORS", lAncestors ))
			return false;
		pCreature->ClearAncestors();
		for ( lAncestor = 0; lAncestor < lAncestors; lAncestor++ )
		{	csKey.Format( "%03d", lAncestor );
			if ( !ReadKey( refFile, csKey, csValue ))
				return false;
			pCreature->AddAncestor( csValue );
		}

		if ( !ReadKey( refFile, "DNA", csValue ))
			return false;
		pCreature->SetDnaFromString( csValue );
		if ( !ReadKey( refFile, "SIGNATURE", csValue ))
			return false;
		long lFitness;
		if ( !ReadKey( refFile, "FITNESS", lFitness ))
			return false;
		pCreature->SetFitness( lFitness );
		pCreature->SetIsMale();
		pCreature->SetGeneration( GetGeneration());

		m_MalePopulation.push( pCreature );

		if ( !refFile.ReadString( csLine ))
			return UnexpectedEOF();
		lMales--;
	}
	
	SetPopulation( lFemales + lMales );
	ReadAverageFitness( refFile );

	return true;	
} // Read

//////////////////////////////////////////////////////////////////////
// write the universe to a text file
void CUniverseBoolean::Write( CStdioFile& refFile )
{
	// do the common writes first
	CUniverse::Write( refFile );

	UINT uValue = GetFemalePopulation();
	CString csValue, csText;
	csText.Format( "FEMALES=%d\n", uValue );
	refFile.WriteString( csText );

	SetEnumeration( false ); // enumerate females
	while ( !m_EnumPopulation.empty())
	{	CCreature* pCreature = m_EnumPopulation.top();
		m_EnumPopulation.pop();
		csValue = pCreature->GetName();
		csText.Format( "NAME=%s\n", csValue );
		refFile.WriteString( csText );

		int nAncestors = pCreature->GetNumberOfAncestors();
		csText.Format( "ANCESTORS=%d\n", nAncestors );
		refFile.WriteString( csText );
		int nAncestor;
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	csValue = pCreature->GetAncestor( nAncestor );
			csText.Format( "%03d=%s\n", nAncestor, csValue );
			refFile.WriteString( csText );
		}

		csValue = pCreature->FormatDnaString();
		csText.Format( "DNA=%s\n", csValue );
		refFile.WriteString( csText );
		csValue = pCreature->GetSignature();
		csText.Format( "SIGNATURE=%s\n", csValue );
		refFile.WriteString( csText );
		int nFitness = pCreature->GetFitness();
		csText.Format( "FITNESS=%d\n", nFitness );
		refFile.WriteString( csText );
		refFile.WriteString( "\n" );
	}

	uValue = GetMalePopulation();
	csText.Format( "MALES=%d\n", uValue );
	refFile.WriteString( csText );

	SetEnumeration( true ); // enumerate males
	while ( !m_EnumPopulation.empty())
	{	CCreature* pCreature = m_EnumPopulation.top();
		m_EnumPopulation.pop();
		csValue = pCreature->GetName();
		csText.Format( "NAME=%s\n", csValue );
		refFile.WriteString( csText );

		int nAncestors = pCreature->GetNumberOfAncestors();
		csText.Format( "ANCESTORS=%d\n", nAncestors );
		refFile.WriteString( csText );
		int nAncestor;
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	csValue = pCreature->GetAncestor( nAncestor );
			csText.Format( "%03d=%s\n", nAncestor, csValue );
			refFile.WriteString( csText );
		}
		
		csValue = pCreature->FormatDnaString();
		csText.Format( "DNA=%s\n", csValue );
		refFile.WriteString( csText );
		csValue = pCreature->GetSignature();
		csText.Format( "SIGNATURE=%s\n", csValue );
		refFile.WriteString( csText );
		int nFitness = pCreature->GetFitness();
		csText.Format( "FITNESS=%d\n", nFitness );
		refFile.WriteString( csText );
		refFile.WriteString( "\n" );
	}

	WriteAverageFitness( refFile );

} // Write

//////////////////////////////////////////////////////////////////////
