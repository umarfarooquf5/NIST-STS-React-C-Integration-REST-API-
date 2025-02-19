/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
U T I L I T I E S
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "externs.h"
#include "utilities.h"
#include "generators.h"
#include "stat_fncs.h"

using namespace System;
using namespace System::Threading;

int generatorOptions(char **streamFile, int option, char *fileName)
{
	FILE *fp;

	switch (option)
	{
	case 0:
		*streamFile = (char *)calloc(200, sizeof(char));
		sprintf(*streamFile, "%s", fileName);
		if ((fp = fopen(*streamFile, "r")) == NULL)
		{
			throw gcnew Exception("File Error: file could not be opened.");
		}
		else
			fclose(fp);
		break;
	case 1:
		*streamFile = "Linear-Congruential";
		break;
	case 2:
		*streamFile = "Quadratic-Congruential-1";
		break;
	case 3:
		*streamFile = "Quadratic-Congruential-2";
		break;
	case 4:
		*streamFile = "Cubic-Congruential";
		break;
	case 5:
		*streamFile = "XOR";
		break;
	case 6:
		*streamFile = "Modular-Exponentiation";
		break;
	case 7:
		*streamFile = "Blum-Blum-Shub";
		break;
	case 8:
		*streamFile = "Micali-Schnorr";
		break;
	case 9:
		*streamFile = "G using SHA-1";
		break;
	default:
		break;
	}
	return option;
}

void fileBasedBitStreams(char *streamFile, int mode)
{
	FILE *fp;
	if (mode == 0)
	{
		if ((fp = fopen(streamFile, "r")) == NULL)
		{
			throw gcnew Exception("ERROR IN FUNCTION fileBasedBitStreams: file could not be opened.");
		}
		readBinaryDigitsInASCIIFormat(fp, streamFile);
		fclose(fp);
	}
	else if (mode == 1)
	{
		if ((fp = fopen(streamFile, "rb")) == NULL)
		{
			throw gcnew Exception("ERROR IN FUNCTION fileBasedBitStreams: file could not be opened.");
		}
		readHexDigitsInBinaryFormat(fp);
		fclose(fp);
	}
}

void readBinaryDigitsInASCIIFormat(FILE *fp, char *streamFile)
{
	int i, j, num_0s, num_1s, bitsRead, bit;

	if ((epsilon = (BitSequence *)calloc(tp.n, sizeof(BitSequence))) == NULL)
	{
		throw gcnew Exception("BITSTREAM DEFINITION: Insufficient memory available.");
	}

	for (i = 0; i < tp.numOfBitStreams; i++)
	{
		num_0s = 0;
		num_1s = 0;
		bitsRead = 0;
		for (j = 0; j < tp.n; j++)
		{
			if (fscanf(fp, "%1d", &bit) == EOF)
			{
				fclose(fp);
				free(epsilon);
				throw gcnew Exception("ERROR: Insufficient data in file.");
			}
			else
			{
				bitsRead++;
				if (bit == 0)
					num_0s++;
				else
					num_1s++;
				epsilon[j] = bit;
			}
		}
		nist_test_suite();
	}
	free(epsilon);
}

void readHexDigitsInBinaryFormat(FILE *fp)
{
	int i, done, num_0s, num_1s, bitsRead;
	BYTE buffer[4];

	if ((epsilon = (BitSequence *)calloc(tp.n, sizeof(BitSequence))) == NULL)
	{
		throw gcnew Exception("BITSTREAM DEFINITION: Insufficient memory available.");
	}

	for (i = 0; i < tp.numOfBitStreams; i++)
	{
		num_0s = 0;
		num_1s = 0;
		bitsRead = 0;
		done = 0;
		do
		{
			if (fread(buffer, sizeof(unsigned char), 4, fp) != 4)
			{
				free(epsilon);
				throw gcnew Exception("READ ERROR: Insufficient data in file.");
			}
			done = convertToBits(buffer, 32, tp.n, &num_0s, &num_1s, &bitsRead);
		} while (!done);
		nist_test_suite();
	}
	free(epsilon);
}

