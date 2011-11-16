/**socket.c*/
#include "common/socket.h"
#include "common/message.h"

/**--------------Client Functions---------------------*/

/**----Internal Functions*/

struct strread_client {
	int socket;
};

/**Read vom Client --> als eigenen Thread starten*/

void* read_client(void *param)
{
        //struct strread_client * data;
	//data = (struct strread_client*)param;
        //int sock = data->socket;
        
        int sock = (int)param;
        //fd_set set;
	int ret;
	//static char buf[512];
        struct GB_NET_HEADER net_head;
	//void * buf;
        
	while (1) {
		/*int max;
		FD_ZERO(&set);
		FD_SET(sock, &set);
		FD_SET(STDIN_FILENO, &set);
		max = (sock>STDIN_FILENO)?sock:STDIN_FILENO;*/
		
		/* RTFM: select *//*
		ret = select(max+1, &set, NULL, NULL, NULL);
		if (ret <= 0) {
			errorPrint("Error in select: %s", strerror(errno));
			break;
		}*/
		/**
		 * sock -> STD_OUT
		 **/
		/*if (FD_ISSET(sock, &set)) {
			ret = read(sock, buf, sizeof(buf));
			if (ret == 0) {
				break;
			}
			if (ret < 0) {
				errorPrint("Cannot read from socket: %s", strerror(errno));
				break;
			}
                        //Do Stuff, after Reading!!!:-)
                        errorPrint("Buffer: %s", buf);
		}*/
                
                ret = read(sock, &net_head, sizeof(net_head));
			if (ret == 0) {
				errorPrint("Connection closed while trying to read");
			}
			if (ret < 0) {
				errorPrint("Cannot read from socket: %s", strerror(errno));
				
			}
                        if (ret > 0) {
                                errorPrint("Read from socket successful!");
                        }
                
                //print_message(&buf);
                        /*Do Stuff, after Reading!!!:-)*/
                        //errorPrint("Header gelesen!");
                
		/**
		 * STDIN -> sock
		 **/
	}
        pthread_exit(0);
	return NULL;
}


/**----External Functions*/
 
/**Function to connect client sockets
 * Call-By-Refferenz Übergabe --> sock
 * Je nach Fall, wird folgendes in sock geschrieben:
 * Connected	=	Socketnr
 * Error		=	-1
 * --> Prüfe sock nach dem Aufruf auf -1 !!!
 **/
 
