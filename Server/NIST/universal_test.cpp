#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
// #include "cephes.h"
#include "erf.h"

using json = nlohmann::json;

// Function to calculate the Universal Test for a single segment
double UniversalTestSegment(const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    int i, j, p, L, Q, K;
    double arg, sqrt2, sigma, phi, sum, c;
    long *T, decRep;
    double expected_value[17] = {0, 0, 0, 0, 0, 0, 5.2177052, 6.1962507, 7.1836656,
                                 8.1764248, 9.1723243, 10.170032, 11.168765,
                                 12.168070, 13.167693, 14.167488, 15.167379};
    double variance[17] = {0, 0, 0, 0, 0, 0, 2.954, 3.125, 3.238, 3.311, 3.356, 3.384,
                           3.401, 3.410, 3.416, 3.419, 3.421};

    // Determine the block length L based on the input size n
    L = 5;
    if (n >= 387840)
        L = 6;
    if (n >= 904960)
        L = 7;
    if (n >= 2068480)
        L = 8;
    if (n >= 4654080)
        L = 9;
    if (n >= 10342400)
        L = 10;
    if (n >= 22753280)
        L = 11;
    if (n >= 49643520)
        L = 12;
    if (n >= 107560960)
        L = 13;
    if (n >= 231669760)
        L = 14;
    if (n >= 496435200)
        L = 15;
    if (n >= 1059061760)
        L = 16;

    Q = 10 * (int)pow(2.0, L);
    K = (int)(floor((double)n / L) - (double)Q); // Blocks to test
    p = (int)pow(2.0, L);

    // Error handling for invalid L or insufficient memory
    if ((L < 6) || (L > 16) || ((double)Q < 10 * pow(2.0, L)) || ((T = (long *)calloc(p, sizeof(long))) == NULL))
    {
        return 0.0; // Return 0 if parameters are invalid
    }

    // Compute expected value and sigma
    c = 0.7 - 0.8 / (double)L + (4 + 32 / (double)L) * pow(K, -3.0 / (double)L) / 15;
    sigma = c * sqrt(variance[L] / (double)K);
    sqrt2 = sqrt(2.0);
    sum = 0.0;

    // Initialize T array
    for (i = 0; i < p; i++)
    {
        T[i] = 0;
    }

    // Initialize table
    for (i = 1; i <= Q; i++)
    {
        decRep = 0;
        for (j = 0; j < L; j++)
        {
            decRep += epsilon[(i - 1) * L + j] * (long)pow(2.0, L - 1 - j);
        }
        T[decRep] = i;
    }

    // Process blocks
    for (i = Q + 1; i <= Q + K; i++)
    {
        decRep = 0;
        for (j = 0; j < L; j++)
        {
            decRep += epsilon[(i - 1) * L + j] * (long)pow(2.0, L - 1 - j);
        }
        sum += log((double)i - T[decRep]) / log(2.0);
        T[decRep] = i;
    }

    phi = (double)(sum / (double)K);
    arg = fabs(phi - expected_value[L]) / (sqrt2 * sigma);
    double p_value = erfc(arg);

    // Free allocated memory
    free(T);

    return p_value;
}

// Main Function to Handle the Universal Test
std::string UniversalTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
{
    // std::cout << "Starting Universal Test with binary file: " << binaryFilePath << std::endl;

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

    if (data.size() < segmentSize * numberOfSegments)
    {
        throw std::runtime_error("Error: Insufficient data for the number of segments and block size requested.");
    }

    // Prepare results
    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * segmentSize;
        int end = std::min(start + segmentSize, (int)data.size());

        if (start >= data.size())
        {
            std::cerr << "Error: Start index exceeds data size. Exiting loop." << std::endl;
            break;
        }

        std::vector<int> segment(data.begin() + start, data.begin() + end);

        // Call UniversalTestSegment
        double p_value = UniversalTestSegment(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Universal Test"},
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
    std::string resultFilePath = "uploads/universal_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Universal test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Universal Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
