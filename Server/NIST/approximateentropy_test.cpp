#include "approximateentropy_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

// Function to Compute Approximate Entropy for One Segment
double ComputeApproximateEntropy(int m, const std::vector<int> &epsilon)
{
    int i, j, k, r, blockSize, seqLength, powLen, index;
    double sum, numOfBlocks, ApEn[2], apen, chi_squared, p_value;
    unsigned int *P;

    seqLength = epsilon.size();
    r = 0;

    for (blockSize = m; blockSize <= m + 1; blockSize++)
    {
        if (blockSize == 0)
        {
            ApEn[0] = 0.00;
            r++;
        }
        else
        {
            numOfBlocks = (double)seqLength;
            powLen = (int)pow(2.0, blockSize + 1) - 1;

            if ((P = (unsigned int *)calloc(powLen, sizeof(unsigned int))) == NULL)
            {
                throw std::runtime_error("Insufficient memory available in Approximate Entropy test.");
            }

            for (i = 1; i < powLen - 1; i++)
            {
                P[i] = 0;
            }

            for (i = 0; i < numOfBlocks; i++) // Compute Frequency
            {
                k = 1;
                for (j = 0; j < blockSize; j++)
                {
                    k <<= 1;
                    if ((int)epsilon[(i + j) % seqLength] == 1)
                        k++;
                }
                P[k - 1]++;
            }

            // Compute Sum
            sum = 0.0;
            index = (int)pow(2.0, blockSize) - 1;
            for (i = 0; i < (int)pow(2.0, blockSize); i++)
            {
                if (P[index] > 0)
                    sum += P[index] * log(P[index] / numOfBlocks);
                index++;
            }
            sum /= numOfBlocks;
            ApEn[r] = sum;
            r++;
            free(P);
        }
    }

    apen = ApEn[0] - ApEn[1];
    chi_squared = 2.0 * seqLength * (log(2.0) - apen);
    p_value = cephes_igamc(pow(2.0, m - 1), chi_squared / 2.0);

    return p_value;
}

// Driver Function to Process Approximate Entropy Test
std::string ApproximateEntropyTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
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

    if (data.size() < segmentSize * numberOfSegments)
    {
        throw std::runtime_error("Error: Insufficient data for the number of segments and block size requested.");
    }

    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * segmentSize;
        int end = std::min(start + segmentSize, (int)data.size());

        if (start >= data.size())
        {
            break;
        }

        std::vector<int> segment(data.begin() + start, data.begin() + end);
        double p_value = ComputeApproximateEntropy(10, segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Approximate Entropy Test"},
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
    std::string resultFilePath = "uploads/approximate_entropy_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Approximate Entropy test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Approximate Entropy Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
