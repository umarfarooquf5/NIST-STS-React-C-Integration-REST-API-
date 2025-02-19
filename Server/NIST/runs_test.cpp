#include "runs_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include "erf.h"

using json = nlohmann::json;

// Runs Test Segment Function
double RunsTestSegment(const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    int S = 0;
    double pi, V, erfc_arg, p_value;

    // Count the number of 1s in the segment
    for (int k = 0; k < n; k++)
    {
        if (epsilon[k])
        {
            S++;
        }
    }

    pi = (double)S / (double)n;

    // Check if pi is within acceptable range
    if (fabs(pi - 0.5) > (2.0 / sqrt((double)n)))
    {
        p_value = 0.0;
    }
    else
    {
        V = 1; // Initialize runs count
        for (int k = 1; k < n; k++)
        {
            if (epsilon[k] != epsilon[k - 1])
            {
                V++;
            }
        }

        erfc_arg = fabs(V - 2.0 * n * pi * (1 - pi)) / (2.0 * pi * (1 - pi) * sqrt((double)2 * n));
        p_value = erfc(erfc_arg);
    }

    return p_value;
}

// Main Function to Handle the Runs Test
std::string RunsTest(const std::string &binaryFilePath, int numberOfSegments, int segmentSize)
{
    std::ifstream file(binaryFilePath, std::ios::binary);
    if (!file.is_open())
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
        double p_value = RunsTestSegment(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Runs Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    // Save results to JSON file
    std::string resultFilePath = "uploads/runstest_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile.is_open())
    {
        throw std::runtime_error("Error saving results to file.");
    }
    json output = {{"status", "success"}, {"results", results}};
    resultFile << output.dump(4);
    resultFile.close();
    std::cout << "Runs Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
