//
//
// file   : anfweight.cpp
// desc.  : Implementation of the algorithm that computes the weight
//          of a Boolean function from its Algebraic Normal Form
//
// author : Cagdas Calik
// e-mail : cagdascalik@gmail.com
//
// file header updated on : 01.05.2019
// first release date     : 14.03.2012
//


#include "anfweight.h"
#include <memory.h>
#include <stdio.h>

unsigned int _uByteWeight[] = {
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};


#define U32WEIGHT(x) (_uByteWeight[((x) >> 0) & 0xff] + \
					  _uByteWeight[((x) >> 8) & 0xff] + \
					  _uByteWeight[((x) >> 16) & 0xff] + \
					  _uByteWeight[((x) >> 24) & 0xff])

#define U64WEIGHT(x) (_uByteWeight[((x) >> 0) & 0xff] + _uByteWeight[((x) >> 8) & 0xff] +\
	_uByteWeight[((x) >> 16) & 0xff] + _uByteWeight[((x) >> 24) & 0xff] +\
	_uByteWeight[((x) >> 32) & 0xff] + _uByteWeight[((x) >> 40) & 0xff] +\
	_uByteWeight[((x) >> 48) & 0xff] + _uByteWeight[((x) >> 56) & 0xff])


u32 ANFTerm_IsZero(ANFTERM &a)
{
	u32 i;

	for(i = 0; i < ANFWEIGHT_WORDS; i++)
		if(a.val[i] != 0)
			return 0;

	return 1;
}

u32 ANFTerm_Covers(ANFTERM& a, ANFTERM& b)
{
	u32 i;

	for(i = 0; i < ANFWEIGHT_WORDS; i++)
	{
		if((~a.val[i]) & b.val[i])
			return 0;
	}

	return 1;
}


void ANFTerm_OR(ANFTERM &dst, ANFTERM &src)
{
	u32 a;

	for(a = 0; a < ANFWEIGHT_WORDS; a++)
		dst.val[a] |= src.val[a];
}

void ANFTerm_ANDNOT(ANFTERM &dst, ANFTERM &src1, ANFTERM &src2)
{
	u32 a;

	for(a = 0; a < ANFWEIGHT_WORDS; a++)
		dst.val[a] = src1.val[a] & ~(src2.val[a]);
}


u32 ANFTerm_Disjoint(ANFTERM& a, ANFTERM& b)
{
	u32 i;

	for(i = 0; i < ANFWEIGHT_WORDS; i++)
	{
		if(a.val[i] & b.val[i])
			return 0;
	}

	return 1;
}


u32 ANFTerm_Weight(ANFTERM &term)
{
	u32 uWeight = 0, a;

	for(a = 0; a < ANFWEIGHT_WORDS; a++)
		uWeight += U64WEIGHT(term.val[a]);

	return uWeight;
}

void ANFTerm_Clear(ANFTERM& term)
{
	memset(&term, 0, sizeof(ANFTERM));
}

void ANFTerm_Print(ANFTERM& term)
{
	u32 a, bFirst = 1;

	for(a = 0; a < ANFWEIGHT_N; a++)
	{
		if(ANFGETBIT(term, a))
		{
			if(!bFirst)
				printf(".");
			else
				bFirst = 0;

			printf("x%d", a + 1);
		}
	}

	// constant term?
	if(bFirst)
		printf("1");
}

u32 ANFTerm_Equal(ANFTERM& a, ANFTERM& b)
{
	u32 i;

	for(i = 0; i < ANFWEIGHT_WORDS; i++)
	{
		if(a.val[i] != b.val[i])
			return 0;
	}

	return 1;
}

u32 ANFTerm_Precedes(ANFTERM& a, ANFTERM& b)
{
	int i;

	for(i = ANFWEIGHT_WORDS - 1; i >= 0; i--)
	{
		if(a.val[i] != b.val[i])
		{
			if(a.val[i] > b.val[i])
				return 1;
			else
				return 0;
		}
	}

	return 0;
}


int ANFTermList_Insert(ANFTERMLIST &list, ANFTERM &newterm)
{
	ANFTERMLIST::iterator it1, it2;
	ANFTERM *pterm;
	u32 w, w2;

	w = ANFTerm_Weight(newterm);

	it1 = list.begin();
	it2 = list.end();

	while(it1 != it2)
	{
		pterm = (ANFTERM*)&(*it1);

		w2 = ANFTerm_Weight(*pterm);

		if(w2 > w)
		{
			list.insert(it1, newterm);
			return 1;
		}
		else if(w2 == w)
		{
			if(ANFTerm_Equal(newterm, *pterm))
			{
				list.erase(it1);
				return -1;
			}
			else if(ANFTerm_Precedes(newterm, *pterm))
			{
				list.insert(it1, newterm);
				return 1;
			}
		}

		++it1;
	}

	list.push_back(newterm);

	return 0;
}

