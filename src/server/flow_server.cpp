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
#include "epoll_socket.h"
#include "flow_server.h"

class FlowEpollWatcher : public EpollSocketWatcher {
public:
	FlowHandler *handler;

	virtual int on_accept(EpollContext &epoll_context) {
		return 0;
	};

	virtual int on_readable(EpollContext &epoll_context, char *read_buffer, int buffer_size, int read_size) {
		handler->do_handle(read_buffer, read_size);
		return 0;
	};

	virtual int on_writeable(EpollContext &epoll_context) {
		if (send(epoll_context.fd, "OK", 2, 0) == -1) {
			perror("send");
		}
		return 0;
	};

	virtual int on_close(EpollContext &epoll_context) {
		return 0;
	};
};

int FlowServer::start(int port, FlowHandler *handler, int backlog) {

	FlowEpollWatcher epoll_watcher;
	epoll_watcher.handler = handler;

	EpollSocket e_socket;
	e_socket.start_epoll(port, epoll_watcher, backlog);

	return 0;
}



