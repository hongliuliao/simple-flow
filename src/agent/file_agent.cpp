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
#include "file_collector.h"
#include "file_reader.h"

timeval conn_timeout = {2, 0};
timeval read_timeout = {0, 100};

void reconnect_until_success(TcpClient &tcp_client, std::string host, int portno) {
    tcp_client.close_socket();
	while(1) {
	    int ret = tcp_client.create_socket(&conn_timeout, &read_timeout);
	    if(ret != 0) {
            LOG_ERROR("create_socket to server error! which ret:%d", ret);
        } else {
            ret = tcp_client.connect_socket(host, portno);
            if(ret == 0) {
                break;
            }
        }

		LOG_WARN("connect_socket error which host:%s, portno:%d, ret:%d", host.c_str(), portno, ret);
		sleep(1);
	}

}

int main(int argc, char *argv[]) {
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port file_path \n", argv[0]);
       exit(0);
    }

    std::string host = argv[1];
    int portno = atoi(argv[2]);
    std::string input_path(argv[3]);

    TcpClient tcp_client;
    int ret = tcp_client.create_socket(&conn_timeout, &read_timeout);
    if(ret != 0) {
        LOG_ERROR("create_socket to server error! which ret:%d", ret);
        return -1;
    }
    ret = tcp_client.connect_socket(std::string(argv[1]), portno);
    if(ret != 0) {
    	LOG_ERROR("connect to server error! which ret:%d", ret);
    	return -1;
    }

    FileCollector fc;
    std::string real_file_path;
    if(fc.get_newest_file(input_path, real_file_path) != 0) {
    	LOG_ERROR("can not get real_file_path for input_path:%s", input_path.c_str());
    	return -1;
    }

    FileReader file_reader(real_file_path);
    int req_size = 4096;
    int res_size = 256;
    char req_buffer[req_size];
    char res_buffer[res_size];
    int read_size = 0;

    int retry_status = 0;
    while(1) {
    	if(!retry_status) {
    		bzero(req_buffer,req_size);
			if(file_reader.read(req_buffer, req_size, read_size) != 0) {
				sleep(1);
				if(file_reader.read(req_buffer, req_size, read_size) != 0) {
					// check if has new file
					if(fc.get_newest_file(input_path, real_file_path) == 0) {
						file_reader.set_file_path(real_file_path);
					}
					continue;
				}
			}
    	}

		int n = tcp_client.write_bytes(req_buffer, read_size);
		if (n < 0) {
			 LOG_ERROR("ERROR writing to socket");
			 reconnect_until_success(tcp_client, host, portno);
			 retry_status = 1;
			 continue;
		}

		bzero(res_buffer, res_size);
		n = tcp_client.read_bytes(res_buffer, res_size - 1);
		if (n <= 0) {
			LOG_ERROR("ERROR reading from socket");
			reconnect_until_success(tcp_client, host, portno);
			retry_status = 1;
			continue;
		}

		retry_status = 0;
		LOG_DEBUG("get info from server:%s", res_buffer);
    }

    tcp_client.close_socket();
    return 0;
}
