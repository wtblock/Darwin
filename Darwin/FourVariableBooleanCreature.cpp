// FourVariableBooleanCreature.cpp: implementation of the CFourVariableBooleanCreature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "FourVariableBooleanCreature.h"
#include <cmath>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CFourVariableBooleanCreature::CFourVariableBooleanCreature( int nRungs, int nGenerations )
{
	USES_CONVERSION;
	
	int nRand;

	// normally the DNA rungs are assigned during reproduction
	// but at the beginning of time, the DNA's are randomly
	// assigned to the number of rungs passed in the parameter
	int nRung;
	for ( nRung = 0; nRung < nRungs; nRung++ )
	{	nRand = rand();	// get a random number
		bool bValue = (( nRand & 1 ) == 1 ); // odd numbers are true
		m_DNA.push_back( bValue );

	}
	if ( nGenerations != 0 )
	{	GUID guidName;
		OLECHAR szGuid[ 40 ];
		
		int nAncestors = 0;
		int nAncestor, nGeneration;
		for ( nGeneration = 1; nGeneration <= nGenerations; nGeneration++ )
		{	int nValue = (int)pow( 2, nGeneration );
			nAncestors += nValue;
		}
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	// create an ancestor
			CoCreateGuid( &guidName );
			::StringFromGUID2( guidName, szGuid, 40 );
			CString csValue( OLE2CT( szGuid ));
			AddAncestor( csValue );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// defined constructor
/*
CFourVariableBooleanCreature::CFourVariableBooleanCreature
(	bool bMale, // male gender
	int nGeneration, // generation 
	int nFitness, // creature fitness
	CString& refName, // name of the creature
	CString& refFather, // name of the creature's father
	CString& refMother, // name of the creature's mother
	CString& refDNA // the creature's DNA
)
{
	// normally the DNA rungs are assigned during reproduction
	// but after reading data from a file, the DNA is assigned
	// based on the file data.
	SetIsMale( bMale );
	SetGeneration( nGeneration );
	SetFitness( nFitness );
	SetName( refName );
	SetFatherName( refFather );
	SetMotherName( refMother );
	SetDnaFromString( refDNA );
}
*/
//////////////////////////////////////////////////////////////////////
// copy constructor
CFourVariableBooleanCreature::CFourVariableBooleanCreature( CFourVariableBooleanCreature* pClone )
{
	*this = *pClone;
}

//////////////////////////////////////////////////////////////////////
// default destructor
CFourVariableBooleanCreature::~CFourVariableBooleanCreature()
{

}

//////////////////////////////////////////////////////////////////////
// Given another creature (the father), a new creature is produced.
// Combining of DNA is on a gene by gene basis and is therefore 
// Species specific.
CCreature* CFourVariableBooleanCreature::Reproduce( CCreature& refFather )
{
	// the DNA is divided into 32 genes	that are made up of 4 bits each
	
	// within the first 16 genes, the bits determine if an input is used
	// (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	
	//		W used	X used	y used	z used
	
	// within the second 16 genes, the bits determine if the input is inverted.
	// (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3
	//		w inv	x inv	y inv	z inv

	// congratulations its a girl/boy!
	CFourVariableBooleanCreature* pCreatureBaby = new CFourVariableBooleanCreature;	

	const int nGenes = 32; // number of genes
	const int nRungs = 4; // number of rungs per gene

	int nIndex; // index into DNA
	int nGene; // current gene
	int nRung; // index into current gene

	for ( nGene = 0; nGene < nGenes; nGene++ )
	{	int nRand = rand();	// get a random number
		// odd number indicates the gene will come from the father
		bool bFather = (( nRand & 1 ) == 1 );
		CFourVariableBooleanCreature* pParent = this; // mother by default
		if ( bFather )
		{	pParent = (CFourVariableBooleanCreature*)(&refFather);	
		}

		// copy each rung of the current gene 
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	nIndex = nGene * nRungs + nRung;
			bool bRung = pParent->GetRungFromDNA( nIndex );
			pCreatureBaby->AddRungToDNA( bRung );
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

//////////////////////////////////////////////////////////////////////
// A signature is like a fingerprint, it is a unique way of identifying
// the creature, and like reproduction, it is species dependent.
// For the CFourVariableBooleanCreature, the signature is the 
// boolean equation that is generated by the DNA.
CString CFourVariableBooleanCreature::GetSignature()
{
	// the DNA is divided into 32 genes	that are made up of 4 bits each
	
	// within the first 16 genes, the bits determine if an input is used
	// (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	
	//		W used	X used	y used	z used
	
	// within the second 16 genes, the bits determine if the input is inverted.
	// (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3
	//		w inv	x inv	y inv	z inv

	const int nTerms = 16; // maximum terms in the equation
	const int nVariables = 4; // number of binary variables
	const int nInputs = nTerms * nVariables; // total bits for use/invert genes
	const int nDnaSize =  m_DNA.size(); // length of a DNA

	// times 2 below because of the 2 different gene types
	ASSERT( nDnaSize == nInputs * 2 ); 

	int nIndex; // index into DNA
	int nTerm; // current term of the equation
	int nVariable; // current variable of the equation

	CString csOutput( "f = " ); // the equation will be written into this text string
	for ( nTerm = 0; nTerm < nTerms; nTerm++ )
	{	bool bTermIsUsed = false;
		for ( nVariable = 0; nVariable < nVariables; nVariable++ )
		{	nIndex = nTerm * nVariables + nVariable;
			ASSERT( nIndex < nInputs );
			bool bUse = m_DNA[ nIndex ];
			if ( bUse )
			{	ASSERT(( nIndex + nInputs ) < nDnaSize );
				bool bInvert = m_DNA[ nIndex + nInputs ];
				bTermIsUsed = true;
				switch ( nVariable )
				{	case 0 : // w
						csOutput += "w";
						break;
					case 1 : // x
						csOutput += "x";
						break;
					case 2 : // y
						csOutput += "y";
						break;
					case 3 : // z
						csOutput += "z";
						break;
				}
				if ( bInvert )
				{	csOutput += "'"; // add a prime
				}
			}
		}
		if ( bTermIsUsed )
		{	csOutput += " + "; // add an "or" between terms
		}
	}
	csOutput.TrimRight( " +" ); // trim trailing spaces and plus signs
	return csOutput;
} // GetSignature

//////////////////////////////////////////////////////////////////////
// format a string to represent a DNA 
CString CFourVariableBooleanCreature::FormatDnaString()
{	const int nGenes = 32; // number of genes
	const int nRungs = 4; // number of rungs per gene

	int nIndex; // index into DNA
	int nGene; // current gene
	int nRung; // index into current gene

	CString csDNA;

	for ( nGene = 0; nGene < nGenes; nGene++ )
	{	// copy each rung of the current gene 
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	nIndex = nGene * nRungs + nRung;
			bool bRung = GetRungFromDNA( nIndex );
			if ( bRung )
			{	csDNA += "1";
			} else
			{	csDNA += "0";
			}
		}
		csDNA += " ";
	}
	return csDNA;
} // FormatDnaString

//////////////////////////////////////////////////////////////////////