int convertToBits(BYTE *x, int xBitLength, int bitsNeeded, int *num_0s, int *num_1s, int *bitsRead)
{
	int i, j, count, bit;
	BYTE mask;
	int zeros, ones;

	count = 0;
	zeros = ones = 0;
	for (i = 0; i < (xBitLength + 7) / 8; i++)
	{
		mask = 0x80;
		for (j = 0; j < 8; j++)
		{
			if (*(x + i) & mask)
			{
				bit = 1;
				(*num_1s)++;
				ones++;
			}
			else
			{
				bit = 0;
				(*num_0s)++;
				zeros++;
			}
			mask >>= 1;
			epsilon[*bitsRead] = bit;
			(*bitsRead)++;
			if (*bitsRead == bitsNeeded)
				return 1;
			if (++count == xBitLength)
				return 0;
		}
	}

	return 0;
}

void openOutputStreams(int option)
{
	int i, numOfOpenFiles = 0;
	char freqfn[200], summaryfn[200], statsDir[200], resultsDir[200];

	sprintf(freqfn, "experiments/%s/freq.txt", generatorDir[option]);
	if ((freqfp = fopen(freqfn, "w")) == NULL)
	{
		throw gcnew Exception("MAIN: Could not open freq file.");
	}

	sprintf(summaryfn, "experiments/%s/finalAnalysisReport.txt", generatorDir[option]);
	if ((summary = fopen(summaryfn, "w")) == NULL)
	{
		throw gcnew Exception("MAIN: Could not open stats file.");
	}

	for (i = 1; i <= NUMOFTESTS; i++)
	{
		if (testVector[i] == 1)
		{
			sprintf(statsDir, "experiments/%s/%s/stats.txt", generatorDir[option], testNames[i]);
			sprintf(resultsDir, "experiments/%s/%s/results.txt", generatorDir[option], testNames[i]);

			if ((stats[i] = fopen(statsDir, "w")) == NULL || (results[i] = fopen(resultsDir, "w")) == NULL)
			{
				throw gcnew Exception("ERROR: LOG FILES COULD NOT BE OPENED.");
			}
			numOfOpenFiles++;
		}
	}
}

void nist_test_suite()
{
	array<Thread ^> ^ tr = gcnew array<Thread ^>(15);
	(tr[0] = gcnew Thread(gcnew ThreadStart(&_LinearComplexity)))->Start();
	(tr[1] = gcnew Thread(gcnew ThreadStart(&_LongestRunOfOnes)))->Start();
	(tr[2] = gcnew Thread(gcnew ThreadStart(&_NonOverlappingTemplateMatchings)))->Start();
	(tr[3] = gcnew Thread(gcnew ThreadStart(&_OverlappingTemplateMatchings)))->Start();
	(tr[4] = gcnew Thread(gcnew ThreadStart(&_RandomExcursions)))->Start();
	(tr[5] = gcnew Thread(gcnew ThreadStart(&_RandomExcursionsVariant)))->Start();
	(tr[6] = gcnew Thread(gcnew ThreadStart(&_Rank)))->Start();
	(tr[7] = gcnew Thread(gcnew ThreadStart(&_Runs)))->Start();
	(tr[8] = gcnew Thread(gcnew ThreadStart(&_Serial)))->Start();
	(tr[9] = gcnew Thread(gcnew ThreadStart(&_Universal)))->Start();
	(tr[10] = gcnew Thread(gcnew ThreadStart(&_ApproximateEntropy)))->Start();
	(tr[11] = gcnew Thread(gcnew ThreadStart(&_BlockFrequency)))->Start();
	(tr[12] = gcnew Thread(gcnew ThreadStart(&_CumulativeSums)))->Start();
	(tr[13] = gcnew Thread(gcnew ThreadStart(&_DiscreteFourierTransform)))->Start();
	(tr[14] = gcnew Thread(gcnew ThreadStart(&_Frequency)))->Start();

	for (int i = 0; i < 15; i++)
		tr[i]->Join();
}
