/*****************************************
 * Demonstration IPv4/IPv6-Client
 * Mit unterstützung von multihomed Servern
 *
 * Original von Sascha Hlusiak, Anpassungen an
 * das Projektgerüst von Stefan Gast
 *****************************************/

#define _POSIX_SOURCE 1
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common/util.h"


/**
 * sock -> STDOUT
 * STDIN -> sock
 **/

static void echo_loop(int sock)
{
	fd_set set;
	int ret;
	static char buf[512];
	
	while (1) {
		int max;
		FD_ZERO(&set);
		FD_SET(sock, &set);
		FD_SET(STDIN_FILENO, &set);
		max = (sock>STDIN_FILENO)?sock:STDIN_FILENO;
		
		/* RTFM: select */
		ret = select(max+1, &set, NULL, NULL, NULL);
		if (ret <= 0) {
			errorPrint("Error in select: %s", strerror(errno));
			break;
		}
		/**
		 * sock -> STD_OUT
		 **/
		if (FD_ISSET(sock, &set)) {
			ret = read(sock, buf, sizeof(buf));
			if (ret == 0) {
				break;
			}
			if (ret < 0) {
				errorPrint("Cannot read from socket: %s", strerror(errno));
				break;
			}
			if (write(STDOUT_FILENO, buf, ret) < ret) {
				errorPrint("Cannot write to stdout: %s", strerror(errno));
				break;
			}
		}
		/**
		 * STDIN -> sock
		 **/
		if (FD_ISSET(STDIN_FILENO, &set)) {
			ret = read(STDIN_FILENO, buf, sizeof(buf));
			if (ret == 0) {
				break;
			}
			if (ret < 0) {
				errorPrint("Cannot read from stdin: %s", strerror(errno));
				break;
			}
			if (write(sock, buf, ret) < ret) {
				errorPrint("Cannot write to socket: %s", strerror(errno));
				break;
			}
		}
	}  
}


int main(int argc, char ** argv)
{
        struct addrinfo *addr_info, *p, hints;
	int ret;

	setProgName(argv[0]);	/* For infoPrint/errorPrint */

	if (argc <= 2)
	{
		errorPrint("Simple Echo-Client");
		errorPrint("Usage: %s [SERVER] [PORT]", argv[0]);
		errorPrint("Example: %s www.sixxs.net 80", argv[0]);
		exit(1);
	}


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0 /* | AI_ADDRCONFIG */;

	/* RTFM: getaddrinfo */
        ret = getaddrinfo(argv[1], argv[2], &hints, &addr_info);
        if (ret)
	{
		errorPrint("getaddrinfo: %s", gai_strerror(ret));
		exit(1);
	}

        p = addr_info;
        while (p)
        {
		int s;
		char dst[INET6_ADDRSTRLEN];

		/* Create socket for found family */		
                s = socket(p->ai_family, p->ai_socktype, 0);

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
			p->ai_addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

		infoPrint("Trying %s ... ",dst);

		/* Try to connect */
                if (connect(s, p->ai_addr, p->ai_addrlen) == 0)
                {
                        infoPrint("Connected");
			
			/* Do stuff when connected*/
			echo_loop(s);
		
			close(s);
			break;
                } else {
			errorPrint("failed to connect: %s", strerror(errno));
		}
		close(s);

		p = p->ai_next;
        }

        freeaddrinfo(addr_info);
        
	return 0;
}


