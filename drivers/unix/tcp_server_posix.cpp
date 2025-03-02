/*************************************************************************/
/*  tcp_server_posix.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "tcp_server_posix.h"
#include "stream_peer_tcp_posix.h"

#if defined(POSIX_IP_ENABLED) || defined(UNIX_ENABLED) || defined(PSP) || defined(__3DS__)

#if defined(WII_ENABLED)
 #include "../../platform/wii/network2.h"
 using namespace gc::net;
#elif defined(PSP)
 #include <sys/select.h>

#else
 #include <poll.h>
#include <arpa/inet.h>
#endif

#ifndef NET_NS
#if defined(WII_ENABLED)
#define NET_NS gc::net
#else
#define NET_NS
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#if !defined(WII_ENABLED)
 #include <netdb.h>
 #include <netinet/in.h>
 #include <sys/socket.h>

 #include <sys/types.h>
 #ifndef NO_FCNTL
  #include <sys/fcntl.h>
 #else
  #include <sys/ioctl.h>
 #endif
#endif
#ifdef JAVASCRIPT_ENABLED
#include <arpa/inet.h>
#endif
#include <assert.h>
TCP_Server* TCPServerPosix::_create() {

	return memnew(TCPServerPosix);
};

void TCPServerPosix::make_default() {

	TCP_Server::_create = TCPServerPosix::_create;
};

Error TCPServerPosix::listen(uint16_t p_port,const List<String> *p_accepted_hosts) {

	printf("********* listening on port %i\n", p_port);
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	ERR_FAIL_COND_V(sockfd == -1, FAILED);
#ifndef NO_FCNTL
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
#else
	int bval = 1;
	ioctl(sockfd, FIONBIO, &bval);
#endif

	int reuse=1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {

		printf("REUSEADDR failed!");
	}

	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;         // host byte order
	my_addr.sin_port = htons(p_port);     // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP TODO: use p_accepted_hosts
	memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) != -1) {

		if (::listen(sockfd, 1) == -1) {

			NET_NS::close(sockfd);
			ERR_FAIL_V(FAILED);
		};
	}
	else {
		return ERR_ALREADY_IN_USE;
	};

	if (listen_sockfd != -1) {

		printf("FAILED\n");
		stop();
	};

	listen_sockfd = sockfd;

	printf("OK! %i\n", listen_sockfd);
	return OK;
};

bool TCPServerPosix::is_connection_available() const {

	if (listen_sockfd == -1) {
		return false;
	};

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(listen_sockfd, &readfds);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int maxfd = listen_sockfd + 1;

	int ret = NET_NS::select(maxfd, &readfds, NULL, NULL, &timeout);
	if (ret == -1) {
		// Handle error
		return FAILED;
	} else if (ret > 0 && FD_ISSET(listen_sockfd, &readfds)) {
		printf("has connection!\n");
		return true;
	}

	return false;

};

Ref<StreamPeerTCP> TCPServerPosix::take_connection() {

	if (!is_connection_available()) {
		return Ref<StreamPeerTCP>();
	};

	struct sockaddr_in their_addr;
	socklen_t sin_size = sizeof(their_addr);
	int fd = accept(listen_sockfd, (struct sockaddr *)&their_addr, &sin_size);
	ERR_FAIL_COND_V(fd == -1, Ref<StreamPeerTCP>());
#ifndef NO_FCNTL
	fcntl(fd, F_SETFL, O_NONBLOCK);
#else
	int bval = 1;
	ioctl(fd, FIONBIO, &bval);
#endif

	Ref<StreamPeerTCPPosix> conn = memnew(StreamPeerTCPPosix);
	IP_Address ip;
	ip.host = (uint32_t)their_addr.sin_addr.s_addr;
	conn->set_socket(fd, ip, ntohs(their_addr.sin_port));

	return conn;
};

void TCPServerPosix::stop() {

	if (listen_sockfd != -1) {
		print_line("CLOSING CONNECTION");
		int ret = NET_NS::close(listen_sockfd);
		ERR_FAIL_COND(ret!=0);
	};

	listen_sockfd = -1;
};


TCPServerPosix::TCPServerPosix() {

	listen_sockfd = -1;
};

TCPServerPosix::~TCPServerPosix() {

	stop();
};
#endif
