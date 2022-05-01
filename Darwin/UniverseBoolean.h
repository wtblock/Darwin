/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003-2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Universe.h"
#include "EnvironmentBoolean.h"

class CUniverseBoolean : public CUniverse  
{
// construction / destruction
public:
	// general constructor
	CUniverseBoolean();
	// general destructor
	virtual ~CUniverseBoolean();

// operations
public:
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
	virtual CString GetIdentifierString(){ return "CUniverseBoolean"; }

	// update the average fitness given a current population and a new fitness
	// since fitness is specific to the type of creature
	// living in the universe, this is a pure virtual function
	// that must be handled by each specific universe
	// this is a running average that is recalculated after each
	// new creature is born -- the new fitness value is averaged in 
	// and the current population is incremented
	virtual void UpdateAverageFitness( int nFitness, UINT& nPopulation );

	// indicate if the fitness is acceptable--for example, if comparing results
	// with a table, indicate true if all correct answers are found.
	virtual bool GetAcceptableFitness()
	{	int nFitness;
		UINT uGeneration;
		CString csSignature;
		GetHighScoreData( nFitness, uGeneration, csSignature );	
		
		// acceptable fitness is one where the number of correct 
		// answers is equal to the maximum number of correct answers
		// (no incorrect answers)
		int nMaximumCorrectAnswers = m_env.GetMaximumNumberOfTerms();
		int nCorrectAnswerWeight = m_env.GetCorrectAnswerWeight();
		int nNumberOfCorrectAnswers = nFitness / nCorrectAnswerWeight;

		return nNumberOfCorrectAnswers == nMaximumCorrectAnswers;
	}

	// indicate if the fitness is stalled--for example, if comparing results
	// with a table, indicate true if the number of correct answers is not
	// changing.
	virtual bool GetStalledFitness()
	{	int nMaximumCorrectAnswers = m_env.GetMaximumNumberOfTerms();
		int nCorrectAnswerWeight = m_env.GetCorrectAnswerWeight();
		int nNumberOfCorrectAnswers = m_nHighFitness / nCorrectAnswerWeight;

		// if we have reached the maximum number of correct answers
		// then just look for no change in high generation
		bool bStalled = false;
		if ( nNumberOfCorrectAnswers == nMaximumCorrectAnswers )
		{	// stalled fitness means fitness has not improved in
			// 100 generations
		
			bStalled = ( m_uGeneration - m_uHighGeneration ) > 100;
		} else // look for no change in correct answers
		{	if ( nNumberOfCorrectAnswers > m_nMaxCorrectAnswers	)
			{	m_nMaxCorrectAnswers = nNumberOfCorrectAnswers;
				m_uMaxCorrectAnswersGeneration = m_uGeneration;
			}

			// stalled fitness means correct answers have not improved in 
			// 50 generations
			bStalled = ( m_uGeneration - m_uMaxCorrectAnswersGeneration ) > 20;
		}

		if ( bStalled )
		{	return true;
		}
		return false;
	}

// data variables
protected:
	CEnvironmentBoolean m_env; // environment to be survived
	int m_nMaxCorrectAnswers;
	UINT m_uMaxCorrectAnswersGeneration;

};

////////////////////////////////////////////////////////////////////////////
