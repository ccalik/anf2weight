//
//
// file   : lint.cpp
// desc.  : large integer class implementation for 128-bit arithmetic
//
// author : Cagdas Calik, ccalik@metu.edu.tr
// date   : 14.03.2012
//


#include "lint.h"
#include <memory.h>
#include <stdio.h>


void LINT_Zero(LINT *a)
{
	a->words[0] = 0;
	a->words[1] = 0;
}

void LINT_Set(LINT *a, u64 val)
{
	a->words[0] = val;
	a->words[1] = 0;
}


void LINT_Add(LINT *a, LINT *b, LINT *c)
{

	c->words[0] = a->words[0] + b->words[0];
	c->words[1] = a->words[1] + b->words[1];

	// carry
	if((c->words[0] < a->words[0]) || (c->words[0] < b->words[0]))
		c->words[1]++;
}

void LINT_Sub(LINT *a, LINT *b, LINT *c)
{
	c->words[0] = a->words[0] - b->words[0];
	c->words[1] = a->words[1] - b->words[1];

	// borrow
	if((c->words[0] > a->words[0]) && (c->words[0] > b->words[0]))
		c->words[1]--;
}

void LINT_Shl1(LINT *a)
{
	u64 msb;

	msb = a->words[0] >> 63;

	a->words[1] = (a->words[1] << 1) ^ msb;
	a->words[0] <<= 1;
}

void LINT_Shr(LINT *a, u32 x)
{
	u64 highpart;

	if(x == 0)
		return;

	if(x >= 64)
	{
		x -= 64;
		a->words[0] = a->words[1] >> x;
		a->words[1] = 0;
	}
	else
	{
		highpart = a->words[1] << (64 - x);
		a->words[1] >>= x;
		a->words[0] = (a->words[0] >> x) ^ highpart;
	}
}

void LINT_Negate(LINT *a)
{
	a->words[0] = ~a->words[0];
	a->words[1] = ~a->words[1];

	// increment
	a->words[0]++;

	if(a->words[0] == 0)
		a->words[1]++;
}

void LINT_2Pow(LINT *a, u32 x)
{
	a->words[0] = 0;
	a->words[1] = 0;

	a->words[x / WORD_BITS] ^= (u64)1 << (x % WORD_BITS);
}

void LINT_Print(LINT *a)
{
	//printf("%I64u:%I64u\n", a->words[1], a->words[0]);

	int index = 0, i = 0;
	char buffer[64];
	LINT t, q, r;

	if(a->words[0] == 0 && a->words[1] == 0)
	{
		printf("0\n");
		return;
	}

	t = *a;

	do
	{
		LINT_Zero(&q);
		LINT_Zero(&r);

		for(i = 127; i >= 0; i--)
		{
			LINT_Shl1(&r);
			r.words[0] |= LINT_GETBIT(&t, i);

			if(r.words[0] >= 10)
			{
				r.words[0] -= 10;
				LINT_SETBIT(&q, i);
			}
		}

		buffer[index++] = r.words[0] + '0';

		t = q;

	}while(!(q.words[0] == 0 && q.words[1] == 0));

	// reverse print
	for(i = index - 1; i >= 0; i--)
		printf("%c", buffer[i]);

	printf("\n");
}
