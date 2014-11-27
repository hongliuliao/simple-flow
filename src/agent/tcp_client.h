/*
 * tcp_client.h
 *
 *  Created on: Nov 11, 2014
 *      Author: liao
 */

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class TcpClient {
private:
	int setNonblocking();
	int _sockfd;
public:
	int create_socket(timeval *conn_timeout = NULL, timeval *read_timeout = NULL);

	int connect_socket(std::string ip, int port);

	int connect_noblock_socket(std::string ip, int port);

	int& get_sockfd() {
	    return _sockfd;
	}
};

#endif /* TCP_CLIENT_H_ */
