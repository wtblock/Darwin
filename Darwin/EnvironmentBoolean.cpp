// EnvironmentBoolean.cpp: implementation of the CEnvironmentBoolean class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "darwin.h"
#include "EnvironmentBoolean.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEnvironmentBoolean::CEnvironmentBoolean()
{	// initialize variables	to zero
	SetNumberOfOutputs();
	SetNumberOfVariables();
	SetOutput();
	SetInvertedOutput( false );	
	SetMaximumNumberOfTerms();
	
	// have to keep track of errors here because a text file
	// could easily have format errors or be missing
	bool bError = false;
	
	// read the truth table and setup problem variables
	try
	{	// the text file is assumed to be in the folder with Darwin
		// and is designed to contain only good states (No Don't Cares,
		// or invalid states)
  		CStdioFile file( "TruthTable.txt", CFile::modeRead );
		CString csLine;
		int nLine = 0;
		CString csMessage;
		while ( file.ReadString( csLine ))
		{	// remove leading and trailing whitespace
			csLine.TrimLeft();
			csLine.TrimRight();
			int nLen = csLine.GetLength();
			// lower case all text for simpler comparisons
			csLine.MakeLower();
			if ( nLine == 0 ) // verify this is a truth table file
			{	if ( csLine != "truth table" )
				{	bError = true;
					csMessage.Format( "%03d: Truth table file has invalid format\n", nLine );
					break;
				}		
			} else if ( nLen == 0 )	// ignore empty lines
			{	
			} else if ( csLine[ 0 ] == '#' ) // ignore comment lines
			{	
			} else // parse remaining lines
			{	char* pBuf = csLine.GetBuffer( nLen );
				CString csDelim( ",=" );
				char* pToken = strtok( pBuf, csDelim );
				if ( pToken )
				{	CString csToken = pToken;
					CString csValue;
					// read the number of input variables
					if ( csToken == "inputs" ) // read the number of input variables
					{	pToken = strtok( NULL, csDelim );
						if ( pToken )
						{	csValue = pToken;
							SetNumberOfVariables( atol( csValue ));	
						} else
						{	bError = true;
							csMessage.Format( "%03d: Error reading number of inputs\n", nLine );
							break;
						}
					} else if ( csToken == "outputs" ) // read the number of output variables
					{	pToken = strtok( NULL, csDelim );
						if ( pToken )
						{	csValue = pToken;
							SetNumberOfOutputs( atol( csValue ));
						} else
						{	bError = true;
							csMessage.Format( "%03d: Error reading number of outputs\n", nLine );
							break;
						}
					} else if ( csToken == "maxterms" ) // read the maximum number of terms in the equation
					{	pToken = strtok( NULL, csDelim );
						if ( pToken )
						{	csValue = pToken;
							SetMaximumNumberOfTerms( atol( csValue ));
						} else
						{	bError = true;
							csMessage.Format( "%03d: Error reading maximum number of terms\n", nLine );
							break;
						}
					} else if ( csToken == "output" ) // read the output variable being evaluated
					{	pToken = strtok( NULL, csDelim );
						if ( pToken )
						{	csValue = pToken;
							SetOutput( atol( csValue ));
						} else
						{	bError = true;
							csMessage.Format( "%03d: Error reading output value\n", nLine );
							break;
						}
					} else if ( csToken == "invert" ) // generate inverted output?
					{	pToken = strtok( NULL, csDelim );
						if ( pToken )
						{	csValue = pToken;
							int nValue = atol( csValue );
							SetInvertedOutput( nValue != 0 );
						} else
						{	bError = true;
							csMessage.Format( "%03d: Error reading invert value\n", nLine );
							break;
						}
					} else // read a row from the truth table
					{	TABLE_ENTRY te;
						char* pStop; // where the conversion stops
						
						// read the inputs from the line as a hex value--this represents
						// a valid state (combination of input values) where the input values
						// are packed as the least significant bits of this integer
						te.nInput = strtol( csToken, &pStop, 16 );

						// through away input comment--this is a binary equivalent of the leading
						// hex value--to aid the user, but not used by the program
						pToken = strtok( NULL, csDelim ); 
						if ( pToken )
						{	int nOut = 0; // column we are reading
							int nOutput = GetOutput(); // column we are looking for
							
							// assume failure
							csMessage.Format( "%03d: Output value too large\n", nLine );
							bError = true; 
							
							// parse the columns of output until we read the output
							// we are interested in--'output' token read earlier
							pToken = strtok( NULL, csDelim );
							while ( pToken )
							{	csValue = pToken;
								if ( nOut == nOutput ) // this is the output we want
								{	int nValue = atol( csValue );
									bool bValue = nValue != 0;
									if ( GetInvertedOutput())
									{	bValue = !bValue;
									}
									// store it in the table entry
									te.bOutput = bValue;  
									bError = false;	// signal success
									break;	
								}
								pToken = strtok( NULL, csDelim ); // get the next column
								nOut++;	// and increment the column number
							}
							if ( bError )
							{	break; // exit and display a message
							} else
							{	m_TruthTable.push_back( te ); // store entry in table
							}
						} else // line ended unexpectedly
						{	bError = true;	
							csMessage.Format( "%03d: Unexpected end-of-line\n", nLine );
						}
					}
				}
			}
			nLine++; // keep track of line number for error messages
		}
		if ( bError ) // tell the user what went wrong
		{	csMessage += "An error occurred reading the truth table file:\n\t TruthTableNoDC.txt";
			AfxMessageBox( csMessage );
		}
	}
	catch(...)
	{	AfxMessageBox( "Could not open truth table file: TruthTableNoDC.txt" );
		bError = true;
	}

	// record our success / failure
	SetIsOK( !bError );

} // CEnvironmentBoolean

