/*
 * file_agent.cpp, read file and send to flow_server
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <fstream>

#include "simple_log.h"

#include "tcp_client.h"

class FileReader {
private:
	std::string file_path;
	int offset;
public:
	FileReader(std::string file_path) {
		this->file_path = file_path;
		offset = 0;
	}

	int read(char *buffer, int size, int &read_size) {
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

};

int main(int argc, char *argv[]) {
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port file_path \n", argv[0]);
       exit(0);
    }

    int sockfd;
    int portno = atoi(argv[2]);

    TcpClient tcp_client;
    int ret = tcp_client.connect_socket(std::string(argv[1]), portno, sockfd);
    if(ret != 0) {
    	LOG_ERROR("connect to server error! which ret:%d", ret);
    	return -1;
    }

    int req_size = 4096;
    int res_size = 256;
    char req_buffer[req_size];
    char res_buffer[res_size];
    FileReader file_reader(argv[3]);
    int read_size = 0;

    while(1) {
		bzero(req_buffer,req_size);
		if(file_reader.read(req_buffer,req_size, read_size) != 0) {
			sleep(1);
			continue;
		}

		int n = write(sockfd, req_buffer, read_size);
		if (n < 0) {
			 LOG_ERROR("ERROR writing to socket");
			 continue;
		}
		bzero(res_buffer, res_size);
		n = read(sockfd, res_buffer, res_size - 1);
		if (n < 0) {
			LOG_ERROR("ERROR reading from socket");
			continue;
		}
		LOG_DEBUG("get info from server:%s", res_buffer);
    }

    close(sockfd);
    return 0;
}
