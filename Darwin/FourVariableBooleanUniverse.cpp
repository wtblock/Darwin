// FourVariableBooleanUniverse.cpp: implementation of the CFourVariableBooleanUniverse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "FourVariableBooleanUniverse.h"
#include "FourVariableBooleanEnvironment.h"
#include "FourVariableBooleanCreature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CFourVariableBooleanUniverse::CFourVariableBooleanUniverse()
{

}

//////////////////////////////////////////////////////////////////////
// default destructor
CFourVariableBooleanUniverse::~CFourVariableBooleanUniverse()
{

}

//////////////////////////////////////////////////////////////////////
// build the next generation
// this is a pure virtual function because it depends on the
// type of creatures living in this universe and the type of 
// environment they are required to survive
void CFourVariableBooleanUniverse::NextGeneration()
{	// these queues will become the new generation
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > NextFemaleGeneration;
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > NextMaleGeneration;
	
	// temporary holding for sisters
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > SisterQueue;
	UINT uNewPopulation = 0;
	
	CFourVariableBooleanEnvironment env;
	int nMales = GetMalePopulation();
	int nFemales = GetFemalePopulation();
	ResetAverageFitness();

	// clone the geniuses
	UINT uClone, uClones = GetNumberOfClones();
	std::vector<CCreature*> arrMales;
	std::vector<CCreature*> arrFemales;
	for ( uClone = 0; uClone < uClones; uClone++ )
	{	CFourVariableBooleanCreature* pMother = 0;
		CFourVariableBooleanCreature* pFather = 0;

		if ( nFemales > 0 )
		{	pMother = (CFourVariableBooleanCreature*)m_FemalePopulation.top();
			m_FemalePopulation.pop(); // remove from female population
			nFemales--;
		} else
		{	break;
		}
		
		if ( m_bGender ) // gender is supported?
		{	if ( nMales > 0 )
			{	pFather = (CFourVariableBooleanCreature*)m_MalePopulation.top();	
				m_MalePopulation.pop(); // remove from male population
				nMales--;
			} else
			{	delete pMother;
				break;
			}
		} else // all parents are female
		{	if ( nFemales > 0 )
			{	pFather = (CFourVariableBooleanCreature*)m_FemalePopulation.top();
				m_FemalePopulation.pop(); // remove from female population
				nFemales--;
			} else
			{	delete pMother;
				break;
			}
		}

		// determine if mother or father is most fit
		if ( pFather->GetFitness() > pMother->GetFitness())	// clone the father
		{	CFourVariableBooleanCreature* pClone = new CFourVariableBooleanCreature( pFather );
			// see how well the creature can survive the given environment
			int nFitness = pClone->Survive( env );
			// keep running average of the population's fitness
			// the fitness value is averaged in and population is
			// incremented
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
		{	CFourVariableBooleanCreature* pClone = new CFourVariableBooleanCreature( pMother );
			// see how well the creature can survive the given environment
			int nFitness = pClone->Survive( env );
			// keep running average of the population's fitness
			// the fitness value is averaged in and population is
			// incremented
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

	nMales = GetMalePopulation();
	nFemales = GetFemalePopulation();
	while ( uNewPopulation < m_uInitialPopulation )
	{	CFourVariableBooleanCreature* pMother = 0;
		CFourVariableBooleanCreature* pFather = 0;

		if ( SisterQueue.size() > 0 ) // any sisters waiting?
		{	pMother = (CFourVariableBooleanCreature*)SisterQueue.top();
			SisterQueue.pop(); // remove from sister population
		} else // general population
		{	if ( nFemales > 0 )
			{	pMother = (CFourVariableBooleanCreature*)m_FemalePopulation.top();
				m_FemalePopulation.pop(); // remove from female population
				nFemales--;
			} else
			{	break;
			}
		}

		if ( m_bGender ) // gender is supported?
		{	if ( nMales > 0 )
			{	pFather = (CFourVariableBooleanCreature*)m_MalePopulation.top();	
				m_MalePopulation.pop(); // remove from male population
				nMales--;
			} else
			{	delete pMother;
				break;
			}
		} else // all parents are female
		{	if ( SisterQueue.size() > 0 ) // any sisters waiting?
			{	pFather = (CFourVariableBooleanCreature*)SisterQueue.top();
				SisterQueue.pop(); // remove from sister population
			} else
			{	if ( nFemales > 0 )
				{	pFather = (CFourVariableBooleanCreature*)m_FemalePopulation.top();
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
				{	pMother = (CFourVariableBooleanCreature*)m_FemalePopulation.top();
					m_FemalePopulation.pop(); // remove from female population
					nFemales--;
				} else
				{	bNoFemales = true;
					delete pFather;
					break;
				}
			}
		}
		
		if ( bNoFemales )
		{	break;
		}

		UINT uChildren = GetChildren(); // how many children can each family have
		while ( uChildren > 0 && uNewPopulation < m_uInitialPopulation )
		{	// create a new child
			CFourVariableBooleanCreature* pChild = (CFourVariableBooleanCreature*)pMother->Reproduce( *pFather );	
			
			if ( GetMutation()) // is mutation happening?
			{	UINT uChance = GetMutationChance();
				pChild->Mutate( uChance );
			}

			// see how well the creature can survive the given environment
			int nFitness = pChild->Survive( env );
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
		delete pMother;
		delete pFather;
	}
	
	// update the new population
	SetPopulation( uNewPopulation );

	// update the new generation
	SetGeneration( GetGeneration() + 1 );

	// remove the current population
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
void CFourVariableBooleanUniverse::BigBang()
{	COleDateTime dt( COleDateTime::GetCurrentTime());
	// set the start time
	SetStartDateTime( dt );
	m_timeSpan.SetDateTimeSpan( 0, 0, 0, 0 );
	m_timeSpan.SetStatus( COleDateTimeSpan::valid ); 

	CFourVariableBooleanEnvironment env;
	ResetAverageFitness();
	int nGenerations = GetRestrictedGenerations();
	while ( m_uPopulation < m_uInitialPopulation )
	{
		// spontaneously create a new creature with a 128 rung DNA and nGenerations of ancestors
		CFourVariableBooleanCreature* pCreature = new CFourVariableBooleanCreature( 128, nGenerations );
		
		// see how well the creature can survive the given environment
		int nFitness = pCreature->Survive( env );
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
void CFourVariableBooleanUniverse::UpdateAverageFitness( int nFitness, UINT& nPopulation )
{	// correct answers are multiplied by 10000
	// to position the value as follows: ##xxxx, where the ##
	// is a value of 0 to 16 (0 to 16 correct answers)
	int nCorrectAnswers = ( nFitness / 10000 );
	int nAverageCorrectAnswers = ( m_nAverageFitness / 10000 ) * nPopulation + nCorrectAnswers;
	// in an equation (f=#### + #### + ##) a term is defined as 
	// each element between the plus signs (there are three 
	// terms in the above example.
	// the number of terms is multiplied by 100 and the value
	// stored here is larger based on a fewer number of terms
	// since the maximum number of terms is 16 (2 to the 4th power
	// because there are 4 variables each of which can have 2 values), 
	// the value for the number of terms is 16 minus the actual value
	// to provide a higher fitness for fewer values: xx##xx, where
	// ## is a value between 0 and 16 (16 corresponds to 0 terms,
	// and 0 corresponds to 16 terms)
	int nTerms = ( nFitness % 10000 / 100 );	
	int nAverageTerms = ( m_nAverageFitness % 10000 / 100 ) * nPopulation + nTerms;	
	// the number of inputs used in the equation can be a maximum of 64
	// (16 terms times 4 variables each). Like the number of terms
	// we want the score to be higher for lower values, so the value 
	// stored here is 64 minus the actual value: xxxx##, where ##
	// is a value between 0 and 64. In this equation, f=#### + #### + ##
	// there are 10 inputs and 3 terms.  The value stored in the number
	// of inputs would be 54 (64 - 10).
	int nInputs = nFitness % 100;
	int nAverageInputs = ( m_nAverageFitness % 100 ) * nPopulation + nInputs;
	
	// increment the new population for the new fitness value being averaged in 
	nPopulation++;

	// calculate the new running average of each component
	nAverageCorrectAnswers /= nPopulation;
	nAverageTerms /= nPopulation;
	nAverageInputs /= nPopulation;

	// re-assemble the components into the total running average
	m_nAverageFitness = 10000 * nAverageCorrectAnswers + 100 * nAverageTerms + nAverageInputs;

} // UpdateAverageFitness

//////////////////////////////////////////////////////////////////////
// read the universe from a text file
bool CFourVariableBooleanUniverse::Read( CStdioFile& refFile )
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
	{	CFourVariableBooleanCreature* pCreature = new CFourVariableBooleanCreature;
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
	{	CFourVariableBooleanCreature* pCreature = new CFourVariableBooleanCreature;
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
void CFourVariableBooleanUniverse::Write( CStdioFile& refFile )
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


