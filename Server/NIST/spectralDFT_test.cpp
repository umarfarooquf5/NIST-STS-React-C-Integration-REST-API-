#include "spectralDFT_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cephes.h"
#include "defs.h"
#include "dfft.h" // Include the DFFT functions

using json = nlohmann::json;

double SpectralDFTCalculation(int M, const std::vector<int> &epsilon, int n)
{
    double p_value, upperBound, percentile, N_l, N_o, d, *m = NULL, *X = NULL, *wsave = NULL;
    int i, count, ifac[15];

    if (((X = (double *)calloc(n, sizeof(double))) == NULL) ||
        ((wsave = (double *)calloc(2 * n, sizeof(double))) == NULL) ||
        ((m = (double *)calloc(n / 2 + 1, sizeof(double))) == NULL))
    {
        throw std::runtime_error("Insufficient Memory for Work Space: Spectral DFT Test");
    }

    for (i = 0; i < n; i++)
        X[i] = 2 * (int)epsilon[i] - 1;

    __ogg_fdrffti(n, wsave, ifac);
    __ogg_fdrfftf(n, X, wsave, ifac);

    m[0] = sqrt(X[0] * X[0]);
    for (i = 0; i < n / 2; i++)
        m[i + 1] = sqrt(pow(X[2 * i + 1], 2) + pow(X[2 * i + 2], 2));

    count = 0;
    upperBound = sqrt(2.995732274 * n);
    for (i = 0; i < n / 2; i++)
        if (m[i] < upperBound)
            count++;

    percentile = (double)count / (n / 2) * 100;
    N_l = (double)count;
    N_o = (double)0.95 * n / 2.0;
    d = (N_l - N_o) / sqrt(n / 4.0 * 0.95 * 0.05);
    p_value = erfc(fabs(d) / sqrt(2.0));
    return p_value;
}

std::string SpectralDFTTest(const std::string &binaryFilePath, int M, int numberOfSegments)
{
    std::ifstream file(binaryFilePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open binary file: " + binaryFilePath);
    }

    std::vector<int> data;
    char byte;
    while (file.read(&byte, 1))
    {
        for (int i = 7; i >= 0; --i)
        {
            data.push_back((byte >> i) & 1);
        }
    }
    file.close();

    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        std::vector<int> segment(data.begin() + i * M, data.begin() + std::min((i + 1) * M, (int)data.size()));
        double p_value = SpectralDFTCalculation(M, segment, segment.size());
        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Spectral DFT Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    if (!std::filesystem::exists("uploads"))
    {
        std::filesystem::create_directory("uploads");
    }
    std::string resultFilePath = "uploads/spectralDFT_result.json";
    std::ofstream resultFile(resultFilePath);
    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Spectral DFT Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
