/*
 * statistic_server.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <vector>
#include <fstream>
#include <sstream>
#include "json/json.h"
#include "hiredis/hiredis.h"
#include "simple_log.h"
#include "http_server.h"

std::vector<std::string> get_cmds(std::vector<std::string> &times) {
	std::vector<std::string> result;
	struct tm *time_info;
	time_t start;
	time(&start);

	// get recent 5s data
	for (unsigned i = 5; i > 0; i--) {
		time_t stat_time = start - i;
		time_info = localtime(&stat_time);
		char day_str[80];
		bzero(day_str, 80);
		strftime(day_str, 80, "%Y-%m-%d_%H:%M:%S", time_info);
		std::string cmd("INCRBY ");
		cmd += day_str;
		cmd += " 0";
		LOG_DEBUG("build redis cmd:%s", cmd.c_str());
		result.push_back(cmd);
		times.push_back(std::string(day_str));
	}
	return result;
}

int send_commands(std::vector<std::string> cmds, std::vector<std::string> times, Json::Value &json_data) {
	redisContext *rc = redisConnect("localhost", 6379);

	for(size_t i=0;i<cmds.size();i++) {
		redisAppendCommand(rc, cmds[i].c_str());
	}

	for(size_t i=0;i<cmds.size();i++) {
		redisReply *reply;
		redisGetReply(rc, (void **)&reply);

		if(reply->type == REDIS_REPLY_ERROR) {
			LOG_WARN("redis return error which msg:%s", reply->str);
			continue;
		}
		Json::ArrayIndex index = (Json::ArrayIndex) i;
		json_data[index]["unit"] = times[i];
		json_data[index]["value"] = reply->integer;
		LOG_DEBUG("start_time:%s, pv:%d", times[i].c_str(), reply->integer);

		if(reply != NULL) {
			freeReplyObject(reply);
		}
	}

	redisFree(rc);
	return 0;
}

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

	root["JSChart"]["optionset"][index]["set"] = "setLineColor";
	root["JSChart"]["optionset"][index]["value"] = "'#8D9386'";

	root["JSChart"]["optionset"][index+1]["set"] = "setLineWidth";
	root["JSChart"]["optionset"][index+1]["value"] = "2";

	root["JSChart"]["optionset"][index+2]["set"] = "setTitleColor";
	root["JSChart"]["optionset"][index+2]["value"] = "'#7D7D7D'";

	root["JSChart"]["optionset"][index+3]["set"] = "setAxisColor";
	root["JSChart"]["optionset"][index+3]["value"] = "'#9F0505'";

	root["JSChart"]["optionset"][index+4]["set"] = "setGridColor";
	root["JSChart"]["optionset"][index+4]["value"] = "'#a4a4a4'";

	root["JSChart"]["optionset"][index+5]["set"] = "setAxisValuesColor";
	root["JSChart"]["optionset"][index+5]["value"] = "'#333639'";

	root["JSChart"]["optionset"][index + 6]["set"] = "setAxisNameColor";
	root["JSChart"]["optionset"][index + 6]["value"] = "'#333639'";

	root["JSChart"]["optionset"][index + 7]["set"] = "setTextPaddingLeft";
	root["JSChart"]["optionset"][index + 7]["value"] = "0";
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
	if(uri.find(".js") != std::string::npos) {
		res.set_head("Content-Type", "application/javascript");
	} else {
		res.set_head("Content-Type", "text/html");
	}

	return res;
}

int main() {
	HttpServer http_server;
	http_server.add_mapping("/get_data", get_statist_info);
	http_server.add_mapping("/show.html", static_source_handler);
	http_server.add_mapping("/jscharts.js", static_source_handler);
	http_server.start(3493);
}
