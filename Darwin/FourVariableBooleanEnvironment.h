// FourVariableBooleanEnvironment.h: interface for the CFourVariableBooleanEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOURVARIABLEBOOLEANENVIRONMENT_H__8FEEA00A_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
#define AFX_FOURVARIABLEBOOLEANENVIRONMENT_H__8FEEA00A_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Environment.h"

/////////////////////////////////////////////////////////////////////////////
class CFourVariableBooleanEnvironment : public CEnvironment  
{
public:
	CFourVariableBooleanEnvironment();
	virtual ~CFourVariableBooleanEnvironment();
	// given a strand of DNA, evaluate its fitness to survive this environment
	// where larger numbers are better.  This is a pure virtual function,
	// so each derived environment needs to create its own implimentation.
	virtual int EvaluateFitness( const vector<bool>& DNA );

private:
	vector<bool> m_TruthTable; // set of data points that we want the DNA to duplicate
};

#endif // !defined(AFX_FOURVARIABLEBOOLEANENVIRONMENT_H__8FEEA00A_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
