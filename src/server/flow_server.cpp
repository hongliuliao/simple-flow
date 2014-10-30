/*
 * flow_server.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
#include "simple_log.h"
#include "flow_server.h"

int FlowServer::listen_on(int port, int backlog) {
	int sockfd; /* listen on sock_fd */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in my_addr; /* my address information */
	memset (&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET; /* host byte order */
	my_addr.sin_port = htons(port); /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, backlog) == -1) {
		perror("listen");
		exit(1);
	}
	return sockfd;
}

int FlowServer::accept_socket(int sockfd) {
	int new_fd;
	struct sockaddr_in their_addr; /* connector's address information */
	socklen_t sin_size = sizeof(struct sockaddr_in);

	if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size)) == -1) {
		perror("accept");
		return -1;
	}

	LOG_DEBUG("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
	return new_fd;
}

int FlowServer::start(int port, FlowHandler *handler, int backlog) {

	int sockfd = this->listen_on(port, backlog);
	LOG_INFO("FlowServer starting listen on:%d", port);

	while (1) { /* main accept() loop */
		int new_fd = this->accept_socket(sockfd);
		if(new_fd == -1) {
			continue;
		}

		int buffer_size = 4096;
		char read_buffer[buffer_size];
		memset(read_buffer, 0, buffer_size);

		int read_size;
		while((read_size = recv(new_fd, read_buffer, buffer_size, 0)) > 0) {

			handler->do_handle(read_buffer, read_size);

			if (send(new_fd, "OK", 2, 0) == -1) {
				perror("send");
			}
		}

		LOG_DEBUG("connect close!");
		close(new_fd);
	}

	return 0;
}

