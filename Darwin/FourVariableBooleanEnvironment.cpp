// FourVariableBooleanEnvironment.cpp: implementation of the CFourVariableBooleanEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "FourVariableBooleanEnvironment.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CFourVariableBooleanEnvironment::CFourVariableBooleanEnvironment()
{
	// these are the data points we want the DNA to be able to 
	// reproduce give the four input variables (W, X, Y, and Z).
	
	const bool f[] = 
	{	// f		W X Y Z
		true,	// 	0 0 0 0
		false, 	//	0 0	0 1
		true, 	//	0 0	1 0
		false,	//	0 0	1 1
		true, 	//	0 1	0 0
		false,	//	0 1	0 1
		false,	//	0 1	1 0
		false,	//	0 1	1 1
		true,	//	1 0	0 0
		true,	//	1 0	0 1
		false,	//	1 0	1 0
		true, 	//	1 0	1 1
		true,	//	1 1	0 0
		true,	//	1 1	0 1
		false,	//	1 1	1 0
		false,	//	1 1	1 1
	};

	// initialize the truth table
	int i;
	for ( i = 0; i < 16; i++ )
	{	m_TruthTable.push_back( f[ i ] );
	}
}

//////////////////////////////////////////////////////////////////////
// default destructor
CFourVariableBooleanEnvironment::~CFourVariableBooleanEnvironment()
{
	m_TruthTable.clear();
}

//////////////////////////////////////////////////////////////////////
// given a strand of DNA evaluate its fitness to survive this environment
// where larger numbers are better.  This is a pure virtual function,
// so each derived environment needs to create its own implimentation.
int CFourVariableBooleanEnvironment::EvaluateFitness( const vector<bool>& DNA )
{	// using standard boolean algebra techniques, this truth table
	// can be evaluated with the following boolean equation:

	//		f = y'z' + wy'+ w'x'z' + wx'z

	// where the apostrophy (') indicates the inverse of the variable
	
	// Fitness will be evaluated by the number of correct answers for 
	// all possible states for the variables w, x, y and z, minimum 
	// number of terms (the above equation has 4 out of a possible 16), 
	// and finally the minimum number of inputs (the above equation has
	// 10 inputs out of a possible 64).	  
	
	// 10000 points will be awarded for each correct answer
	// 100 points will be awarded times the value of (16 - number of terms)
	// 1 point times (64 - number of inputs)
	
	// the DNA is divided into 32 genes	that are made up of 4 bits each
	
	// within the first 16 genes, the bits determine if an input is used
	// (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3	
	//		W used	X used	y used	z used
	
	// the number of terms comes from this information, where a term is required 
	// for each gene that has at least one input used	

	// within the second 16 genes, the bits determine if the input is inverted.
	// this information is only used if the input is used above.  
	// (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3
	//		w inv	x inv	y inv	z inv

	const int nVariables = 4;  // number of input variable
	const int nMaxTerms = 16;  // maximum number of terms in equation
	const int nMaxStates = nMaxTerms; // maximum states (combinations) for these variables
	const int nMaxInputs = nVariables * nMaxTerms;

	// initialize the result to zero
	int nFitness = 0;

	// perform a little sanity check here to be sure the DNA is long enough
	if ( DNA.size() == nMaxInputs * 2 )
	{	int nIndex, nVariable;
		int nTerm, nTerms = 0;
		int nInputs = 0;
		
		// count the terms, inputs
		for ( nTerm = 0; nTerm < nMaxTerms; nTerm++ )
		{	bool bTermUsed = false;
			for ( nVariable = 0; nVariable < nVariables; nVariable++ )
			{	nIndex = nTerm * nVariables + nVariable;
				if ( DNA[ nIndex ] )
				{	bTermUsed = true;
					nInputs++;
				} 
			}		
			if ( bTermUsed )
			{	nTerms++;
			}
		}
		// count the correct answers
		int nCorrectAnswers = 0;
		int nState;
		for ( nState = 0; nState < nMaxStates; nState++ )
		{	// generate all possible combinations of the input variables
			bool w = (( nState & 8 ) == 8 );
			bool x = (( nState & 4 ) == 4 );
			bool y = (( nState & 2 ) == 2 );
			bool z = (( nState & 1 ) == 1 );

			// get the correct answer from the truth table
			bool bCorrectResult = m_TruthTable[ nState ];

			// we will accumulate the result of all terms by or'ing them together
			bool bAccumulatedResult = false;
			for ( nTerm = 0; nTerm < nMaxTerms; nTerm++ )
			{	// we will accumulate the result of the used variables
				// of each term by and'ing them together
				bool bTermResult = true;
				bool bTermUsed = false;
				for ( nVariable = 0; nVariable < nVariables; nVariable++ )
				{	nIndex = nTerm * nVariables + nVariable;
					bool bInverted;
					if ( DNA[ nIndex ] ) // variable is used
					{	bTermUsed = true;
						bInverted = DNA[ nIndex + nMaxInputs ];
						switch ( nVariable )
						{	case 0 : // w
								bTermResult = bTermResult && ( w ^ bInverted );
								break;
							case 1 : // x
								bTermResult = bTermResult && ( x ^ bInverted );
								break;
							case 2 : // y
								bTermResult = bTermResult && ( y ^ bInverted );
								break;
							case 3 : // z
								bTermResult = bTermResult && ( z ^ bInverted );
								break;
						}
					}
					// if any variable if false, the term will be false 
					if ( bTermResult == false )
					{	break; // do not need to check any more variables
					}
				}
				if ( bTermUsed )
				{	bAccumulatedResult = bAccumulatedResult || bTermResult;
					// if any term is true, the final result will be true
					if ( bAccumulatedResult == true )
					{	break; // do not need to check any more terms
					}
				}
			}
			if ( bAccumulatedResult == bCorrectResult )
			{	nCorrectAnswers++;
			}		
		}
		nFitness = 10000 * nCorrectAnswers + 100 * ( nMaxTerms - nTerms ) + nMaxInputs - nInputs;
	}	

	return nFitness;
} // EvaluateFitness

//////////////////////////////////////////////////////////////////////
