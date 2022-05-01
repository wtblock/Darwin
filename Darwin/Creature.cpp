// Creature.cpp: implementation of the CCreature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Darwin.h"
#include "Creature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// default constructor
CCreature::CCreature()
{
	USES_CONVERSION;
	
	int nRand = rand();	// get a random number
	m_bMale = (( nRand & 1 ) == 1 ); // odd numbers will be male
	
	// initialize the creature's fitness to default (zero is most unfit)
	SetFitness();
	// default the generation to zero
	SetGeneration();

	// each creature is assigned GUID (globally unique ID) which is 
	// stored as a character string
	GUID guidName;
	OLECHAR szGuid[ 40 ];

	CoCreateGuid( &guidName );
	::StringFromGUID2( guidName, szGuid, 40 );
	m_csName = OLE2CT( szGuid );

} // CCreature

//////////////////////////////////////////////////////////////////////
// copy constructor
CCreature::CCreature( CCreature* pClone )
{
	*this = *pClone;

} // CCreature

//////////////////////////////////////////////////////////////////////
// default destructor
CCreature::~CCreature()
{
	// cleanup by emptying the container
	m_DNA.clear();
}

//////////////////////////////////////////////////////////////////////
// less than operator
bool CCreature::operator < ( CCreature& refCreature )
{
	return m_nFitness < refCreature.GetFitness(); 
} // operator <

//////////////////////////////////////////////////////////////////////
// Mutation is one of the ways variety is added to the
// gene pool of the creature population. The uChance parameter
// represents the likelyhood of one of the DNA rungs being 
// changed ( 1 chance in uChances )
void CCreature::Mutate( UINT uChance )
{
	if ( uChance == 0 )
	{	return; // nothing to do
	}

	UINT uRungs = m_DNA.size(); // number of rungs in DNA
	// probability is mulitplied by the	total available rungs 
	// that can be changed
	uChance *= uRungs;  
	UINT uRung = (UINT)abs( rand()) % uChance;  
	if ( uRung < uRungs )
	{	bool bValue = m_DNA[ uRung ];
		m_DNA[ uRung ] = !bValue; // switch the value
	} 
} // Mutate

//////////////////////////////////////////////////////////////////////
