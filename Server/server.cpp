#include "httplib.h"
#include "nist_handler.h"
#include <iostream>

int main()
{
    httplib::Server svr;

    // Enable Cross-Origin Resource Sharing (CORS)
    svr.set_pre_routing_handler([](const httplib::Request &, httplib::Response &res)
                                {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled; });

    // Route for NIST tests
    svr.Post("/NIST", [](const httplib::Request &req, httplib::Response &res)
             { handleNIST(req, res); });

    // Route to fetch result file
    svr.Get("/result", [](const httplib::Request &req, httplib::Response &res)
            {
        const std::string resultFilePath = "uploads/result.json";
        std::ifstream resultFile(resultFilePath);
        if (resultFile.is_open()) {
            std::string fileContent((std::istreambuf_iterator<char>(resultFile)),
                                    std::istreambuf_iterator<char>());
            res.set_content(fileContent, "application/json");
            resultFile.close();
        } else {
            res.status = 404;
            res.set_content("Result file not found.", "text/plain");
        } });

    // Start the server
    std::cout << "Server running at http://192.168.18.30:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