//////////////////////////////////////////////////////////////////////
CEnvironmentBoolean::~CEnvironmentBoolean()
{
	m_TruthTable.clear();
}

//////////////////////////////////////////////////////////////////////
// given a strand of DNA evaluate its fitness to survive this environment
// where larger numbers are better.  This is a pure virtual function,
// so each derived environment needs to create its own implimentation.
int CEnvironmentBoolean::EvaluateFitness( const vector<bool>& DNA )
{	// using standard boolean algebra techniques, a truth table
	// can be evaluated to a boolean equation of the following format:

	//		f = c'd' + ac'+ a'b'd' + ab'd

	// where the apostrophy (') indicates the inverse of the variable
	
	// Fitness will be evaluated by the number of correct answers for 
	// all possible states for the variables a, b, c and d, etc., minimum 
	// number of terms (the above equation has 4), and finally the minimum 
	// number of inputs (the above equation has 10).	  
	
	// GetCorrectAnswerWeight() points will be awarded for each correct answer
	// GetNumberOfTermsWeight() points will be awarded times the inverse 
	// of the terms used 
	//		(Max Terms - Terms Used)
	// 1 point times the inverse of the inputs used (Max Inputs - Inputs Used)
	
	// the number of genes in the DNA is equal to the Max Terms * 2, where the 
	// first half of the genes are defined as USE gene, and the second half
	// of the genes are defined as INV genes.
	
	// within the first half of genes, the bits determine if an input is used
	// (the input is used if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3, etc.	
	//		a used	b used	c used	d used, etc.
	
	// the number of terms comes from this information, where a term is required 
	// for each gene that has at least one input used	

	// within the second half of genes, the bits determine if the input is inverted.
	// this information is only used if the input is used above.  
	// (the input is inverted if the bit is true):

	//		bit 0	bit 1	bit 2	bit 3, etc.
	//		a inv	b inv	c inv	d inv, etc.

	// number of boolean input variables
	const int nVariables = GetNumberOfVariables(); 
	// maximum number of terms in equation--determined by the number of 
	// entries in the truth table
	const int nMaxTerms = GetMaximumNumberOfTerms(); 
	// number of valid states for these variables--normally would be the number
	// of combinations of nVariables, but because some of the states are "don't care",
	// the number of valid states is the number of entries read from the truth table.
	const int nValidStates = nMaxTerms; 
	const int nMaxInputs = nVariables * nMaxTerms;

	// initialize the result to zero
	int nFitness = 0;

	// perform a little sanity check here to be sure the DNA is long enough
	if ( DNA.size() == nMaxInputs * 2 )
	{	int nIndex, nVariable;
		int nTerm, nTerms = 0;
		int nInputs = 0;
		
		// loop through all of the USE genes in the DNA
		// and count the terms and inputs used
		for ( nTerm = 0; nTerm < nMaxTerms; nTerm++ )
		{	bool bTermUsed = false;
			
			// loop through each rung of the gene's DNA
			for ( nVariable = 0; nVariable < nVariables; nVariable++ )
			{	// calculate an index into the USE gene DNA values
				nIndex = nTerm * nVariables + nVariable;
				// if the variable is used
				if ( DNA[ nIndex ] )
				{	bTermUsed = true;
					nInputs++;
				} 
			}		
			if ( bTermUsed )
			{	nTerms++;
			}
		}

		// count the correct answers generated by the DNA
		int nCorrectAnswers = 0;
		int nCnt;
		
		// loop through all of the valid states defined by the truth table
		for ( nCnt = 0; nCnt < nValidStates; nCnt++ )
		{	// read a valid state from the truth table (non-don't care state)
			// where a state is defined as specific combination of input variables
			// in the on and off state
			int nState = m_TruthTable[ nCnt ].nInput;
			
			// the truth table tells us what the expected result is for this state
			bool bCorrectResult = m_TruthTable[ nCnt ].bOutput;

			// we will accumulate the result of all terms by or'ing them together
			bool bAccumulatedResult = false;

			// loop through all of the genes in the given DNA
			for ( nTerm = 0; nTerm < nMaxTerms; nTerm++ )
			{	// we will accumulate the result of the used variables
				// of each term by and'ing them together.  we start by
				// assuming the result to be true, and any falses will
				// force the term result to be false.
				bool bTermResult = true;
				bool bTermUsed = false;

				// loop through each rung the gene's DNA
				for ( nVariable = 0; nVariable < nVariables; nVariable++ )
				{	// calculate an index into the USE gene DNA values
					nIndex = nTerm * nVariables + nVariable;
					// if the variable is used
					if ( DNA[ nIndex ] )
					{	// if any varibles are used, the term is used
						bTermUsed = true; 
						// offset USE gene by max inputs to index the INV gene
						bool bInverted = DNA[ nIndex + nMaxInputs ];
						// read the bit value from nState indexed by nVariable where 
						// a 0 nVariable corresponds to the most significant bit.
						bool bVariable = GetBit( nState, nVariable );
						// 'and' in the bit variable with possible inversion
						bTermResult = bTermResult && ( bVariable ^ bInverted );
					}
					// if any variable is false, the term will be false 
					// because variables are 'and'ed together
					if ( bTermResult == false )
					{	break; // do not need to check any more variables
					}
				}

				if ( bTermUsed )
				{	bAccumulatedResult = bAccumulatedResult || bTermResult;
					// if any term is true, the final result will be true
					// because terms are 'or'ed together
					if ( bAccumulatedResult == true )
					{	break; // do not need to check any more terms
					}
				}
			}

			// accumulate the number of correct answers
			if ( bAccumulatedResult == bCorrectResult )
			{	nCorrectAnswers++;
			}
					
		} // end valid state loop

		// calculate the fitness base on the weighted values for correct
		// answers, number of terms, and number of inputs
		nFitness = GetCorrectAnswerWeight() * nCorrectAnswers + 
			GetNumberOfTermsWeight() * ( nMaxTerms - nTerms ) + 
			nMaxInputs - nInputs;
	}	

	return nFitness;
} // EvaluateFitness

//////////////////////////////////////////////////////////////////////
