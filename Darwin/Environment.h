/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
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
	// so each derived environment needs to create its own implementation.
	virtual int EvaluateFitness( const vector<bool>& DNA ) = 0;

// data values
protected:
	bool m_bOK; // true is environment is error free

};

/////////////////////////////////////////////////////////////////////////////
