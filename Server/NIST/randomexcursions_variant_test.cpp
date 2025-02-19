#include "randomexcursions_variant_test.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "cephes.h"
#include "defs.h"

using json = nlohmann::json;

double ComputeRandomExcursionsVariant(const std::vector<int> &epsilon, int n);

double ComputeRandomExcursionsVariant(const std::vector<int> &epsilon, int n)
{
    int i, p, J, x, constraint, count, *S_k;
    int stateX[18] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    double p_value;

    if ((S_k = (int *)calloc(n, sizeof(int))) == NULL)
    {
        throw std::runtime_error("Random Excursions Variant: Insufficient memory allocated.");
    }

    J = 0;
    S_k[0] = 2 * (int)epsilon[0] - 1;
    for (i = 1; i < n; i++)
    {
        S_k[i] = S_k[i - 1] + 2 * epsilon[i] - 1;
        if (S_k[i] == 0)
            J++;
    }
    if (S_k[n - 1] != 0)
        J++;

    constraint = (int)MAX(0.005 * pow(n, 0.5), 500);
    if (J >= constraint)
    {
        for (p = 0; p <= 17; p++)
        {
            x = stateX[p];
            count = 0;
            for (i = 0; i < n; i++)
            {
                if (S_k[i] == x)
                    count++;
            }
            p_value = erfc(fabs((double)count - J) / (sqrt(2.0 * J * (4.0 * fabs((double)x) - 2))));
        }
    }
    free(S_k);
    return p_value;
}

std::string RandomExcursionsVariantTest(const std::string &binaryFilePath, int m, int numberOfSegments)
{
    std::ifstream file(binaryFilePath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open binary file: " + binaryFilePath);

    std::vector<int> data;
    char byte;
    while (file.read(&byte, 1))
    {
        for (int i = 7; i >= 0; --i)
            data.push_back((byte >> i) & 1);
    }
    file.close();

    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * m;
        int end = std::min(start + m, static_cast<int>(data.size()));

        if (start >= static_cast<int>(data.size()))
        {
            std::cerr << "Error: Start index exceeds data size. Exiting loop." << std::endl;
            break;
        }

        std::vector<int> segment(data.begin() + start, data.begin() + end);
        double p_value = ComputeRandomExcursionsVariant(segment, static_cast<int>(segment.size()));

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Random Excursions Variant"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    if (!std::filesystem::exists("uploads"))
        std::filesystem::create_directory("uploads");

    std::string resultFilePath = "uploads/randomexcursions_variant_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
        throw std::runtime_error("Failed to save test results to: " + resultFilePath);

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Random Excursions Variant Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
