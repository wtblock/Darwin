// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__8FEEA006_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
#define AFX_ENVIRONMENT_H__8FEEA006_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
class CEnvironment  
{
// construction / destruction
public:
	// default constructor
	CEnvironment();
	// default destructor
	virtual ~CEnvironment();

// attributes
public:
	// is this a good environment--returns true if there were no errors 
	// during the creation of the environment.
	bool GetIsOK(){ return m_bOK; }
	// is this a good environment--returns true if there were no errors 
	// during the creation of the environment.
	void SetIsOK( bool bValue = true ){ m_bOK = bValue; }

// operations
public:
	// given a strand of DNA evaluate its fitness to survive this environment
	// where larger numbers are better.  This is a pure virtual function,
	// so each derived environment needs to create its own implimentation.
	virtual int EvaluateFitness( const vector<bool>& DNA ) = 0;

// data values
protected:
	bool m_bOK; // true is environment is error free

};

#endif // !defined(AFX_ENVIRONMENT_H__8FEEA006_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
