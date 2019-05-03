//
//
// file   : lint.h
// desc.  : large integer class interface
//
// author : Cagdas Calik, ccalik@metu.edu.tr
// date   : 14.03.2012
//


#ifndef LINT_H
#define LINT_H

#include "types.h"

//#define LINT_WORDS	2
#define WORD_BITS (8 * sizeof(u64))

typedef struct _tagLINT
{
	u64	words[2];

} LINT;

#define LINT_GETBIT(l, x) (((l)->words[(x) / 64] >> ((x) % 64)) & 1)
#define LINT_SETBIT(l, x) ((l)->words[(x) / 64] |= ((u64)1 << ((x) % 64)))

void LINT_Zero(LINT *a);
void LINT_Set(LINT *a, u64 val);
void LINT_Add(LINT *a, LINT *b, LINT *c);
void LINT_Sub(LINT *a, LINT *b, LINT *c);
void LINT_Shl1(LINT *a);
void LINT_Shr(LINT *a, u32 x);
void LINT_Negate(LINT *a);
void LINT_2Pow(LINT *a, u32 x);
void LINT_Print(LINT *a);

#endif // LINT_H
