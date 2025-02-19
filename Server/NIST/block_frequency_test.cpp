#include "block_frequency_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

// Function to calculate the Block Frequency Test for one segment
double BlockFrequencySegment(int M, const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    if (n == 0)
    {
        std::cerr << "Error: Empty epsilon vector in BlockFrequencySegment." << std::endl;
        return 0.0;
    }

    // Number of blocks
    int N = n / M;
    double sum = 0.0;

    // Compute the sum of squared differences for each block
    for (int i = 0; i < N; i++)
    {
        int blockSum = 0;
        for (int j = 0; j < M; j++)
        {
            blockSum += epsilon[j + i * M];
        }
        double pi = static_cast<double>(blockSum) / M;
        double v = pi - 0.5;
        sum += v * v;
    }

    // Compute Chi-squared statistic and p-value
    double chi_squared = 4.0 * M * sum;
    double p_value = cephes_igamc(N / 2.0, chi_squared / 2.0);

    return p_value;
}

// Main Function to Handle the Block Frequency Test
std::string BlockFrequencyTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
{
    // std::cout << "Starting BlockFrequencyTest with binary file: " << binaryFilePath << std::endl;

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

        // Log segment details
        // std::cout << "Processing segment " << i + 1 << " of size " << segment.size() << std::endl;

        // Call BlockFrequencySegment for calculation
        double p_value = BlockFrequencySegment(128, segment); // Assuming block size M=16

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Block Frequency Test"},
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
    std::string resultFilePath = "uploads/blockfrequency_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save BlockFrequency test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "BlockFrequency Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
