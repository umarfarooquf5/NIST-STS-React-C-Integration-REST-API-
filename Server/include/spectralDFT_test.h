#ifndef SPECTRALDFT_TEST_H
#define SPECTRALDFT_TEST_H

#include <vector>
#include <string>

#include "dfft.h" // Include the DFFT functions

void __ogg_fdrffti(int n, double *wsave, int *ifac);
void __ogg_fdrfftf(int n, double *X, double *wsave, int *ifac);

double SpectralDFTCalculation(int M, const std::vector<int> &epsilon, int n);
std::string SpectralDFTTest(const std::string &binaryFilePath, int M, int numberOfSegments);

#endif // SPECTRALDFT_TEST_H
