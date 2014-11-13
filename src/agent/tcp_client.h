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
	int setNonblocking(int fd);

public:
	int connect_socket(std::string ip, int port, int &sockfd);

	int connect_noblock_socket(std::string ip, int port, int &sockfd);
};

#endif /* TCP_CLIENT_H_ */
