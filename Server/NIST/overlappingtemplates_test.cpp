#include "overlappingtemplates_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

double Pr(int u, double eta);

// Function to Compute Overlapping Templates Test for One Segment
double ComputeOverlappingTemplates(int m, const std::vector<int> &epsilon, int n)
{
    int i, k, match;
    double W_obs, eta, sum, chi2, p_value, lambda;
    int M, N, j, K = 5;
    unsigned int nu[6] = {0, 0, 0, 0, 0, 0};
    double pi[6] = {0.364091, 0.185659, 0.139381, 0.100571, 0.0704323, 0.139865};
    std::vector<int> sequence(m, 1);

    M = 1032;
    N = n / M;

    lambda = (double)(M - m + 1) / pow(2.0, m);
    eta = lambda / 2.0;
    sum = 0.0;

    for (i = 0; i < K; i++) // Compute Probabilities
    {
        pi[i] = Pr(i, eta);
        sum += pi[i];
    }
    pi[K] = 1 - sum;

    for (i = 0; i < N; i++)
    {
        W_obs = 0;
        for (j = 0; j < M - m + 1; j++)
        {
            match = 1;
            for (k = 0; k < m; k++)
            {
                if (sequence[k] != epsilon[i * M + j + k])
                {
                    match = 0;
                }
            }
            if (match == 1)
            {
                W_obs++;
            }
        }

        if (W_obs <= 4)
        {
            nu[(int)W_obs]++;
        }
        else
        {
            nu[K]++;
        }
    }

    sum = 0;
    chi2 = 0.0; // Compute Chi Square
    for (i = 0; i < K + 1; i++)
    {
        chi2 += pow((double)nu[i] - (double)N * pi[i], 2) / ((double)N * pi[i]);
        sum += nu[i];
    }
    p_value = cephes_igamc(K / 2.0, chi2 / 2.0);

    return p_value;
}

// **Driver Function to Process Overlapping Templates Test**
std::string OverlappingTemplatesTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
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
        double p_value = ComputeOverlappingTemplates(9, segment, segment.size());

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Overlapping Templates Test"},
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
    std::string resultFilePath = "uploads/overlapping_templates_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Overlapping Templates test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Overlapping Templates Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}

double Pr(int u, double eta)
{
    int l;
    double sum, p;

    if (u == 0)
    {
        p = exp(-eta);
    }
    else
    {
        sum = 0.0;
        for (l = 1; l <= u; l++)
        {
            sum += exp(-eta - u * log(2.0) + l * log(eta) - cephes_lgam(l + 1) + cephes_lgam(u) - cephes_lgam(l) - cephes_lgam(u - l + 1));
        }
        p = sum;
    }
    return p;
}