void ANFTermList_Print(ANFTERMLIST& list)
{
	ANFTERMLIST::iterator it1, it2;

	it1 = list.begin();
	it2 = list.end();

	while(it1 != it2)
	{
		ANFTerm_Print(*it1);
		printf("\n");

		++it1;
	}
}

u32 ANFTermList_Exists(ANFTERMLIST& list, ANFTERM& term)
{
	ANFTERMLIST::iterator it1, it2;

	it1 = list.begin();
	it2 = list.end();

	while(it1 != it2)
	{
		if(ANFTerm_Equal(term, *it1))
			return 1;

		++it1;
	}

	return 0;
}




// Weight computation related
LINT ANFWeight_SubtreeLookup(u32 w)
{
	LINT result;

	// Assume k is odd
	LINT_2Pow(&result, ANFWEIGHT_N - w + 1);
	LINT_Negate(&result);

	return result;
}

LINT ANFWeight_Compute(ANFTERMLIST &func, u32 uDepth, ANFTERMLIST *plistIsolated)
{
	ANFTERMLIST listDisjoint;
	ANFTERMLIST::iterator it1, it2;
	ANFTERMVECTOR vecProcessed, vecSubset;
	ANFTERM CurrentTerm, UnionTerms, DisjointTerm;
	LINT Weight, S, T;
	u32 uCurrentTermWeight, a;
	ANFTERMLIST listIsolatedLocal;

	LINT_Zero(&Weight);

	ANFTerm_Clear(UnionTerms);

	if(func.size() > 2)
	{
		if(ANFWeight_SplitIsolatedTerms(func, listIsolatedLocal))
		{
			// check balancedness
			it1 = listIsolatedLocal.begin();
			uCurrentTermWeight = ANFTerm_Weight(*it1);
			if(uCurrentTermWeight == 1)
			{
				LINT_2Pow(&Weight, ANFWEIGHT_N - 1);
				return Weight;
			}
			else if(uCurrentTermWeight == 0 && listIsolatedLocal.size() > 1)
			{
				// skip constant term
				++it1;
				if(ANFTerm_Weight(*it1) == 1)
				{
					LINT_2Pow(&Weight, ANFWEIGHT_N - 1);
					return Weight;
				}
			}
		}
	}

	it1 = func.begin();
	it2 = func.end();
	while(it1 != it2)
	{
		if(uDepth == 1)
		{
			printf("processing term %d\n", vecProcessed.size() + 1);
			fflush(NULL);
		}

		CurrentTerm = (ANFTERM)*it1;

		uCurrentTermWeight = ANFTerm_Weight(CurrentTerm);

		if(ANFTerm_Disjoint(CurrentTerm, UnionTerms))
		{
			if(uCurrentTermWeight == 0)
			{
				// S = 2^n - 2W
				LINT_Zero(&S);
				LINT_Sub(&S, &Weight, &S);
				LINT_Sub(&S, &Weight, &S);
			}
			else
			{
				S = Weight;
				LINT_Shr(&S, uCurrentTermWeight - 1);
				LINT_Negate(&S);
			}
		}
		else
		{

			vecSubset.clear();
			listDisjoint.clear();

			for(a = 0; a < vecProcessed.size(); a++)
			{
				if(ANFTerm_Covers(CurrentTerm, vecProcessed[a]))
					vecSubset.push_back(vecProcessed[a]);
				else
				{
					ANFTerm_ANDNOT(DisjointTerm, vecProcessed[a], CurrentTerm);

					ANFTermList_Insert(listDisjoint, DisjointTerm);
				}
			}

			// Sum comming from the new subsets
			LINT_Zero(&S);

			if(!listDisjoint.empty())
			{
				S = ANFWeight_Compute(listDisjoint, uDepth + 1);

				LINT_Shr(&S, uCurrentTermWeight - 1);
				LINT_Negate(&S);
			}

			if(vecSubset.size() & 1)
			{
				LINT_Negate(&S);

				T = ANFWeight_SubtreeLookup(uCurrentTermWeight);

				LINT_Add(&S, &T, &S);
			}
		}

		// add the current term itself
		LINT_2Pow(&T, ANFWEIGHT_N - uCurrentTermWeight);
		LINT_Add(&S, &T, &S);

		// partial sum comming from current term
		LINT_Add(&Weight, &S, &Weight);
	
		vecProcessed.push_back(CurrentTerm);
		
		ANFTerm_OR(UnionTerms, CurrentTerm);

		++it1;
	}

	// Process isolated terms
	if(!listIsolatedLocal.empty())
	{
		it1 = listIsolatedLocal.begin();
		it2 = listIsolatedLocal.end();
		while(it1 != it2)
		{
			CurrentTerm = *it1;
			uCurrentTermWeight = ANFTerm_Weight(CurrentTerm);

			// Weight -= Weight / (2^{d-1})
			T = Weight;
			LINT_Shr(&T, uCurrentTermWeight);
			LINT_Shl1(&T);
			LINT_Negate(&T);
			LINT_Add(&Weight, &T, &Weight);

			// Weight += 2^{n-d}
			LINT_2Pow(&T, ANFWEIGHT_N - uCurrentTermWeight);
			LINT_Add(&Weight, &T, &Weight);

			++it1;
		}		
	}

	return Weight;
}


