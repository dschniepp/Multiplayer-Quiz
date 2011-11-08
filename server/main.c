/*****************************************
 * Demonstration IPv4/IPv6-Server
 * Mit unterstützung von multihomed Servern
 *
 * Original von Sascha Hlusiak, Modifikationen
 * für das Projektgerüst von Stefan Gast
 *****************************************/

#define _POSIX_SOURCE 1

#include "common/socket.h"

/**
 * struct to pass custom data to echo_thread
 **/
/*
struct client_data {
	int sock;
	struct sockaddr_storage addr;
	socklen_t addrlen;
};
*/
int main(int argc, char ** argv)
{
	char *server, *service;
	struct addrinfo;
	int i;
	int sockets[128];
	int numsockets = 0;

	setProgName(argv[0]);	/* For our infoPrint/errorPrint functions */

	if (argc <= 1)
	{
		errorPrint("Simple Echo-Server");
		errorPrint("Usage: %s [BINDADDR] [SERVICE]", argv[0]);
		errorPrint("Example: %s localhost 8080", argv[0]);
		exit(1);
	}
	if (argc == 3) {
		server = argv[1];
		service = argv[2];
	} else {
		server = NULL;
		service = argv[1];
	}

        connect_socket_server(sockets, &numsockets, server, service);
        if (sockets[0]==-1){
            errorPrint("Error while opening socket connection");
        }
        errorPrint("sockets[0]=%d", sockets[0]);
        errorPrint("numsockets=%d", numsockets);
        
        //printf("sockets[0]=%d, numsockets=%d",sockets[0], numsockets);
        accept_loop_server(sockets, numsockets);
	
        while(1){
        }
        
	for (i=0; i<numsockets; i++) {
		close(sockets[i]);
	}
	return 0;
}


