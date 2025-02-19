#ifndef NIST_HANDLER_H
#define NIST_HANDLER_H

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

// Function declaration to handle NIST requests
void handleNIST(const httplib::Request &req, httplib::Response &res);

#endif
