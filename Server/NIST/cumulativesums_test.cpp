#include "cumulativesums_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

// Function to compute Cumulative Sums Test for one segment
double ComputeCumulativeSums(const std::vector<int> &epsilon)
{
    int S = 0, sup = 0, inf = 0, z, zrev, k, n = epsilon.size();
    double sum1, sum2, p_value;

    // Forward computation
    for (k = 0; k < n; k++)
    {
        epsilon[k] ? S++ : S--;
        if (S > sup)
            sup++;
        if (S < inf)
            inf--;
        z = (sup > -inf) ? sup : -inf;
        zrev = (sup - S > S - inf) ? sup - S : S - inf;
    }

    // Forward p-value calculation
    sum1 = 0.0;
    for (k = (-n / z + 1) / 4; k <= (n / z - 1) / 4; k++)
    {
        sum1 += cephes_normal(((4 * k + 1) * z) / sqrt((double)n));
        sum1 -= cephes_normal(((4 * k - 1) * z) / sqrt((double)n));
    }

    sum2 = 0.0;
    for (k = (-n / z - 3) / 4; k <= (n / z - 1) / 4; k++)
    {
        sum2 += cephes_normal(((4 * k + 3) * z) / sqrt((double)n));
        sum2 -= cephes_normal(((4 * k + 1) * z) / sqrt((double)n));
    }

    p_value = 1.0 - sum1 + sum2;

    // Backward p-value calculation
    sum1 = 0.0;
    for (k = (-n / zrev + 1) / 4; k <= (n / zrev - 1) / 4; k++)
    {
        sum1 += cephes_normal(((4 * k + 1) * zrev) / sqrt((double)n));
        sum1 -= cephes_normal(((4 * k - 1) * zrev) / sqrt((double)n));
    }

    sum2 = 0.0;
    for (k = (-n / zrev - 3) / 4; k <= (n / zrev - 1) / 4; k++)
    {
        sum2 += cephes_normal(((4 * k + 3) * zrev) / sqrt((double)n));
        sum2 -= cephes_normal(((4 * k + 1) * zrev) / sqrt((double)n));
    }

    p_value = 1.0 - sum1 + sum2;
    return p_value;
}

// Driver function to process Cumulative Sums Test
std::string CumulativeSumsTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
{
    // Open and read binary file
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

    if (data.size() < segmentSize * numberOfSegments)
    {
        throw std::runtime_error("Error: Insufficient data for the number of segments and block size requested.");
    }

    json results = json::array();
    time_t now = time(nullptr);

    // Process each segment
    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * segmentSize;
        int end = std::min(start + segmentSize, (int)data.size());

        if (start >= data.size())
        {
            break;
        }

        std::vector<int> segment(data.begin() + start, data.begin() + end);
        double p_value = ComputeCumulativeSums(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Cumulative Sums Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    // Ensure uploads directory exists
    if (!std::filesystem::exists("uploads"))
    {
        std::filesystem::create_directory("uploads");
    }

    // Save results
    std::string resultFilePath = "uploads/cumulativesums_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Cumulative Sums test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Cumulative Sums Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
