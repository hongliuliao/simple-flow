/*
 * tcp_client.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: liao
 */

#include "tcp_client.h"
#include <sys/fcntl.h>

int TcpClient::setNonblocking(int fd) {
	int flags;

	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int TcpClient::connect_socket(std::string ip, int port, int &sockfd) {
	int n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		return -1;
	}
	server = gethostbyname(ip.c_str());
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		return -1;
	}
	return 0;
}

int TcpClient::connect_noblock_socket(std::string ip, int port, int &sockfd) {
	int ret = this->connect_socket(ip, port, sockfd);
	if(ret != 0) {
		return ret;
	}
	this->setNonblocking(sockfd);
	return 0;
}