u32 ANFWeight_FindIsolatedTerm(ANFTERMLIST& func, ANFTERMLIST &Candidates)
{
	ANFTERMLIST::iterator it1, it2, it3, it4;
	ANFTERM CurrentTerm, UnionTerms;
	std::vector<u32> vecpos;


	ANFTerm_Clear(UnionTerms);

	if(func.size() < 2)
	{
		return Candidates.size();
	}

	it1 = func.begin();
	it2 = func.end();
	while(it1 != it2)
	{
		CurrentTerm = *it1;

		if(ANFTerm_Disjoint(CurrentTerm, UnionTerms))
		{
			if(ANFTerm_Weight(CurrentTerm) != 0)
				ANFTermList_Insert(Candidates, CurrentTerm);
		}
		else
		{
			it3 = Candidates.begin();
			it4 = Candidates.end();

			while(it3 != it4)
			{
				if(!ANFTerm_Disjoint(CurrentTerm, *it3))
				{
					it3 = Candidates.erase(it3);
				}
				else
					++it3;
			}
		}

		ANFTerm_OR(UnionTerms, CurrentTerm);
		++it1;
	}

	return Candidates.size();
}

u32 ANFWeight_SplitIsolatedTerms(ANFTERMLIST& func, ANFTERMLIST &Isolated)
{
	ANFTERMLIST::iterator it1, it2, it3, it4;
	ANFTERM CurrentTerm, UnionTerms;
	std::vector<u32> vecpos;


	if(func.size() < 2)
	{
		return 0;
	}

	ANFTerm_Clear(UnionTerms);

	it1 = func.begin();
	it2 = func.end();
	while(it1 != it2)
	{
		CurrentTerm = *it1;

		if(ANFTerm_Disjoint(CurrentTerm, UnionTerms))
		{
			//if(ANFTerm_Weight(CurrentTerm) != 0)
				ANFTermList_Insert(Isolated, CurrentTerm);
		}
		else
		{
			it3 = Isolated.begin();
			it4 = Isolated.end();

			while(it3 != it4)
			{
				if(!ANFTerm_Disjoint(CurrentTerm, *it3))
				{
					it3 = Isolated.erase(it3);
				}
				else
					++it3;
			}
		}

		ANFTerm_OR(UnionTerms, CurrentTerm);
		++it1;
	}

	// Remove isolated terms from func
	it1 = Isolated.begin();
	it2 = Isolated.end();
	while(it1 != it2)
	{
		CurrentTerm = *it1;

		it3 = func.begin();
		it4 = func.end();
		while(it3 != it4)
		{
			if(ANFTerm_Equal(CurrentTerm, *it3))
			{
				func.erase(it3);
				break;
			}

			++it3;
		}

		++it1;
	}
	
	return Isolated.size();
}




LINT ANFWeight_ComputeDisjoint(ANFTERMLIST& func)
{
	LINT Weight, S;
	ANFTERMLIST::iterator it1, it2;
	ANFTERM term;
	u32 uTermWeight;

	LINT_Zero(&Weight);

	it1 = func.begin();
	it2 = func.end();
	while(it1 != it2)
	{
		term = *it1;

		uTermWeight = ANFTerm_Weight(term); 

		// -2.Weight/2^wt(term)
		S = Weight;
		LINT_Shr(&S, uTermWeight - 1);
		LINT_Negate(&S);
		LINT_Add(&Weight, &S, &Weight);

		LINT_2Pow(&S, ANFWEIGHT_N - uTermWeight);
		LINT_Add(&Weight, &S, &Weight);

		++it1;
	}

	return Weight;
}

