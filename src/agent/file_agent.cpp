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
#include "file_agent.h"

void FileAgent::set_flow_handler(FlowHandler &flow_handler) {
    _flow_handler = &flow_handler;
};

void FileAgent::start(char *input_path) {
    FileCollector fc;
    std::string real_file_path;
    if(fc.get_newest_file(std::string(input_path), real_file_path) != 0) {
        LOG_ERROR("can not get real_file_path for input_path:%s", input_path);
        return ;
    }

    FileReader file_reader(real_file_path);
    int req_size = 4096;
    char req_buffer[req_size];
    int read_size = 0;

    int retry_status = 0;
    while(1) {
        if(!retry_status) {
            bzero(req_buffer,req_size);
            if(file_reader.read(req_buffer, req_size, read_size) != 0) {
                sleep(1);
                if(file_reader.read(req_buffer, req_size, read_size) != 0) {
                    // check if has new file
                    file_reader.check_and_reset();
                    continue;
                }
            }
        }

        int n = _flow_handler->do_handle(req_buffer, read_size);
        if (n < 0) {
             LOG_ERROR("ERROR writing to socket");
             sleep(1);
             retry_status = 1;
             continue;
        }

        retry_status = 0;
    }
}
