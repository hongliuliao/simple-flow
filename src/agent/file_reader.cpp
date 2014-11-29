/*
 * file_reader.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <fstream>
#include "simple_log.h"
#include "file_reader.h"

FileReader::FileReader(std::string file_path) {
    this->file_path = file_path;
    offset = 0;
}

int FileReader::read(char *buffer, int size, int &read_size) {
    std::fstream fs(file_path.c_str(), std::fstream::in);
    if(!fs) {
        LOG_WARN("open file failed which path:%s", file_path.c_str());
        return -1;
    }

    fs.seekg(offset, fs.beg);
    fs.read(buffer, size);

    read_size = fs.gcount();

    int ret = 0;
    if(read_size == 0) {
        LOG_DEBUG("no more new data which offset:%d", offset);
        ret = 1; // no more new data
    }

    offset += read_size;
    fs.close();
    return ret;
}

void FileReader::set_file_path(std::string file_path) {
    this->file_path = file_path;
}
