#include "linearcomplexity_test.h"
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

using json = nlohmann::json;

double LinearComplexityCalculation(int M, const std::vector<int> &epsilon, int n);

double LinearComplexityCalculation(int M, const std::vector<int> &epsilon, int n)
{
    int i, ii, j, d, N, L, m, N_, parity, sign, K = 6;
    double p_value, T_, mean, nu[7], chi2;
    double pi[7] = {0.01047, 0.03125, 0.12500, 0.50000, 0.25000, 0.06250, 0.020833};
    BitSequence *T = NULL, *P = NULL, *B_ = NULL, *C = NULL;

    N = (int)floor((double)n / M);
    if (((B_ = (BitSequence *)calloc(M, sizeof(BitSequence))) == NULL) ||
        ((C = (BitSequence *)calloc(M, sizeof(BitSequence))) == NULL) ||
        ((P = (BitSequence *)calloc(M, sizeof(BitSequence))) == NULL) ||
        ((T = (BitSequence *)calloc(M, sizeof(BitSequence))) == NULL))
    {

        if (B_ != NULL)
            free(B_);
        if (C != NULL)
            free(C);
        if (P != NULL)
            free(P);
        if (T != NULL)
            free(T);

        throw std::runtime_error("Insufficient Memory for Work Space: Linear Complexity Test");
    }

    for (i = 0; i < K + 1; i++)
        nu[i] = 0.00;

    for (ii = 0; ii < N; ii++)
    {
        for (i = 0; i < M; i++)
        {
            B_[i] = 0;
            C[i] = 0;
            T[i] = 0;
            P[i] = 0;
        }
        L = 0;
        m = -1;
        d = 0;
        C[0] = 1;
        B_[0] = 1;

        N_ = 0;
        while (N_ < M)
        {
            d = (int)epsilon[ii * M + N_];

            for (i = 1; i <= L; i++)
                d += C[i] * epsilon[ii * M + N_ - i];

            d = d % 2;
            if (d == 1)
            {
                for (i = 0; i < M; i++)
                {
                    T[i] = C[i];
                    P[i] = 0;
                }

                for (j = 0; j < M; j++)
                    if (B_[j] == 1)
                        P[j + N_ - m] = 1;

                for (i = 0; i < M; i++)
                    C[i] = (C[i] + P[i]) % 2;

                if (L <= N_ / 2)
                {
                    L = N_ + 1 - L;
                    m = N_;
                    for (i = 0; i < M; i++)
                        B_[i] = T[i];
                }
            }
            N_++;
        }

        if ((parity = (M + 1) % 2) == 0)
            sign = -1;
        else
            sign = 1;

        mean = M / 2.0 + (9.0 + sign) / 36.0 - 1.0 / pow((double)2, M) * (M / 3.0 + 2.0 / 9.0);

        if ((parity = M % 2) == 0)
            sign = 1;
        else
            sign = -1;

        T_ = sign * (L - mean) + 2.0 / 9.0;

        if (T_ <= -2.5)
            nu[0]++;
        else if (T_ > -2.5 && T_ <= -1.5)
            nu[1]++;
        else if (T_ > -1.5 && T_ <= -0.5)
            nu[2]++;
        else if (T_ > -0.5 && T_ <= 0.5)
            nu[3]++;
        else if (T_ > 0.5 && T_ <= 1.5)
            nu[4]++;
        else if (T_ > 1.5 && T_ <= 2.5)
            nu[5]++;
        else
            nu[6]++;
    }

    chi2 = 0.00;
    for (i = 0; i < K + 1; i++)
        chi2 += pow(nu[i] - N * pi[i], 2) / (N * pi[i]);

    p_value = cephes_igamc(K / 2.0, chi2 / 2.0);
    return p_value;
}

std::string LinearComplexityTest(const std::string &binaryFilePath, int M, int numberOfSegments)
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
        double p_value = LinearComplexityCalculation(500, segment, segment.size());
        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Linear Complexity Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    // Ensure uploads directory exists
    if (!std::filesystem::exists("uploads"))
    {
        std::filesystem::create_directory("uploads");
    }
    std::string resultFilePath = "uploads/linearcomplexity_result.json";
    std::ofstream resultFile(resultFilePath);
    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Linear Complexity Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
