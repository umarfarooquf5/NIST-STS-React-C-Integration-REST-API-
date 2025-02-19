/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
              U T I L I T Y  F U N C T I O N  P R O T O T Y P E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef UTILITIES_H
#define UTILITIES_H

#include "../NIST_UI/assess.h"

// Function Prototypes
int generatorOptions(char **streamFile, int option, char *fileName);
void fileBasedBitStreams(char *streamFile, int mode);
void readBinaryDigitsInASCIIFormat(FILE *fp, char *streamFile);
void readHexDigitsInBinaryFormat(FILE *fp);
int convertToBits(BYTE *x, int xBitLength, int bitsNeeded, int *num_0s, int *num_1s, int *bitsRead);
void openOutputStreams(int option);
void invokeTestSuite(int option, char *streamFile, int modeReadFile);
void nist_test_suite();
void SetInitNISTParam(InitNISTParam *init);

#endif // UTILITIES_H
