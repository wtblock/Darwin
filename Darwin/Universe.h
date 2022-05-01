// Universe.h: interface for the CUniverse class.
//

#if !defined(AFX_UNIVERSE_H__8FEEA008_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
#define AFX_UNIVERSE_H__8FEEA008_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "creature.h"
#include <vector>
#include <queue>

// TEMPLATE STRUCT lessptr
// Priority queues (priority_queue from The C++ Standard Library) 
// expect a parameter explaining how their contents are compared
// to each other (so they can be prioritized). Creatures are
// stored in priority queues as pointers to the creatures because they
// are allocated on the heap with the new operator.  This template
// insures the pointers themselves are not compared, but instead are
// dereferenced first, and the contents of what they are pointing to
// (CCreatures) are compaired.

// CCreatures have a less than operator, so when creatures are 
// compared to each other, their fitness values determine
// if a creature is less than or greater than another creature.

// Priority queues insure the first creature out of the queue has the
// highest fitness values.
template<class _Ty>
struct lessptr : binary_function<_Ty, _Ty, bool> 
{
	bool operator()(_Ty _X, _Ty _Y) const
	{	return (*_X < *_Y); 
	}
};

//////////////////////////////////////////////////////////////////////
class CUniverse  
{
// construction / destruction
public:
	// default constructor
	CUniverse();
	// default destructor
	virtual ~CUniverse();

// attributes
public:
	// is this a good universe--returns true if there were no errors 
	// during the creation of the universe.
	bool GetIsOK(){ return m_bOK; }
	// is this a good universe--returns true if there were no errors 
	// during the creation of the universe.
	void SetIsOK( bool bValue = true ){ m_bOK = bValue; }

	// number of clones to pass to next generation
	void SetNumberOfClones( UINT uValue ){ m_uNumClones = uValue; }
	// number of clones to pass to next generation
	UINT GetNumberOfClones(){ return m_uNumClones; }

	// support genders
	void SetGender( bool bValue = true ){ m_bGender = bValue; }
	// support genders
	bool GetGender(){ return m_bGender; }

	// number of generations to restrict marriage of relatives
	void SetRestrictedGenerations( UINT uValue = 1 ){ m_uRestrictedGenerations = uValue; }
	// number of generations to restrict marriage of relatives
	UINT GetRestrictedGenerations(){ return m_uRestrictedGenerations; }

	// support mutations
	void SetMutation( bool bValue = true ){ m_bMutation = bValue; }
	// support mutations
	bool GetMutation(){ return m_bMutation; }

	// the current generation
	void SetGeneration( UINT uValue ){ m_uGeneration = uValue; }
	// the current generation
	UINT GetGeneration(){ return m_uGeneration; }

	// the current population
	void SetPopulation( UINT uValue ){ m_uPopulation = uValue; }
	// the current population
	UINT GetPopulation(){ return m_uPopulation; }

	// the initial population
	void SetInitialPopulation( UINT uValue ){ m_uInitialPopulation = uValue; }
	// the initial population
	UINT GetInitialPopulation(){ return m_uInitialPopulation; }

	// the chance of a mutation is one divided by this number
	void SetMutationChance( UINT uValue ){ m_uMutationChance = uValue; }
	// the chance of a mutation is one divided by this number
	UINT GetMutationChance(){ return m_uMutationChance; }

	// number of children born to each family
	void SetChildren( UINT uValue ){ m_uChildren = uValue; }
	// number of children born to each family
	UINT GetChildren(){ return m_uChildren; }

	// get the start date/time
	COleDateTime GetStartDateTime(){ return m_dateStart; }

	// set the start date/time if it has not already been set
	void SetStartDateTime( COleDateTime dt )
	{	COleDateTime dtNew;
		if ( m_dateStart == dtNew )
		{	m_dateStart = dt;
		}
	}
	// set the incremental start date/time
	void SetIncrementalStartDateTime()
	{	
		m_dateIncrementalStart = COleDateTime::GetCurrentTime();
	}
	// accumulate run time
	void AccumulateRunTime()
	{	COleDateTime dt( COleDateTime::GetCurrentTime());
		COleDateTimeSpan dtSpan;
		dtSpan = dt	- m_dateIncrementalStart;
		m_timeSpan += dtSpan;	
	}
	// get the time span -- length of time the experiment ran
	COleDateTimeSpan GetRunTime(){ return m_timeSpan; }

	// get the female population
	UINT GetFemalePopulation(){ return m_FemalePopulation.size(); }

	// get the male population
	UINT GetMalePopulation(){ return m_MalePopulation.size(); }

	// update high score data
	void UpdateHighScoreData();
	
	// get high score data
	void GetHighScoreData( int& nFitness, UINT& uGeneration, CString& csSignature );

