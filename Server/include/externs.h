#ifndef EXTERNS_H
#define EXTERNS_H

#include "defs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                   G L O B A L   D A T A   S T R U C T U R E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Bit stream */
extern BitSequence *epsilon;

/* Test parameter structure */
extern TP tp;

/* File output streams */
extern FILE *stats[NUMOFTESTS + 1];
extern FILE *results[NUMOFTESTS + 1];
extern FILE *freqfp;
extern FILE *summary;

/* Test configuration */
extern int testVector[NUMOFTESTS + 1];

/* Generator and test names */
extern char generatorDir[NUMOFGENERATORS][20];
extern char testNames[NUMOFTESTS + 1][32];

#endif /* EXTERNS_H */
