/*
 * log_index.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <iostream>
#include <map>
#include <time.h>
#include "file_agent.h"
#include "curl/curl.h"
#include "curl_utils.h"
#include "flow_handler.h"

class LogIndexHandler : public LineFlowHandler {

public:
    int handle_lines(std::vector<std::string> lines) {

        for (unsigned i = 0; i < lines.size(); i++) {
            std::string &line = lines[i];

            Json::Value params;
            params["log_content"] = line;
            time_t now = time(NULL);
            params["create_time"] = (Json::Int64) now;

            std::string result;
            std::string url = std::string("http://localhost:9200/mytest/accesslog/");
            CURLcode code = CurlUtils::post_json(url, params, result);
            std::cout << "get code:" << code << ",result:" << result << std::endl;
        }
        return 0;
    }
};

int main() {
    LogIndexHandler h;
    FileAgent fa;
    fa.set_flow_handler(h);
    fa.start("/home/liao/programs/nginx/logs/access.log");
    return 0;
}
