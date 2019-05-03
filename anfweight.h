//
//
// file   : anfweight.h
// desc.  : Interface for weight computation module
//
// author : Cagdas Calik, ccalik@metu.edu.tr
// date   : 14.03.2012
//


#ifndef ANFWEIGHT_H
#define ANFWEIGHT_H

#include "types.h"
#include "lint.h"
#include <list>
#include <vector>




#define ANFWEIGHT_N 128
#define ANFWEIGHT_TYPE u64
#define ANFWEIGHT_WORDBITS (8 * sizeof(ANFWEIGHT_TYPE))
#define ANFWEIGHT_WORDS ((ANFWEIGHT_N - 1) / ANFWEIGHT_WORDBITS + 1)

typedef struct _tagANFTERM
{
	u64 val[ANFWEIGHT_WORDS];

} ANFTERM;

typedef std::list<ANFTERM> ANFTERMLIST;
typedef std::vector<ANFTERM> ANFTERMVECTOR;

#define ANFGETBIT(term, i) ((term.val[(i) / ANFWEIGHT_WORDBITS] >> (ANFWEIGHT_WORDBITS - 1 - (i) % ANFWEIGHT_WORDBITS)) & 1)
#define ANFSETBIT(term, i) (term.val[(i) / ANFWEIGHT_WORDBITS] |= (ANFWEIGHT_TYPE)1 << ((ANFWEIGHT_WORDBITS - 1 - (i) % ANFWEIGHT_WORDBITS)))

void ANFTerm_ANDNOT(ANFTERM &dst, ANFTERM &src1, ANFTERM &src2);
void ANFTerm_OR(ANFTERM &dst, ANFTERM &src);
u32 ANFTerm_Covers(ANFTERM&, ANFTERM&);
u32 ANFTerm_Disjoint(ANFTERM&, ANFTERM&);
u32 ANFTerm_Precedes(ANFTERM&, ANFTERM&);
u32 ANFTerm_Equal(ANFTERM&, ANFTERM&);
u32 ANFTerm_Weight(ANFTERM&);
void ANFTerm_Clear(ANFTERM&);
void ANFTerm_Print(ANFTERM&);
u32 ANFTerm_IsZero(ANFTERM&);
int ANFTermList_Insert(ANFTERMLIST&, ANFTERM&);
void ANFTermList_Print(ANFTERMLIST&);
u32 ANFTermList_Exists(ANFTERMLIST&, ANFTERM&);


// Weight computation related
LINT ANFWeight_Compute(ANFTERMLIST&, u32 uDepth, ANFTERMLIST* plistIsolated = NULL);
LINT ANFWeight_ComputeDisjoint(ANFTERMLIST&);
LINT ANFWeight_SubtreeLookup(u32 w);

u32 ANFWeight_FindIsolatedTerm(ANFTERMLIST&, ANFTERMLIST&);
u32 ANFWeight_SplitIsolatedTerms(ANFTERMLIST& func, ANFTERMLIST &Isolated);


#endif // ANFWEIGHT_H
