#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <sstream>
#include <thread>
#include <vector>
#include <mutex>
// Include NIST .h file
#include "nist_handler.h"
// Include Tests .h files
#include "frequency_test.h"
#include "block_frequency_test.h"
#include "runs_test.h"
#include "serial_test.h"
#include "universal_test.h"
#include "longestRunOfOnes_test.h"
#include "randomexcursions_test.h"
#include "randomexcursions_variant_test.h"
#include "overlappingtemplates_test.h"
#include "nonoverlappingtemplates_test.h"
#include "approximateentropy_test.h"
#include "cumulativesums_test.h"
#include "rank_test.h"
#include "linearcomplexity_test.h"
#include "spectralDFT_test.h"

using json = nlohmann::json;
std::mutex result_mutex;

void runTest(const std::string &task, const std::string &binaryFilePath, int numberOfSegments, int segmentSize, json &results)
{
    std::string testResult;

    if (task == "Frequency")
    {
        testResult = FrequencyTest(binaryFilePath, numberOfSegments, segmentSize);
    }
    else if (task == "Block Frequency (Block Length: 128 bits)")
    {
        testResult = BlockFrequencyTest(binaryFilePath.c_str(), segmentSize, numberOfSegments);
    }
    else if (task == "Runs")
    {
        testResult = RunsTest(binaryFilePath, numberOfSegments, segmentSize);
    }
    else if (task == "Serial (Block Length: 16 bits)")
    {
        testResult = SerialTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Universal (Block Length: 1280 bits)")
    {
        testResult = UniversalTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Longest Runs of Ones")
    {
        testResult = LongestRunOfOnesTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Random Excursions")
    {
        testResult = RandomExcursionsTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Random Excursions Variant")
    {
        testResult = RandomExcursionsVariantTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Overlapping Templates (Template Length: 9 bits)")
    {
        testResult = OverlappingTemplatesTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Non-overlapping Templates (Template Length: 9 bits)")
    {
        testResult = NonOverlappingTemplateTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Approximate Entropy (Block Length: 10 bits)")
    {
        testResult = ApproximateEntropyTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Cumulative Sums")
    {
        testResult = CumulativeSumsTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Rank")
    {
        testResult = RankTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Linear Complexity (Block Length: 500 bits)")
    {
        testResult = LinearComplexityTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else if (task == "Spectral DFT")
    {
        testResult = SpectralDFTTest(binaryFilePath, segmentSize, numberOfSegments);
    }
    else
    {
        std::lock_guard<std::mutex> lock(result_mutex);
        results.push_back({{"test_name", task}, {"status", "unsupported"}, {"details", "Test not implemented yet"}});
        return;
    }

    std::ifstream resultFile(testResult);
    if (resultFile.is_open())
    {
        json testResults;
        resultFile >> testResults;
        resultFile.close();

        std::lock_guard<std::mutex> lock(result_mutex);
        for (const auto &result : testResults["results"])
        {
            results.push_back(result);
        }
    }
}

void handleNIST(const httplib::Request &req, httplib::Response &res)
{
    try
    {
        std::cout << "Handling /NIST POST request..." << std::endl;

        if (!std::filesystem::exists("uploads"))
        {
            std::filesystem::create_directory("uploads");
        }

        if (!req.has_file("binary_file") || !req.has_file("additional_file"))
        {
            res.status = 400;
            res.set_content("Error: Missing binary_file or additional_file in the request.", "text/plain");
            return;
        }

        const auto &binaryFile = req.get_file_value("binary_file");
        const auto &additionalFile = req.get_file_value("additional_file");

        const std::string binaryFilePath = "uploads/binary_file.dat";
        std::ofstream binaryOut(binaryFilePath, std::ios::binary);
        if (!binaryOut)
            throw std::runtime_error("Failed to create binary_file.dat");
        binaryOut.write(binaryFile.content.c_str(), binaryFile.content.size());
        binaryOut.close();

        const std::string additionalFilePath = "uploads/additional_file.json";
        std::ofstream additionalOut(additionalFilePath);
        if (!additionalOut)
            throw std::runtime_error("Failed to create additional_file.json");
        additionalOut << additionalFile.content;
        additionalOut.close();

        json additionalData = json::parse(additionalFile.content);
        int numberOfSegments = additionalData["numberOfSegments"].is_string()
                                   ? std::stoi(additionalData["numberOfSegments"].get<std::string>())
                                   : additionalData["numberOfSegments"].get<int>();

        int segmentSize = additionalData["segmentSize"].is_string()
                              ? std::stoi(additionalData["segmentSize"].get<std::string>())
                              : additionalData["segmentSize"].get<int>();

        auto selectedTasks = additionalData["selectedTasks"];

        json results = json::array();
        std::vector<std::thread> threads;

        for (const auto &task : selectedTasks)
        {
            threads.emplace_back(runTest, task, binaryFilePath, numberOfSegments, segmentSize, std::ref(results));
        }

        for (auto &t : threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        const std::string resultFilePath = "uploads/result.json";
        std::ofstream resultFile(resultFilePath);
        resultFile << json{
            {"status", "success"},
            {"filename", binaryFile.filename},
            {"number_of_segments", numberOfSegments},
            {"segment_size", segmentSize},
            {"results", results}};
        resultFile.close();

        std::cout << "All the Tests results Saved..." << std::endl;
        res.set_content(json{
                            {"status", "success"},
                            {"filename", binaryFile.filename},
                            {"number_of_segments", numberOfSegments},
                            {"segment_size", segmentSize},
                            {"result_file", resultFilePath}}
                            .dump(4),
                        "application/json");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("Error: " + std::string(e.what()), "text/plain");
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred." << std::endl;
        res.status = 500;
        res.set_content("Unknown error occurred.", "text/plain");
    }
}
