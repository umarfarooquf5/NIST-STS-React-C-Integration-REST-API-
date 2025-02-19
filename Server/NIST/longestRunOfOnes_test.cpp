#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"

using json = nlohmann::json;

// Function to calculate the Longest Run of Ones for a single segment
double LongestRunOfOnesSegment(const std::vector<int> &epsilon)
{
    int n = epsilon.size();
    double p_value, chi2, pi[7];
    int run, v_n_obs, N, i, j, K, M, V[7];
    unsigned int nu[7] = {0, 0, 0, 0, 0, 0, 0};

    // Set parameters based on the size of n
    if (n < 128)
    {
        std::cerr << "Error: n is too short for Longest Run of Ones Test." << std::endl;
        return 0.0; // Test not valid for very small sequences
    }
    if (n < 6272)
    {
        K = 3;
        M = 8;
        V[0] = 1;
        V[1] = 2;
        V[2] = 3;
        V[3] = 4;
        pi[0] = 0.21484375;
        pi[1] = 0.3671875;
        pi[2] = 0.23046875;
        pi[3] = 0.1875;
    }
    else if (n < 750000)
    {
        K = 5;
        M = 128;
        V[0] = 4;
        V[1] = 5;
        V[2] = 6;
        V[3] = 7;
        V[4] = 8;
        V[5] = 9;
        pi[0] = 0.1174035788;
        pi[1] = 0.242955959;
        pi[2] = 0.249363483;
        pi[3] = 0.17517706;
        pi[4] = 0.102701071;
        pi[5] = 0.112398847;
    }
    else
    {
        K = 6;
        M = 10000;
        V[0] = 10;
        V[1] = 11;
        V[2] = 12;
        V[3] = 13;
        V[4] = 14;
        V[5] = 15;
        V[6] = 16;
        pi[0] = 0.0882;
        pi[1] = 0.2092;
        pi[2] = 0.2483;
        pi[3] = 0.1933;
        pi[4] = 0.1208;
        pi[5] = 0.0675;
        pi[6] = 0.0727;
    }

    N = n / M; // Number of blocks
    for (i = 0; i < N; i++)
    {
        v_n_obs = 0;
        run = 0;
        for (j = 0; j < M; j++)
        {
            if (epsilon[i * M + j] == 1)
            {
                run++;
                if (run > v_n_obs)
                    v_n_obs = run;
            }
            else
                run = 0;
        }
        if (v_n_obs < V[0])
            nu[0]++;
        for (j = 0; j <= K; j++)
        {
            if (v_n_obs == V[j])
                nu[j]++;
        }
        if (v_n_obs > V[K])
            nu[K]++;
    }

    // Compute Chi-squared statistic
    chi2 = 0.0;
    for (i = 0; i <= K; i++)
        chi2 += ((nu[i] - N * pi[i]) * (nu[i] - N * pi[i])) / (N * pi[i]);

    // Compute p-value
    p_value = cephes_igamc((double)(K / 2.0), chi2 / 2.0);

    return p_value;
}

// Driver function to handle Longest Run of Ones Test
std::string LongestRunOfOnesTest(const std::string &binaryFilePath, int segmentSize, int numberOfSegments)
{
    // std::cout << "Starting Longest Run of Ones Test with binary file: " << binaryFilePath << std::endl;

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

        // Call LongestRunOfOnesSegment
        double p_value = LongestRunOfOnesSegment(segment);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Longest Run of Ones Test"},
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
    std::string resultFilePath = "uploads/longestrunofones_test_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save Longest Run of Ones test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Longest Run of Ones Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
