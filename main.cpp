//
//
// file   : main.cpp
// desc.  : command line parser for anfweight
//
// author : Cagdas Calik, ccalik@metu.edu.tr
// date   : 14.03.2012
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "anfweight.h"


bool IsDigit(char ch)
{
	if(ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}

const char* SkipWhitespace(const char *str)
{
	const char *ret = str;

	while(*ret == ' ' || *ret == '\t' || *ret == '\r' || *ret == '\n')
		ret++;

	if(*ret == '\0')
		ret = NULL;

	return ret;
}

int Str2ANFTermList(const char *szANF, ANFTERMLIST &monomials, int &N)
{
	ANFTERM term;
	char szIndex[16];
	const char *str, *end;
	bool bInTerm = false;
	int i, index;
	N = 0;

	ANFTerm_Clear(term);

	str = SkipWhitespace(szANF);

	while(str != NULL)
	{

		if(*str == '+' || *str == '^')
		{
			if(bInTerm)
			{
				monomials.push_back(term);
				ANFTerm_Clear(term);
				bInTerm = false;
			}

			str++;
		}
		else if(*str == '1')
		{
			// constant term
			monomials.push_back(term);
			str++;
		}
		else if(*str == 'x')
		{
			if(!bInTerm)
			{
				bInTerm = true;
				ANFTerm_Clear(term);
			}

			end = str + 1;
			i = 0;
			while(IsDigit(*end))
				szIndex[i++] = *end++;

			str = end;

			szIndex[i] = 0;

			index = atoi(szIndex);

			if(index > N)
				N = index;

			ANFSETBIT(term, index - 1);
		}
		else
		{
			// skip '.'
			str++;
		}

		str = SkipWhitespace(str);
	}

	if(bInTerm)
		monomials.push_back(term);

	return 0;
}

void PrintUsage()
{
	printf("anf2weight v1.1 (March 2012) - Cagdas Calik\n");
	printf("\n");
	printf("\n");
	printf("Usage: anf2weight [-n N] [-f filename] | [anf] [-p] ");
	printf("\n");
	printf(" -n N        : declare that the boolean function is of N variables\n");
	printf(" -f filename : read ANF from the file\n");
	printf(" -p          : show progress\n");
	printf("\n");
	printf("Example1: anf2weight x1.x2+x3.x4\n");
	printf("Example2: anf2weight -n 8 x1.x2+x3.x4\n");
	printf("Example3: anf2weight -f anf.txt\n");
	printf("Example4: anf2weight -n 64 -f anf.txt -p\n");
	printf("\n");
}

int ParseCmdLine(int argc, char* argv[])
{
	int		i;
	int		N = -1;				// number of variables declared by user
	int		n;					// number of variabes appearing in the anf
	bool	bParseError = false;// indicates a parsing error
	bool	bProgress = false;	// show progress
	bool	bFromFile = false;	// read function anf from file
	char	szFileName[260];	// input file
	char	*pFileBuffer = NULL;// file contents
	long	lFileLength;		// length of the input file
	int		nANFArgIndex = -1;	// Argument index of anf if it is given from cmdline
	ANFTERMLIST monomials;		// function's and
	FILE	*fp = NULL;
	LINT	weight;				// Result

	for(i = 1; i < argc; i++)
	{
		// number of variables
		if(strcmp(argv[i], "-n") == 0)
		{
			if((i + 1) < argc)
			{
				N = atoi(argv[i + 1]);

				if(N > 128)
				{
					bParseError = true;
					printf("error: declared number of input variables cannot exceed 128.\n");
				}

				i++;
			}
			else
			{
				bParseError = true;
				printf("missing parameter after %s\n", argv[i]);
			}
		}
		else if(strcmp(argv[i], "-p") == 0)
		{
			bProgress = true;
		}
		else if(strcmp(argv[i], "-f") == 0)
		{
			if((i + 1) < argc)
			{
				strcpy(szFileName, argv[i + 1]);
				bFromFile = true;
				i++;
			}
			else
			{
				bParseError = true;
				printf("missing parameter after %s\n", argv[i]);
			}
		}
		else
		{
			if(nANFArgIndex == -1)
				nANFArgIndex = i;
			else
			{
				// multiple arguments for anf
				printf("multiple anf definition in arg. %d, remove blank spaces from the anf.\n", i + 1);
				bParseError = true;
			}
		}
	}

	if(bFromFile)
	{
		fp = fopen(szFileName, "rb");

		if(fp == NULL)
		{
			printf("cannot open file: %s\n", szFileName);
			bParseError = true;
		}
		else
		{
			// compute file length
			fseek(fp, 0, SEEK_END);
			lFileLength = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			pFileBuffer = new char[lFileLength];

			if(pFileBuffer == NULL)
			{
				printf("cannot allocate memory for file : %s\n", szFileName);
				bParseError = true;
			}
			else
			{
				// v1.1
				fread(pFileBuffer, 1, lFileLength, fp);

				// parse anf from the file
				if(Str2ANFTermList(pFileBuffer, monomials, n) != 0)
					bParseError = true;
			}
		}
	}
	else
	{
		// parse anf from command line
		if(Str2ANFTermList(argv[nANFArgIndex], monomials, n) != 0)
			bParseError = true;
	}


	if(fp)
		fclose(fp);

	if(pFileBuffer) 
		delete [] pFileBuffer;

	if(!bParseError)
	{
		// Check maximum number of variables supported
		if(n > 128)
		{
			printf("error: maximum supported number of variables is 128.\n");
		}
		else if((N != -1) && (N < n))
		{
			// Check if number of variables in the anf complies with users declared number of variables
			printf("error: declared number of variables is less than the one in the ANF.\n");
		}
		else
		{
			if(N != -1)
				n = N;

			// Compute the weight
			weight = ANFWeight_Compute(monomials, bProgress ? 1 : 2, NULL);

			// Normalize the weight if number of variables is less than 128
			// since the weight is computed as if the function contains 128 variables
			if(n < ANFWEIGHT_N)
			{
				LINT_Shr(&weight, ANFWEIGHT_N - n);
			}

			LINT_Print(&weight);
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		PrintUsage();
		return -1;
	}

	ParseCmdLine(argc, argv);

	return 0;
}