void connect_socket_client(int *sock, char serv_addr[], char port[], char username[]){
 
	struct addrinfo *addr_info, *p, hints;
	int ret;
	/**Set the socket at the beginning to -1 and change value if connection works!*/
		
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0 /** | AI_ADDRCONFIG */;
	
	
		/**RTFM: getaddrinfo */
        ret = getaddrinfo(serv_addr, port, &hints, &addr_info);
        if (ret){
			errorPrint("getaddrinfo: %s", gai_strerror(ret));
			exit(1);
		}

        p = addr_info;
        while (p){
			
			char dst[INET6_ADDRSTRLEN];

			/**Create socket for found family */		
                        *sock = socket(p->ai_family, p->ai_socktype, 0);

			/**RTFM: getnameinfo */
			getnameinfo(p->ai_addr,
			p->ai_addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

			/**Try to connect */
                        if (connect(*sock, p->ai_addr, p->ai_addrlen) == 0){
                                infoPrint("Connected");
                                break;
				
			}else{
				errorPrint("Error, while trying %s",dst);
			}
			p = p->ai_next;		
		}
        freeaddrinfo(addr_info);
        //write_client(*sock, username);
}
/**Function to close Clientsocket*/

void close_socket_client( int sock ){
    close(sock);
}

/**Write vom Client*/

void write_client(int sock, char buf[])
{
	/*fd_set set;
	int ret;*/
        size_t size = strlen(buf);
        write(sock, buf, size);
	/*
        int max;
	FD_ZERO(&set);
	FD_SET(sock, &set);
	FD_SET(STDIN_FILENO, &set);
	max = (sock>STDIN_FILENO)?sock:STDIN_FILENO;
		
	// RTFM: select
        if (FD_ISSET(sock, &set)) {
                ret = select(max+1, NULL, &set, NULL, NULL);
                if (ret <= 0) {
                        errorPrint("Error in select: %s", strerror(errno));
                }
	
                if (write(sock, buf, size) < ret) {
			errorPrint("Cannot write to socket: %s", strerror(errno));
                }
        }
	*/	
}

/**Create a new Command_Thread for server read*/

void listener_thread_client(int sock){

	pthread_t command_thread;
        //struct strread_client* data;
        //data = (struct strread_client*)malloc(sizeof(struct strread_client));
        //data->socket=sock;
	// Background new connection
	if((pthread_create(&command_thread, NULL, read_client, (void*)sock))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
	};

}

/**--------------Server Functions---------------------*/

/**----Internal Functions*/

/**
 * struct to pass custom data to echo_thread
 **/
struct client_data {
	int sock;
	struct sockaddr_storage addr;
	socklen_t addrlen;
};


/**
 * Copy all data from fd -> fd
 * using a buffer of 512 Byte
 **/
static void echo_loop(int fd)
{
	int ret;
	static char buf[512];

	while (1) {
		ret = read(fd, buf, sizeof(buf));
                infoPrint("official readtest :-)/n");
		if (ret == 0) {
			break;
		}
		if (ret < 0) {
			errorPrint("Cannot read: %s", strerror(errno));
			break;
		}
		if (write(fd, buf, ret) < ret) {
			errorPrint("Cannot write: %s", strerror(errno));
			break;
		}
	}
}

/**
 * Thread to handle connection in background and run echo_loop
 * param: struct client_data*
 **/
void* echo_thread(void* param)
{
	char dst[INET6_ADDRSTRLEN];
	struct client_data * data;
	data = (struct client_data*)param;

	/* RTFM: getnameinfo */
	getnameinfo((struct sockaddr*)&data->addr,
			data->addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

	infoPrint("Connection opened from %s",dst);
	echo_loop(data->sock);
	close(data->sock);
	infoPrint("Connection closed from %s", dst);

	free(data);

	pthread_exit(0);
	return NULL;
}

/**----External Functions*/

/**Function to connect server sockets
 * Call-By-Refferenz Übergabe --> sockets, numsockets
 * Je nach Fall, wird folgendes in sockets geschrieben:
 * Connected	=	Socketummern
 * Error		=	sockets(0) = -1
 * --> Prüfe sockets(0) nach dem Aufruf auf -1 !!!
 **/



void connect_socket_server(int sockets[], int *numsockets, char server[], char service[]){

	struct addrinfo *addr_info, *p, hints;
	int ret;
	*numsockets = 0;
	/**Set the socket at the beginning to -1 and change value if connection works!*/
	sockets[*numsockets] = -1;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	/* RTFM: getaddrinfo */
	ret = getaddrinfo(server, service, &hints, &addr_info);
	if (ret)
	{
		errorPrint("Error in getaddrinfo: %s", gai_strerror(ret));
		exit(1);
	}

	p = addr_info;
	while (p)
	{
		int s;
		char dst[INET6_ADDRSTRLEN];
		char service[INET6_ADDRSTRLEN];
		int on = 1;

		/* Create socket for found family */		
		s = socket(p->ai_family, p->ai_socktype, 0);

		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
			errorPrint("Error in setsockopt: %s", strerror(errno)); /* maybe not so fatal, continue */
		}
		if (p->ai_family == AF_INET6) {
			if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
				errorPrint("Error in setsockopt: %s", strerror(errno));
			}
		}

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
				p->ai_addrlen,
				dst,
				sizeof(dst),
				service,
				sizeof(service),
				NI_NUMERICHOST | NI_NUMERICSERV);

		infoPrint("Trying %s:%s ... ",dst, service);

		/* Try to bind socket */
		if (bind(s, p->ai_addr, p->ai_addrlen) == 0) {
			if (listen(s, 1) < 0) {
				errorPrint("listen failed: %s", strerror(errno));
				close(s);
			} else 	{
				infoPrint("bind successful");
				sockets[*numsockets] = s;
                                *numsockets = *numsockets + 1;
			}
		} else {
			errorPrint("bind failed: %s", strerror(errno));
			close(s);
		}

		p = p->ai_next;
	}
	freeaddrinfo(addr_info);
}

void close_socket_server( int sockets[], int numsockets ){
    int i;
	for (i=0; i<numsockets; i++) {
		close(sockets[i]);
	}
}

/**
 * Wait for connection on all available sockets
 * fd: array of sockets in listen state
 * numfd: number of valid sockets in fd
 **/

/**Create a new Accep_Loop_Thread to read from the client*/

void accep_loop_thread_server(int sock){

	pthread_t command_thread;
        //struct strread_client* data;
        //data = (struct strread_client*)malloc(sizeof(struct strread_client));
        //data->socket=sock;
	// Background new connection
	if((pthread_create(&command_thread, NULL, read_client, (void*)sock))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
	};

}

void accept_loop_server(int fd[], int numfd)
{
	fd_set set;
	int max, i, ret;

	if (numfd < 1) {
		errorPrint("No sockets available!");
		return;
	}
	while (1) {
		max = 0;
		FD_ZERO(&set);
		for (i=0; i<numfd; i++) {
			if (fd[i] > max)
				max = fd[i];
			FD_SET(fd[i], &set);
		}

		/* wait for first fd that has data */
		ret = select(max+1, &set, NULL, NULL, NULL);
		if (ret <= 0) {
			errorPrint("Error in select: %s", strerror(errno));
			return;
		}
		for (i=0; i<numfd; i++)
			if (FD_ISSET(fd[i], &set)) {
				struct client_data *data;

				data = (struct client_data*)malloc(sizeof(struct client_data));

				data->addrlen = sizeof(data->addr);
				data->sock = accept(fd[i], (struct sockaddr*) &data->addr, &data->addrlen);

				if (data->sock < 0) {
					errorPrint("Error in accept: %s", strerror(errno));
					free(data);
				} else {
					pthread_t thread_id;
					/* Background new connection */
					if((pthread_create(&thread_id, NULL, echo_thread, data))!=0){
						errorPrint("Error while creating thread: %s", strerror(errno));
					};
				}
			}
	}
}

