/*
 * file_collector.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: liao
 */
#include <cstring>
#include <cerrno>
#include "regex.h"
#include <dirent.h>
#include <sys/stat.h>

#include "simple_log.h"
#include "file_collector.h"


bool FileCollector::is_match_file(std::string input, std::string pattern) {
	regex_t reg;
	int com_ret = regcomp(&reg, pattern.c_str(), REG_EXTENDED);
	if(com_ret != 0) {
		LOG_WARN("pattern compile error which pattern:%s, com_ret:%d", pattern.c_str(), com_ret);
		return false;
	}

	const size_t nmatch = 1;
	regmatch_t pmatch[1];
	int status = regexec(&reg, input.c_str(), nmatch, pmatch, 0);

	bool ret = false;
	if (status == REG_NOMATCH) {
		LOG_DEBUG("not match for pattern:%s, which input:%s", pattern.c_str(), input.c_str());
	} else if (status == 0) {
		LOG_DEBUG("match success which pattern:%s, input:%s", pattern.c_str(), input.c_str());
		ret = true;
	} else {
		char ebuf[80];
		size_t len = regerror(status, &reg, ebuf, 80);
		LOG_WARN("get unkonw status:%d, error_inf:%s, which pattern:%s, input:%s, len:%d", status, ebuf, pattern.c_str(), input.c_str(), len);
	}

	regfree(&reg);
	return ret;
}

int FileCollector::get_newest_file(std::string input_path, std::string &newest_file_path) {
	// get file dir
	int pos = input_path.find_last_of('/');
	if(pos == std::string::npos) {
		LOG_ERROR("can not found dir info which input_path:%s", input_path.c_str());
		return -1;
	}
	std::string dir = input_path.substr(0, pos);
	std::string file_name_pattern = input_path.substr(pos + 1, input_path.size() - pos);
	LOG_DEBUG("parse input_path which dir:%s, file_name_pattern:%s", dir.c_str(), file_name_pattern.c_str());

	DIR	*dir_ptr = NULL;
	if ((dir_ptr = opendir(dir.c_str())) == NULL){
		LOG_ERROR("open dir error which dir:%s, error msg:%s", dir.c_str(), strerror(errno));
		return -1;
	}

	struct stat newest_stat;
	memset(&newest_stat, 0, sizeof(struct stat));
	bool has_file = false;

	// get all file in dir which match pattern
	dirent *item = NULL;
	while((item = readdir(dir_ptr)) != NULL) {
		if(item->d_name[0] == '.'){
			continue;
		}
		if (item->d_type == DT_REG) { // is normal file
			bool match = is_match_file(std::string(item->d_name), file_name_pattern);
			if(!match){
				LOG_DEBUG("not match file:%s", item->d_name);
				continue;
			}

			std::string full_path = dir + "/" + std::string(item->d_name);
			struct stat tmp_stat;
			if(stat(full_path.c_str(), &tmp_stat) != 0){
				LOG_WARN("Oops  stats file failed [path:%s][errno:%s]", full_path.c_str(), strerror(errno));
				continue;
			}

			if(tmp_stat.st_ino != newest_stat.st_ino && tmp_stat.st_mtime > newest_stat.st_mtime) {
				newest_stat = tmp_stat;
				newest_file_path = full_path;
				has_file = true; //此目录类有符合要求的文件
			}
		}
	}
	return has_file ? 0 : -1;
}
