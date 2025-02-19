#include "rank_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"
#include "matrix.h"

using json = nlohmann::json;

// Function to calculate Rank Test for a given segment
double RankTestSegment(const std::vector<int> &epsilon)
{
    int N, i, k, r;
    double p_value, product, chi_squared, arg1, p_32, p_31, p_30, R, F_32, F_31, F_30;
    BitSequence **matrix = create_matrix(32, 32);

    N = epsilon.size() / (32 * 32);
    if (isZero(N))
    {
        p_value = 0.00;
    }
    else
    {
        r = 32;
        product = 1;
        for (i = 0; i < r; i++)
            product *= ((1.0 - pow(2.0, i - 32)) * (1.0 - pow(2.0, i - 32))) / (1.0 - pow(2.0, i - r));
        p_32 = pow(2.0, r * (64 - r) - 1024) * product;

        r = 31;
        product = 1;
        for (i = 0; i < r; i++)
            product *= ((1.0 - pow(2.0, i - 32)) * (1.0 - pow(2.0, i - 32))) / (1.0 - pow(2.0, i - r));
        p_31 = pow(2.0, r * (64 - r) - 1024) * product;

        p_30 = 1 - (p_32 + p_31);

        F_32 = 0;
        F_31 = 0;
        for (k = 0; k < N; k++)
        {
            BitSequence *epsilonArray = new BitSequence[epsilon.size()];
            for (size_t i = 0; i < epsilon.size(); ++i)
                epsilonArray[i] = static_cast<BitSequence>(epsilon[i]);

            def_matrix(32, 32, matrix, k, epsilonArray);
            R = computeRank(32, 32, matrix);

            delete[] epsilonArray;

            if (R == 32)
                F_32++;
            if (R == 31)
                F_31++;
        }
        F_30 = (double)N - (F_32 + F_31);

        chi_squared = (pow(F_32 - N * p_32, 2) / (N * p_32) +
                       pow(F_31 - N * p_31, 2) / (N * p_31) +
                       pow(F_30 - N * p_30, 2) / (N * p_30));

        arg1 = -chi_squared / 2.0;
        p_value = exp(arg1);

        for (i = 0; i < 32; i++)
            free(matrix[i]);
        free(matrix);
    }
    return p_value;
}

// Main Function to Handle the Rank Test
std::string RankTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
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

    if (data.size() < segmentSize)
    {
        throw std::runtime_error("Error: Insufficient data for the segment size requested.");
    }

    json results = json::array();
    time_t now = time(nullptr);

    // int numberOfSegments = data.size() / segmentSize;

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * segmentSize;
        int end = std::min(start + segmentSize, (int)data.size());

        std::vector<int> segment(data.begin() + start, data.begin() + end);
        double p_value = RankTestSegment(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Rank Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    if (!std::filesystem::exists("uploads"))
    {
        std::filesystem::create_directory("uploads");
    }

    std::string resultFilePath = "uploads/ranktest_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save rank test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Rank Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
