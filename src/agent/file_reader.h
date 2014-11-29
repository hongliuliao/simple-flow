/*
 * file_reader.h
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */

#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <string>

class FileReader {

public:
    FileReader(std::string file_path);

    int read(char *buffer, int size, int &read_size);

    void set_file_path(std::string file_path);
private:
    std::string file_path;
    int offset;

};

#endif /* FILE_READER_H_ */
