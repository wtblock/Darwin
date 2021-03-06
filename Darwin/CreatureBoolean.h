/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Creature.h"

/////////////////////////////////////////////////////////////////////////////
class CCreatureBoolean : public CCreature  
{
// attributes
public:
	// number of binary variables
	void SetNumberOfVariables( int nValue = 0 ){ m_nVariables = nValue; }
	// number of binary variables
	int GetNumberOfVariables(){ return m_nVariables; }

	// The output being calculated--the truth table contains 
	// multiple possible outputs for every state of the table,
	// this value indicates which output is being calculated.
	// The value comes from the "output=#" line in the truth 
	// table.
	void SetOutput( int nValue = 0 ){ m_nOutput = nValue; }
	// The output being calculated--the truth table contains 
	// multiple possible outputs for every state of the table,
	// this value indicates which output is being calculated.
	// The value comes from the "output=#" line in the truth 
	// table.
	int GetOutput(){ return m_nOutput; }

	// maximum number of equation terms
	void SetMaximumNumberOfTerms( int nValue = 0 ){ m_nMaxTerms = nValue; }
	// maximum number of equation terms
	int GetMaximumNumberOfTerms(){ return m_nMaxTerms; } 

	// Generate inverted output
	bool GetInvertedOutput(){ return m_bInvertedOutput; }
	// Generate inverted output
	void SetInvertedOutput( bool bValue = true ){ m_bInvertedOutput = bValue; }

	// number of genes
	int GetNumberOfGenes(){ return 2 * GetMaximumNumberOfTerms(); } 

	// number of different types of genes
	int GetNumberOfGeneTypes(){ return 2; }

// construction	/ destruction
public:
	// constructor to randomly create creatures given the number of rungs
	// in the DNA and the number of restricted generations
	CCreatureBoolean( int nRungs = 0, int nGenerations = 0 );
	// copy constructor used for cloning creatures
	CCreatureBoolean( CCreatureBoolean* pClone );
	// default destructor
	virtual ~CCreatureBoolean();
	
// operations
public:
	// The copy operator is used to clone creatures
	CCreatureBoolean& operator=( CCreatureBoolean& refClone )
	{	SetIsMale( refClone.GetIsMale());
		SetGeneration( refClone.GetGeneration());
		SetName( refClone.GetName());
		SetFitness( refClone.GetFitness()); 
		SetNumberOfVariables( refClone.GetNumberOfVariables());
		SetMaximumNumberOfTerms( refClone.GetMaximumNumberOfTerms());
		SetInvertedOutput( refClone.GetInvertedOutput());
		SetOutput( refClone.GetOutput());
		
		// copy the rungs of DNA
		int nRung, nRungs = refClone.GetLengthOfDNA();
		SetLengthOfDNA( nRungs );
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	SetDNA( nRung, refClone.GetDNA( nRung ));
		}
		// copy the ancestors
		int nAncestor, nAncestors = refClone.GetNumberOfAncestors();
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	CString csName = refClone.GetAncestor( nAncestor );
			AddAncestor( csName );	
		}
		return *this;
	}

	// Given another creature, a new creature is produced.
	// Combining of DNA is on a gene by gene basis and is 
	// therefore Species specific.
	virtual CCreature* Reproduce( CCreature& refFather );

	// A signature is like a fingerprint, it is a unique way of identifying
	// the creature, and like reproduction, it is species dependent.
	// For the CCreatureBoolean, the signature is the 
	// boolean equation that is generated by the DNA.
	virtual CString GetSignature();

	// format a string to represent a DNA 
	virtual CString FormatDnaString();

// data variables
protected:
	int m_nOutput; // output number being generated
	int m_nVariables; // number of input variables
	int m_nMaxTerms; // maximum number of equation terms
	bool m_bInvertedOutput;
};

/////////////////////////////////////////////////////////////////////////////
