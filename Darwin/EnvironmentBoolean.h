// EnvironmentBoolean.h: interface for the CEnvironmentBoolean class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENTBOOLEAN_H__6D147826_82BB_4F0E_B157_EDC90A97EB8C__INCLUDED_)
#define AFX_ENVIRONMENTBOOLEAN_H__6D147826_82BB_4F0E_B157_EDC90A97EB8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Environment.h"
#include "math.h"

class CEnvironmentBoolean : public CEnvironment  
{
public:
// definitions
protected:
	// example truth table read from disk in the constructor
	//
	//	Truth Table
	//	inputs=7
	//	outputs=4
	//	output=0
	//	#hex  abc defg =0,1,2,3
	//	30,   011 0000 =0,0,0,1
	//	33,   011 0011 =0,1,0,0
	//	5B,   101 1011 =0,1,0,1
	//	5F,   101 1111 =0,1,1,0
	//	6D,   110 1101 =0,0,1,0
	//	70,   111 0000 =0,1,1,1
	//	79,   111 1001 =0,0,1,1
	//	7B,   111 1011 =1,0,0,1
	//	7E,   111 1110 =0,0,0,0
	//	7F,   111 1111 =1,0,0,0
	//
	// The hex value preceeding the first comma is stored
	// in the nInput value of the structure.  
	//
	// The binary values between the , and = is for human 
	// consumption only to illustrate the boolean input variables
	// (Darwin ignores this data--it is equivalent to the leading
	// hex value)
	//
	// One of the output columns (designated by 
	// the "output=0" line of the table is stored in the
	// bOutput field of the structure (Darwin only calculates
	// one output function at a time).

	// the four outputs for the above table have been calculated 
	// by Darwin as follows:
	//		f0 = bdfg
	//		f1 = ad' + b' + a'g
	//		f2 = af' + b'e
	//		f3 = f'g' + ae'
	// These functions are not necessarily the only solutions.

	// the following structure is a single record from 
	// the above truth table example.
	typedef struct
	{	// input variables packed into a single integer
		// representing a single state of the truth table
		int nInput; 
		// expected value of the output	for this row of
		// the truth table
		bool bOutput; 
	} TABLE_ENTRY;

// construction / destruction
public:
	// default constructor
	CEnvironmentBoolean();
	// default destructor
	virtual ~CEnvironmentBoolean();

// attributes
public:
	// the number of output functions (represented as the 
	// comma separated columns following the equal sign in 
	// the truth table)
	// The value comes from the "outputs=#" line in the 
	// truth table.
	int GetNumberOfOutputs(){ return m_nOutputs; }
	// the number of output functions (represented as the 
	// comma separated columns following the equal sign in 
	// the truth table)
	// The value comes from the "outputs=#" line in the 
	// truth table.
	void SetNumberOfOutputs( int nValue = 0 ){ m_nOutputs = nValue; } 

	// The output being calculated--the truth table contains 
	// multiple possible outputs for every state of the table,
	// this value indicates which output is being calculated.
	// The value comes from the "output=#" line in the truth 
	// table.
	int GetOutput(){ return m_nOutput; }
	// The output being calculated--the truth table contains 
	// multiple possible outputs for every state of the table,
	// this value indicates which output is being calculated.
	// The value comes from the "output=#" line in the truth 
	// table.
	void SetOutput( int nValue = 0 ){ m_nOutput = nValue; } 

	// Generate inverted output
	bool GetInvertedOutput(){ return m_bInvertedOutput; }
	// Generate inverted output
	void SetInvertedOutput( bool bValue = true ){ m_bInvertedOutput = bValue; }

	// the number of binary input variables--supplied in the 
	// truth table by the "inputs=#" line.
	int GetNumberOfVariables(){ return m_nVariables; }
	// the number of binary input variables--supplied in the 
	// truth table by the "inputs=#" line.
	void SetNumberOfVariables( int nValue = 0 ){ m_nVariables = nValue; } 

	// Get the value of a single bit from the input value given 
	// its bit index (0 is the index of the most significant bit)
	bool GetBit( int nInput, int nIndex )
	{	// since the zero index is actually associated with the most 
		// significant bit, the index needs to be inverted first
		int nInv = GetNumberOfVariables() - nIndex - 1;
		int nMask = (int)pow( 2, nInv );
		// read the bit value using the calculated mask
		bool bValue = (( nInput & nMask ) == nMask );
		return bValue;
	}
	
	// maximumn number of terms in the equations--normally would be 
	// equal to the number of input combinations, but with don't care 
	// states, this value becomes the number of entries in the 
	// truth table.
	int GetMaximumNumberOfTerms()
	{	int nTerms = m_TruthTable.size();
		if ( m_nMaxTerms > 0 )
		{	nTerms = m_nMaxTerms;
		}
		return nTerms; 
	}
	// maximum number of equation terms--an artificial maximum can 
	// be read from the truth table.  this value is used unless
	// it is set to zero (where truth table size is used)
	void SetMaximumNumberOfTerms( int nValue = 0 ){ m_nMaxTerms = nValue; }

	// weighted value for number of correct answers when calculating fitness
	// (correct answers are more important than reducing the number of terms
	// and inputs)
	int GetCorrectAnswerWeight(){ return 10000000; }

	// weighted value for number of terms in the equation
	// (less important than correct answers, but slightly more
	// important than the number of inputs (un-weighted by default)
	int GetNumberOfTermsWeight(){ return 10000; }

// operations
public:
	// given a strand of DNA, evaluate its fitness to survive this environment
	// where larger numbers are better.  This is a pure virtual function,
	// so each derived environment needs to create its own implimentation.
	virtual int EvaluateFitness( const vector<bool>& DNA );

// data variables
private:
	// set of data points that we want the DNA to duplicate	-- this is the
	// environment the creature must survive -- the most fit creature can
	// duplicate this truth table with a minimum number of terms and inputs.
	vector<TABLE_ENTRY> m_TruthTable; 
	int m_nVariables; // number of boolean input variables
	int m_nOutputs; // number of boolean output functions
	int m_nOutput; // which output is being solved? (0..m_nOutputs - 1)
	bool m_bInvertedOutput;	// Generate inverted output
	int m_nMaxTerms; // maximum number of equation terms

};

#endif // !defined(AFX_ENVIRONMENTBOOLEAN_H__6D147826_82BB_4F0E_B157_EDC90A97EB8C__INCLUDED_)
