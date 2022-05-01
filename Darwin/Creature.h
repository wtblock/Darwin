/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "Environment.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
class CCreature  
{
// construction / destruction
public:
	// default constructor
	CCreature();
	// copy constructor
	CCreature( CCreature* pCreature );
	// default destructor
	virtual ~CCreature();

// attributes
public:
	// is this creature male?
	bool GetIsMale(){ return m_bMale; }
	// set this creature's gender to male?
	void SetIsMale( bool bValue = true ){ m_bMale = bValue; }
	// what is this creature's fitness
	int GetFitness(){ return m_nFitness; }
	// set this creature's fitness
	void SetFitness( int nFitness = 0 ){ m_nFitness = nFitness; }
	// what is this creature's generation
	int GetGeneration(){ return m_nGeneration; }
	// set this creature's generation
	void SetGeneration( int nGeneration = 0 ){ m_nGeneration = nGeneration; }
	// what is the creature's name
	CString GetName(){ return m_csName; }
	// set the creature's name
	void SetName( LPCSTR pcszName ){ m_csName = pcszName; }
	// return number of ancestors
	int GetNumberOfAncestors(){ return (int)m_Ancestors.size(); }
	// get an ancestor given an index to ancestor array
	CString GetAncestor( int nIndex )
	{	
		ASSERT( -1 < nIndex && nIndex < (int)m_Ancestors.size());
		return m_Ancestors[ nIndex ];
	}
	// add an ancestor
	void AddAncestor( CString& csName ){ m_Ancestors.push_back( csName ); }
	// given a name, is it an ancestor?
	bool IsAncestor( CString& csName )
	{	int nAncestor, nAncestors = GetNumberOfAncestors();
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	if ( csName == GetAncestor( nAncestor ))
			{	return true;
			}
		}
		return false;
	}
	// given another creature, is it related, i.e is it an ancestor
	// or do we have a common ancestor?
	bool GetIsRelated( CCreature& refCreature )
	{	// first see if the given creature is related
		CString csName = refCreature.GetName();
		if ( IsAncestor( csName ))
		{	return true;
		}
		// next check to see if we have a common ancestor
		int nAncestor, nAncestors = refCreature.GetNumberOfAncestors();
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	csName = refCreature.GetAncestor( nAncestor );
			if ( IsAncestor( csName ))
			{	return true; 
			}
		}
		return false;
	}

	// length of the DNA
	int GetLengthOfDNA(){ return (int)m_DNA.size(); }
	// length of the DNA
	void SetLengthOfDNA( int nValue ){ m_DNA.resize( nValue ); }

	// rung of the DNA
	bool GetDNA( int nRung )
	{	ASSERT( -1 < nRung && nRung < (int)m_DNA.size());
		return m_DNA[ nRung ];
	}
	// rung of the DNA
	void SetDNA( int nRung, bool bValue )
	{	ASSERT( -1 < nRung && nRung < (int)m_DNA.size());
		m_DNA[ nRung ] = bValue;
	}
	// add a rung to the DNA
	void AddRungToDNA( bool bRung ){ m_DNA.push_back( bRung ); }

// operations
public:
	// set DNA from DNA string
	void SetDnaFromString( CString& refDNA )
	{	int nChar;
		int nChars = refDNA.GetLength();

		m_DNA.clear();
		for ( nChar = 0; nChar < nChars; nChar++ )
		{	char cValue = refDNA[ nChar ];
			
			switch ( cValue )
			{	case '0' : AddRungToDNA( false ); break;
				case '1' : AddRungToDNA( true ); break;
				default : continue; // skip all other characters
			}
		}
	}

	// remove all ancestors
	void ClearAncestors(){ m_Ancestors.clear(); }

	// The less than operator is used for comparisons between creatures
	// and is based on the creature's fitness.  The operator returns TRUE 
	// if this creature is less than the referenced creature.
	bool operator < ( CCreature& refCreature );
	
	// The copy operator is used to clone creatures
	CCreature& operator=( CCreature& refClone )
	{	SetIsMale( refClone.GetIsMale());
		SetGeneration( refClone.GetGeneration());
		SetName( refClone.GetName());
		SetFitness( refClone.GetFitness()); 
		
		// copy the rungs of DNA
		int nRungs = refClone.GetLengthOfDNA();
		SetLengthOfDNA( nRungs );
		int nRung;
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	SetDNA( nRung, refClone.GetDNA( nRung ));	
		}
		// copy the ancestors
		int nAncestors = refClone.GetNumberOfAncestors();
		int nAncestor;
		for ( nAncestor = 0; nAncestor < nAncestors; nAncestor++ )
		{	CString csName = refClone.GetAncestor( nAncestor );
			AddAncestor( csName );	
		}
		return *this;
	}

	// Each creature will be asked to survive given an "environment" 
	// and the creature's fitness will be calculated as a measure 
	// of how well it was adapted for the environment. 
	int Survive( CEnvironment& env	)
	{
		m_nFitness = env.EvaluateFitness( m_DNA );
		return m_nFitness;
	}

	// Mutation is one of the ways variety is added to the
	// gene pool of the creature population. The uChance parameter
	// represents the likelihood of one of the DNA rungs being 
	// changed ( 1 chance in uChances )
	void Mutate( UINT uChance = 1000 );

/////////////////////////////////////////////////////////////////////////////
// Pure virtual functions -- the CCreature definition cannot exist in nature
// because the following definitions are not defined.  These are referred to
// as Pure Virtual Functions, and indicate that any creatures derived from
// our creature definition has to have this behavior, but the exact behavior
// is specific to the species.  Reproduction is an example here, because
// how the DNA combines is dependent on the number of rungs and how they are
// divided into genes.
/////////////////////////////////////////////////////////////////////////////
public:
	// Given another creature, a new creature is produced.
	// Combining of DNA is on a gene by gene basis and is 
	// therefore Species specific.
	virtual CCreature* Reproduce( CCreature& refFather ) = 0;

	// A signature is like a fingerprint, it is a unique way of identifying
	// the creature, and like reproduction, it is species dependent. The 
	// purpose of the signature is to provide a way for us as humans to
	// recognize the creature without having to look its DNA.
	virtual CString GetSignature() = 0;
	
	// format a string to represent a DNA 
	virtual CString FormatDnaString() = 0;
	
// data variables
protected:
	vector<bool> m_DNA; // the DNA strand is an array of booleans
	bool m_bMale; // true if the creature is male
	int m_nFitness; // measure of the creature's fitness where larger numbers are more fit
	int m_nGeneration; // in what generation was this creature born
	CString m_csName; // unique name assigned to the creature
	vector<CString> m_Ancestors; // mother, father, grand parents, etc.
};

/////////////////////////////////////////////////////////////////////////////
