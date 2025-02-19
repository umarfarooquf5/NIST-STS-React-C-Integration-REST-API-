#include "randomexcursions_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"
#include "defs.h"
#include <algorithm> // For std::max
#include <cstdlib>   // For calloc, free
#include <stdexcept> // For std::runtime_error

using json = nlohmann::json;

double ComputeRandomExcursions(int n, const std::vector<int> &epsilon);

// Function to Compute Random Excursions Test for One Segment

double ComputeRandomExcursions(int n, const std::vector<int> &epsilon)
{
    int b, i, j, k, J, x;
    int cycleStart, cycleStop, *cycle = NULL, *S_k = NULL;
    int stateX[8] = {-4, -3, -2, -1, 1, 2, 3, 4};
    int counter[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    double p_value, sum, constraint, nu[6][8];
    double pi[5][6] = {
        {0.0000000000, 0.00000000000, 0.00000000000, 0.00000000000, 0.00000000000, 0.0000000000},
        {0.5000000000, 0.25000000000, 0.12500000000, 0.06250000000, 0.03125000000, 0.0312500000},
        {0.7500000000, 0.06250000000, 0.04687500000, 0.03515625000, 0.02636718750, 0.0791015625},
        {0.8333333333, 0.02777777778, 0.02314814815, 0.01929012346, 0.01607510288, 0.0803755143},
        {0.8750000000, 0.01562500000, 0.01367187500, 0.01196289063, 0.01046752930, 0.0732727051}};

    if (((S_k = (int *)calloc(n, sizeof(int))) == NULL) || ((cycle = (int *)calloc(MAX(1200, n / 20), sizeof(int))) == NULL))
    {
        if (S_k != NULL)
            free(S_k);
        if (cycle != NULL)
            free(cycle);
        throw std::runtime_error("Random Excursions Test:\nInsufficient Work Space Allocated.");
    }

    J = 0;
    S_k[0] = 2 * (int)epsilon[0] - 1;
    for (i = 1; i < n; i++)
    {
        S_k[i] = S_k[i - 1] + 2 * epsilon[i] - 1;
        if (S_k[i] == 0)
        {
            J++;
            if (J > MAX(1200, n / 20))
            {
                free(S_k);
                free(cycle);
                throw std::runtime_error("ERROR IN FUNCTION randomExcursions:\nEXCEEDING THE MAX NUMBER OF CYCLES EXPECTED.");
            }
            cycle[J] = i;
        }
    }
    if (S_k[n - 1] != 0)
        J++;
    cycle[J] = n;

    constraint = MAX(0.005 * pow(n, 0.5), 500);
    if (J >= constraint)
    {
        cycleStart = 0;
        cycleStop = cycle[1];
        for (k = 0; k < 6; k++)
            for (i = 0; i < 8; i++)
                nu[k][i] = 0.;

        for (j = 1; j <= J; j++)
        {
            for (i = 0; i < 8; i++)
                counter[i] = 0;

            for (i = cycleStart; i < cycleStop; i++)
            {
                if ((S_k[i] >= 1 && S_k[i] <= 4) || (S_k[i] >= -4 && S_k[i] <= -1))
                {
                    b = (S_k[i] < 0) ? 4 : 3;
                    counter[S_k[i] + b]++;
                }
            }

            cycleStart = cycle[j] + 1;
            if (j < J)
                cycleStop = cycle[j + 1];

            for (i = 0; i < 8; i++)
            {
                if ((counter[i] >= 0) && (counter[i] <= 4))
                    nu[counter[i]][i]++;
                else if (counter[i] >= 5)
                    nu[5][i]++;
            }
        }

        for (i = 0; i < 8; i++)
        {
            x = stateX[i];
            sum = 0.;
            for (k = 0; k < 6; k++)
                sum += pow(nu[k][i] - J * pi[(int)fabs((double)x)][k], 2) / (J * pi[(int)fabs((double)x)][k]);
            p_value = cephes_igamc(2.5, sum / 2.0);
        }
    }

    return p_value;
}

// **Driver Function to Process Random Excursions Test**
std::string RandomExcursionsTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
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
        double p_value = ComputeRandomExcursions(segment.size(), segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Random Excursions Test"},
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
    std::string resultFilePath = "uploads/randomexcursions_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Random Excursions test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Random Excursions Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
