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

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port file_path \n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
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

		n = write(sockfd, req_buffer, read_size);
		if (n < 0) {
			 error("ERROR writing to socket");
		}
		bzero(res_buffer, res_size);
		n = read(sockfd, res_buffer, res_size - 1);
		if (n < 0) {
			 error("ERROR reading from socket");
		}
		LOG_DEBUG("get info from server:%s", res_buffer);
    }

    close(sockfd);
    return 0;
}
