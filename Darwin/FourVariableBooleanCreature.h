// FourVariableBooleanCreature.h: interface for the CFourVariableBooleanCreature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOURVARIABLEBOOLEANCREATURE_H__8FEEA009_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
#define AFX_FOURVARIABLEBOOLEANCREATURE_H__8FEEA009_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Creature.h"

class CFourVariableBooleanCreature : public CCreature  
{
public:
/*	CFourVariableBooleanCreature
	(	bool bMale, // male gender       
		int nGeneration, // generation   
		int nFitness, // creature fitness
		CString& refName, // name of the creature
		CString& refFather, // name of the creature's father
		CString& refMother, // name of the creature's mother
		CString& refDNA // the creature's DNA
	);
*/
	CFourVariableBooleanCreature( int nRungs = 0, int nGenerations = 0 );
	CFourVariableBooleanCreature( CFourVariableBooleanCreature* pClone );
	virtual ~CFourVariableBooleanCreature();
	
	// The copy operator is used to clone creatures
	CFourVariableBooleanCreature& operator=( CFourVariableBooleanCreature& refClone )
	{	SetIsMale( refClone.GetIsMale());
		SetGeneration( refClone.GetGeneration());
		SetName( refClone.GetName());
		SetFitness( refClone.GetFitness()); 
		
		// copy the rungs of DNA
		int nRungs = refClone.GetLengthOfDNA();
		int nRung;
		for ( nRung = 0; nRung < nRungs; nRung++ )
		{	bool bRung = refClone.GetRungFromDNA( nRung );
			AddRungToDNA( bRung );	
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

	// Given another creature, a new creature is produced.
	// Combining of DNA is on a gene by gene basis and is 
	// therefore Species specific.
	virtual CCreature* Reproduce( CCreature& refFather );

	// A signature is like a fingerprint, it is a unique way of identifying
	// the creature, and like reproduction, it is species dependent.
	// For the CFourVariableBooleanCreature, the signature is the 
	// boolean equation that is generated by the DNA.
	virtual CString GetSignature();

	// format a string to represent a DNA 
	virtual CString FormatDnaString();
};

#endif // !defined(AFX_FOURVARIABLEBOOLEANCREATURE_H__8FEEA009_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
