// FourVariableBooleanUniverse.h: interface for the CFourVariableBooleanUniverse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOURVARIABLEBOOLEANUNIVERSE_H__8FEEA00B_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
#define AFX_FOURVARIABLEBOOLEANUNIVERSE_H__8FEEA00B_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Universe.h"

class CFourVariableBooleanUniverse : public CUniverse  
{
public:
	CFourVariableBooleanUniverse();
	virtual ~CFourVariableBooleanUniverse();

	// read the universe from a text file
	virtual bool Read( CStdioFile& refFile );
	// write the universe to a text file
	virtual void Write( CStdioFile& refFile );

	// build the next generation
	// this is a pure virtual function because it depends on the
	// type of creatures living in this universe and the type of 
	// environment they are required to survive
	virtual void NextGeneration();

	// the beginning of the universe initializes the population
	// this is pure virtual function, because it depends on the 
	// type of creatures living in this universe and the type of 
	// environment they are required to survive
	virtual void BigBang();

	// get identifier string - unique to each species
	virtual CString GetIdentifierString(){ return "CFourVariableBooleanUniverse"; }

	// update the average fitness given a current population and a new fitness
	// since fitness is specific to the type of creature
	// living in the universe, this is a pure virtual function
	// that must be handled by each specific universe
	// this is a running average that is recalculated after each
	// new creature is born -- the new fitness value is averaged in 
	// and the current population is incremented
	virtual void UpdateAverageFitness( int nFitness, UINT& nPopulation );
};

#endif // !defined(AFX_FOURVARIABLEBOOLEANUNIVERSE_H__8FEEA00B_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
