/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "darwin.h"
#include "CreatureBoolean.h"
#include <math.h>
#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////
// constructor to randomly create creatures given the number of rungs
// in the DNA and the number of restricted generations
CCreatureBoolean::CCreatureBoolean( int nRungs, int nGenerations )
{
	USES_CONVERSION;
	
	SetNumberOfVariables();
	SetMaximumNumberOfTerms();
	SetInvertedOutput( false );
	SetOutput();

	int nRand;

	// normally the DNA rungs are assigned during reproduction
	// but at the beginning of time, the DNA's are randomly
	// assigned to the number of rungs passed in the parameter
	m_DNA.resize( nRungs );
	int nRung;
	for ( nRung = 0; nRung < nRungs; nRung++ )
	{	nRand = rand();	// get a random number
		bool bValue = (( nRand & 1 ) == 1 ); // odd numbers are true
		m_DNA[ nRung ] = bValue;

	}

	// determine the number of ancestors to keep track of and build an array
	// of names used to prevent reproduction within restricted generations
	if ( nGenerations != 0 )
	{	GUID guidName;
		OLECHAR szGuid[ 40 ];
		
		int nAncestors = 0;
		int nAncestor, nGeneration;
		// first count the number of relative based on the number of generations
		for ( nGeneration = 1; nGeneration <= nGenerations; nGeneration++ )
		{	int nValue = (int)pow( 2, nGeneration );
			nAncestors += nValue;
		}
		// add the relative's names to the creature's genealogy
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	// create an ancestor
			CoCreateGuid( &guidName );
			::StringFromGUID2( guidName, szGuid, 40 );
			CString csValue( OLE2CT( szGuid ));
			AddAncestor( csValue );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// copy constructor used for cloning creatures
CCreatureBoolean::CCreatureBoolean( CCreatureBoolean* pClone )
{
	*this = *pClone;
}

/////////////////////////////////////////////////////////////////////////////
// default destructor
CCreatureBoolean::~CCreatureBoolean()
{

}

/////////////////////////////////////////////////////////////////////////////
// Given another creature (the father), a new creature is produced.
// Combining of DNA is on a gene by gene basis and is therefore 
// Species specific.
CCreature* CCreatureBoolean::Reproduce( CCreature& refFather )
{
	// the DNA is divided into GetMaximumNumberOfTerms() * GetNumberOfGeneTypes() genes 
	// that are made up of GetNumberOfVariables() bits each
	
	// within the first GetMaximumNumberOfTerms() genes, the bits determine
	// if an input is used. (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	etc.	
	//		a used	b used	c used	d used	etc.
	
	// within the second GetMaximumNumberOfTerms() genes, the bits determine
	// if the input is inverted. (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	etc.	
	//		a inv	b inv	c inv	d inv	etc.

	// congratulations its a girl/boy!
	CCreatureBoolean* pCreatureBaby = new CCreatureBoolean;	
	pCreatureBaby->SetNumberOfVariables( GetNumberOfVariables());
	pCreatureBaby->SetMaximumNumberOfTerms( GetMaximumNumberOfTerms());
	pCreatureBaby->SetInvertedOutput( GetInvertedOutput());
	pCreatureBaby->SetOutput( GetOutput());

	const int nRungs = GetNumberOfVariables(); // number of rungs per gene
	const int nGenes = GetNumberOfGenes(); // number of genes
	const int nSize = nRungs * nGenes; // total size of DNA
	pCreatureBaby->SetLengthOfDNA( nSize );


	int nIndex; // index into DNA
	int nGene; // current gene
	int nRung; // index into current gene
	int ndx;

	for ( nGene = 0; nGene < nGenes; nGene++ )
	{	int nRand = rand();	// get a random number
		// odd number indicates the gene will come from the father
		bool bFather = (( nRand & 1 ) == 1 );
		CCreatureBoolean* pParent = this; // mother by default
		if ( bFather )
		{	pParent = (CCreatureBoolean*)(&refFather);	
		}

		// copy each rung of the current gene 
		nIndex = nGene * nRungs;
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	ndx = nIndex + nRung;
			pCreatureBaby->SetDNA( ndx, pParent->GetDNA( ndx ));
		}
	}

	// Perform some hospital bureaucracy so we can keep track
	// of relationships. For example, if we do not allow marriage
	// between relatives, we can determine if two creatures are 
	// relatives by knowing who their ancestors are.
	int nAncestors = GetNumberOfAncestors();
	if ( nAncestors > 0 )
	{	pCreatureBaby->AddAncestor( refFather.GetName());
		pCreatureBaby->AddAncestor( GetName());
		nAncestors -= 2;
		if ( nAncestors > 0 )
		{	nAncestors /= 2;
			int nAncestor;
			for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
			{	pCreatureBaby->AddAncestor( refFather.GetAncestor( nAncestor ));
				pCreatureBaby->AddAncestor( GetAncestor( nAncestor ));
			}
		}
	}
	pCreatureBaby->SetGeneration( GetGeneration() + 1 );

	return pCreatureBaby;
} // Reproduce

/////////////////////////////////////////////////////////////////////////////
// A signature is like a fingerprint, it is a unique way of identifying
// the creature, and like reproduction, it is species dependent.
// For the CCreatureBoolean, the signature is the 
// boolean equation that is generated by the DNA.
CString CCreatureBoolean::GetSignature()
{
	// the DNA is divided into GetMaximumNumberOfTerms() * GetNumberOfGeneTypes() genes 
	// that are made up of GetNumberOfVariables() bits each
	
	// within the first GetMaximumNumberOfTerms() genes, the bits determine
	// if an input is used. (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	etc.	
	//		a used	b used	c used	d used	etc.
	
	// within the second GetMaximumNumberOfTerms() genes, the bits determine
	// if the input is inverted. (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	etc.	
	//		a inv	b inv	c inv	d inv	etc.


	const int nVariables = GetNumberOfVariables(); // number of binary variables
	const int nTerms = GetMaximumNumberOfTerms(); // maximum terms in the equation
	const int nInputs = nTerms * nVariables; // total bits for use or invert genes
	const int nDnaSize = (int)m_DNA.size(); // length of a DNA
	const int nFirstChar = 'a';
	const int nOutput = GetOutput();

	ASSERT( nDnaSize == nInputs * GetNumberOfGeneTypes()); 

	int nIndex; // index into DNA
	int nTerm; // current term of the equation
	int nVariable; // current variable of the equation

	typedef map<CString, int> MAP_TERM;
	MAP_TERM mt;
	MAP_TERM::iterator pos;

	CString csTerm;
	CString csOutput; // the equation will be written into this text string
	csOutput.Format( "f%d = ", nOutput );

	if ( GetInvertedOutput())
	{	csOutput = "f' = "; // indicate the output is inverted
	}
	for ( nTerm = 0; nTerm < nTerms; nTerm++ )
	{	bool bTermIsUsed = false;
		csTerm.Empty();
		for ( nVariable = 0; nVariable < nVariables; nVariable++ )
		{	nIndex = nTerm * nVariables + nVariable;
			ASSERT( nIndex < nInputs );
			bool bUse = m_DNA[ nIndex ];
			if ( bUse )
			{	ASSERT(( nIndex + nInputs ) < nDnaSize );
				bool bInvert = m_DNA[ nIndex + nInputs ];
				bTermIsUsed = true;
				char nChar = (char)( nFirstChar + nVariable );
				csTerm += nChar;
				if ( bInvert )
				{	csTerm += "'"; // add a prime
				}
			}
		}
		if ( bTermIsUsed )
		{	mt.insert( pair<CString,int>( csTerm, nTerm )); 
		}
	}

	for ( pos = mt.begin(); pos != mt.end(); ++pos )
	{	csOutput += pos->first;
		csOutput += " + ";
	}
	mt.clear();
	csOutput.TrimRight( " +" ); // trim trailing spaces and plus signs
	return csOutput;
} // GetSignature

/////////////////////////////////////////////////////////////////////////////
// format a string to represent a DNA 
CString CCreatureBoolean::FormatDnaString()
{
	const int nRungs = GetNumberOfVariables(); // number of rungs per gene
	const int nGenes = GetNumberOfGenes(); // number of genes

	int nIndex; // index into DNA
	int nGene; // current gene
	int nRung; // index into current gene

	CString csDNA;
	for ( nGene = 0; nGene < nGenes; nGene++ )
	{	nIndex = nGene * nRungs;
		// copy each rung of the current gene 
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	if ( GetDNA( nIndex + nRung ))
			{	csDNA += "1";
			} else
			{	csDNA += "0";
			}
		}
		csDNA += " ";
	}
	return csDNA;
} // FormatDnaString

/////////////////////////////////////////////////////////////////////////////