	// get the average fitness
	int GetAverageFitness(){ return m_nAverageFitness; }
	// reset the average fitness
	void ResetAverageFitness(){ m_nAverageFitness = 0; }

// operations
public:
	// reset the start date/time back to un-initialized state,
	// all high values to zero, and clear out all populations
	void Reset()
	{	COleDateTime dtNew;
		m_dateStart = dtNew;
		COleDateTimeSpan dtSpan;
		m_timeSpan = dtSpan;
		FreePopulations();
		m_uHighGeneration = 0;
		m_csHighSignature = "";
		m_nHighFitness = 0;
		m_nAverageFitness = 0;
		m_uGeneration = 0;
		m_uPopulation = 0;
		m_arrFitness.clear();
	}

	// free populations
	void FreePopulations()
	{	// clean up the male population
		while ( !m_MalePopulation.empty())
		{	CCreature* pCreature = m_MalePopulation.top();
			delete [] pCreature;
			m_MalePopulation.pop();
		}
		// clean up the female population
		while ( !m_FemalePopulation.empty())
		{	CCreature* pCreature = m_FemalePopulation.top();
			delete [] pCreature;
			m_FemalePopulation.pop();
		}
	} // FreePopulations

	// handle unexpected end of file
	bool UnexpectedEOF()
	{	AfxMessageBox( "Unexpected end of file" );
		return false;
	}

	// print a line of text and return new y co-ordinate
	int PrintLine( CDC* pDC, int nX, int nY, const CString& csLine );

// overrides
public:
	// print the state of the universe
	virtual void Print( CDC* pDC );

	// read the universe from a text file
	virtual bool Read( CStdioFile& refFile );
	// write the universe to a text file
	virtual void Write( CStdioFile& refFile );
	// write the average fitness to text file
	virtual void ReadAverageFitness( CStdioFile& refFile );
	// write the average fitness to text file
	virtual void WriteAverageFitness( CStdioFile& refFile );

	// build the next generation
	// this is a pure virtual function because it depends on the
	// type of creatures living in this universe and the type of 
	// environment they are required to survive
	virtual void NextGeneration() = 0;

	// the beginning of the universe initializes the population
	// this is pure virtual function, because it depends on the 
	// type of creatures living in this universe and the type of 
	// environment they are required to survive
	virtual void BigBang() = 0;

	// get identifier string - unique to each species
	virtual CString GetIdentifierString() = 0;

	// update the average fitness given a current population and a new fitness
	// since fitness is specific to the type of creature
	// living in the universe, this is a pure virtual function
	// that must be handled by each specific universe
	virtual void UpdateAverageFitness( int nFitness, UINT& nPopulation ) = 0;

	// indicate if the fitness is acceptable--for example, if comparing results
	// with a table, indicate true if all correct answers are found.
	virtual bool GetAcceptableFitness() = 0;

	// indicate if the fitness is stalled--for example, if comparing results
	// with a table, indicate true if the number of correct answers is not
	// changing.
	virtual bool GetStalledFitness() = 0;

// operations
protected:
	// copy the male/female population to the enumeration population
	void SetEnumeration( bool bMale );
	// parse strings in the format of "key=value"
	bool ParseKeyValueString( CString& csLine, CString& csKey, CString& csValue );
	// read a string value
	bool ReadKey( CStdioFile& refFile, LPCSTR pcszKey, CString& refValue );
	// read a long value
	bool ReadKey( CStdioFile& refFile, LPCSTR pcszKey, long& refValue );
	// read a bool value
	bool ReadKey( CStdioFile& refFile, LPCSTR pcszKey, bool& refValue );

// data values
protected:
	bool m_bOK;					// true if universe had no errors during creation
	bool m_bGender;				// support genders
	bool m_bMutation;			// support mutations
	UINT m_uRestrictedGenerations;// number of generations to restrict marriage of relatives
	UINT m_uNumClones;			// number of fittest creatures to clone
	UINT m_uGeneration;			// current generation
	UINT m_uPopulation;			// current population
	UINT m_uInitialPopulation;	// initial population
	UINT m_uMutationChance;		// 1 chance in this of a mutation
	UINT m_uChildren;			// number of children per family

	// high scorer
	UINT m_uHighGeneration;		// generation of high scorer
	CString m_csHighSignature;	// signature of high scorer
	int m_nHighFitness;			// fitness of high scorer
	int m_nAverageFitness;		// average fitness of population
	 
	COleDateTime m_dateStart;	// starting date and time
	COleDateTimeSpan m_timeSpan;// total running time
	COleDateTime m_dateIncrementalStart; // starting date and time of incremental run
	COleDateTimeSpan m_timeIncrementalSpan;	// incremental running time
	
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > m_MalePopulation; // all the male creatures of the land
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > m_FemalePopulation; // all the female creatures of the land
	priority_queue<CCreature*, std::vector<CCreature*>, lessptr<CCreature*> > m_EnumPopulation; // temporary for enumerating populations

	// keep an array of average fitness values for each generation
	vector<int> m_arrFitness;
};

#endif // !defined(AFX_UNIVERSE_H__8FEEA008_A783_11D4_B7BC_00C04FFA3C73__INCLUDED_)
