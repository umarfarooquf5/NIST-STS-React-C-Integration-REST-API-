#include "frequency_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include "erf.h"

using json = nlohmann::json;

// Frequency Test
double FrequencyTestSegment(const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    double sum = 0.0;
    double sqrt2 = 1.41421356237309504880;

    for (int i = 0; i < n; i++)
    {
        sum += 2 * epsilon[i] - 1;
    }

    double s_obs = fabs(sum) / sqrt((double)n);
    double f = s_obs / sqrt2;
    return erfc(f);
}

// Main Function to Handle the Frequency Test
std::string FrequencyTest(const std::string &binaryFilePath, int numberOfSegments, int segmentSize)
{
    std::ifstream file(binaryFilePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open binary file.");
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
        int start = i * segmentSize;
        int end = std::min(start + segmentSize, (int)data.size());

        if (start >= data.size())
            break;

        std::vector<int> segment(data.begin() + start, data.begin() + end);
        double p_value = FrequencyTestSegment(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Frequency Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    // Save results
    std::string resultFilePath = "uploads/frequencytest_result.json";
    std::ofstream resultFile(resultFilePath);
    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();
    std::cout << "Frequency Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
