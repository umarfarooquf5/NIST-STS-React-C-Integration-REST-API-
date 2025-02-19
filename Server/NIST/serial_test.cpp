#include "serial_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

double psi2(int m, const std::vector<int> &epsilon, int n);

// Optimized Psi2 Function for Large m

double psi2(int m, const std::vector<int> &epsilon, int n)
{
    int i, j, k, powLen;
    double sum, numOfBlocks;
    unsigned int *P;

    if ((m == 0) || (m == -1))
        return 0.0;

    // numOfBlocks = n;
    powLen = (int)pow(2.0, m + 1) - 1;

    if ((P = (unsigned int *)calloc(powLen, sizeof(unsigned int))) == NULL)
    {
        // fprintf(stats[TEST_SERIAL], "Serial Test: Insufficient memory available.\n");
        // fflush(stats[TEST_SERIAL]);
        return 0.0;
    }

    for (i = 1; i < powLen - 1; i++)
        P[i] = 0; // Initialize nodes

    for (i = 0; i < n; i++) // Compute frequency
    {
        k = 1;
        for (j = 0; j < m; j++)
        {
            if (epsilon[(i + j) % n] == 0)
                k *= 2;
            else if (epsilon[(i + j) % n] == 1)
                k = 2 * k + 1;
        }
        P[k - 1]++;
    }

    sum = 0.0;
    for (i = (int)pow(2.0, m) - 1; i < (int)pow(2.0, m + 1) - 1; i++)
        sum += pow((double)P[i], 2);

    sum = (sum * pow(2.0, m) / (double)n) - (double)n;

    free(P);
    return sum;
}

// Serial Test Segment
double SerialTestSegment(int m, const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    if (n == 0)
    {
        std::cerr << "Error: Empty epsilon vector in SerialTestSegment." << std::endl;
        return 0.0;
    }

    // Debugging: Log input values
    // std::cout << "SerialTestSegment called with m = " << m << ", epsilon size = " << n << std::endl;

    double psim0 = psi2(m, epsilon, n);
    double psim1 = psi2(m - 1, epsilon, n);
    double psim2 = psi2(m - 2, epsilon, n);

    // Debugging: Log intermediate psi values
    // std::cout << "psi2 values: psim0 = " << psim0 << ", psim1 = " << psim1 << ", psim2 = " << psim2 << std::endl;

    double del1 = psim0 - psim1;
    double del2 = psim0 - 2.0 * psim1 + psim2;

    // Debugging: Log deltas
    if (std::isnan(del1) || std::isnan(del2) || del1 < 0 || del2 < 0)
    {
        std::cerr << "Error: Invalid delta values (del1 or del2) in SerialTestSegment." << std::endl;
        return 0.0;
    }

    // std::cout << "Deltas: del1 = " << del1 << ", del2 = " << del2 << std::endl;

    double p_value1 = cephes_igamc(pow(2.0, m - 1) / 2, del1 / 2.0);
    double p_value2 = cephes_igamc(pow(2.0, m - 2) / 2, del2 / 2.0);

    // Debugging: Log p-values
    if (std::isnan(p_value1) || std::isnan(p_value2))
    {
        std::cerr << "Error: Invalid p-value calculated by cephes_igamc." << std::endl;
        return 0.0;
    }

    // std::cout << "Segment P-values: p_value1 = " << p_value1 << ", p_value2 = " << p_value2 << std::endl;

    return p_value1;
}

// Main Function to Handle the Serial Test
std::string SerialTest(const std::string &binaryFilePath, int m, int numberOfSegments)
{
    // std::cout << "Starting SerialTest with binary file: " << binaryFilePath << std::endl;

    // Read binary file
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
    // std::cout << "Binary file read successfully. Total bits: " << data.size() << std::endl;

    if (data.size() < m * numberOfSegments)
    {
        throw std::runtime_error("Error: Insufficient data for the number of segments and block size requested.");
    }

    // Prepare results
    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * m;
        int end = std::min(start + m, (int)data.size());

        if (start >= data.size())
        {
            std::cerr << "Error: Start index exceeds data size. Exiting loop." << std::endl;
            break;
        }

        std::vector<int> segment(data.begin() + start, data.begin() + end);

        // Debugging: Log segment details
        // std::cout << "Processing segment " << i + 1 << " of size " << segment.size() << std::endl;

        double p_value = SerialTestSegment(16, segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Serial Test"},
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
    std::string resultFilePath = "uploads/serialtest_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save serial test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Serial Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
