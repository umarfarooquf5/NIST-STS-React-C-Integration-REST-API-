#include "nonoverlappingtemplates_test.h"
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

double NonOverlappingTemplateCalculations(int m, const std::vector<int> &epsilon, int segmentNo);

double NonOverlappingTemplateCalculations(int m, const std::vector<int> &epsilon, int segmentNo)
{
    int numOfTemplates[100] = {0, 0, 2, 4, 6, 12, 20, 40, 74, 148, 284, 568, 1116,
                               2232, 4424, 8848, 17622, 35244, 70340, 140680, 281076, 562152};
    unsigned int W_obs, nu[6], *Wj = NULL;
    double sum, chi2, p_value, lambda, pi[6], varWj;
    int i, j, k, match, SKIP, M, N, K = 5;
    std::vector<int> sequence(m);

    N = 8;
    M = epsilon.size() / N;

    if ((Wj = (unsigned int *)calloc(N, sizeof(unsigned int))) == NULL)
        return 0.0;

    lambda = (M - m + 1) / pow(2.0, m);
    varWj = M * (1.0 / pow(2.0, m) - (2.0 * m - 1.0) / pow(2.0, 2.0 * m));

    if (isNegative(lambda) || isZero(lambda))
        return 0.0;

    SKIP = (numOfTemplates[m] < MAXNUMOFTEMPLATES) ? 1 : (int)(numOfTemplates[m] / MAXNUMOFTEMPLATES);
    numOfTemplates[m] = (int)numOfTemplates[m] / SKIP;

    sum = 0.0;
    for (i = 0; i < 2; i++)
    {
        pi[i] = exp(-lambda + i * log(lambda) - cephes_lgam(i + 1));
        sum += pi[i];
    }
    pi[0] = sum;
    for (i = 2; i <= K; i++)
    {
        pi[i - 1] = exp(-lambda + i * log(lambda) - cephes_lgam(i + 1));
        sum += pi[i - 1];
    }
    pi[K] = 1 - sum;

    for (int jj = 0; jj < MIN(MAXNUMOFTEMPLATES, numOfTemplates[m]); jj++)
    {
        for (k = 0; k < m; k++)
            sequence[k] = (jj + k) % 2; // Example pattern generation

        for (k = 0; k <= K; k++)
            nu[k] = 0;

        for (i = 0; i < N; i++)
        {
            W_obs = 0;
            for (j = 0; j < M - m + 1; j++)
            {
                match = 1;
                for (k = 0; k < m; k++)
                {
                    if ((int)sequence[k] != (int)epsilon[i * M + j + k])
                    {
                        match = 0;
                        break;
                    }
                }
                if (match == 1)
                {
                    W_obs++;
                    j += m - 1;
                }
            }
            Wj[i] = W_obs;
        }

        chi2 = 0.0;
        for (i = 0; i < N; i++)
        {
            chi2 += pow(((double)Wj[i] - lambda) / pow(varWj, 0.5), 2);
        }
        p_value = cephes_igamc(N / 2.0, chi2 / 2.0);
    }

    free(Wj);
    return p_value;
}

std::string NonOverlappingTemplateTest(const std::string &binaryFilePath, int m, int numberOfSegments)
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

    if (data.size() < m * numberOfSegments)
    {
        throw std::runtime_error("Error: Insufficient data for the number of segments and block size requested.");
    }

    json results = json::array();
    time_t now = time(nullptr);

    for (int i = 0; i < numberOfSegments; i++)
    {
        int start = i * m;
        int end = std::min(start + m, (int)data.size());

        if (start >= data.size())
            break;

        std::vector<int> segment(data.begin() + start, data.begin() + end);

        double p_value = NonOverlappingTemplateCalculations(9, segment, i + 1);

        results.push_back({{"segment_no", i + 1},
                           {"test_name", "Non-overlapping Templates Test"},
                           {"timestamp", std::ctime(&now)},
                           {"p_value", p_value},
                           {"result", (p_value > 0.01 && p_value < 1.0) ? "Pass" : "Fail"}});
    }

    if (!std::filesystem::exists("uploads"))
    {
        std::filesystem::create_directory("uploads");
    }

    std::string resultFilePath = "uploads/nonoverlapping_template_result.json";
    std::ofstream resultFile(resultFilePath);
    if (!resultFile)
    {
        throw std::runtime_error("Failed to save non-overlapping template test results to: " + resultFilePath);
    }

    resultFile << json{{"status", "success"}, {"results", results}};
    resultFile.close();

    std::cout << "Non Overlapping Template Test results saved to: " << resultFilePath << std::endl;
    return resultFilePath;
}
