/*
 * file_collector_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: liao
 */

#include "simple_log.h"
#include "file_collector.h"


int main() {
	FileCollector fc;
	std::string find_path;
	int ret = fc.get_newest_file("/tmp/curl_[a-z]+.log", find_path);

	if(ret != 0) {
		LOG_INFO("find newest_file error!");
		return -1;
	}
	LOG_INFO("find newest file success which file_path:%s", find_path.c_str());
	return 0;
}
