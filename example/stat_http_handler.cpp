/*
 * stat_http_handler.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */

#include "stat_http_handler.h"


Response get_statist_info(Request& request) {
    std::vector<std::string> times;
    Json::Value root;
    Json::Value json_data;

    std::vector<std::string> cms = get_cmds(times);
    LOG_DEBUG("get cmds size:%d", cms.size());
    send_commands(cms, times, json_data);

    Json::ArrayIndex index = 0;
    root["JSChart"]["datasets"][index]["type"] = "line";
    root["JSChart"]["datasets"][index]["data"] = json_data;

    return Response(STATUS_OK, root);
};

Response static_source_handler(Request& request) {
    Json::Value root;
    Response res = Response(STATUS_OK, root);
    std::string uri = request.get_request_uri();
    uri.replace(0, 1, "");

    LOG_DEBUG("GET replaced uri:%s", uri.c_str());

    std::string file_path = "test/";
    file_path += uri;
    std::fstream fs(file_path.c_str());
    std::stringstream ss;
    if(fs) {
        int file_max_size = 500 * 1024;
        char buffer[file_max_size];
        bzero(buffer, file_max_size);
        fs.read(buffer, file_max_size);
        ss << std::string(buffer);
    }
    res.body = ss.str();
    std::string content_type = "text/html";
    res.set_head("Content-Type", content_type);

    return res;
}
