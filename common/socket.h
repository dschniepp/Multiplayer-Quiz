/* socket.h für Linux/UNIX */
#ifndef SOCKET_H
#define	SOCKET_H

//#include "client/main.h"
/*
struct LISTENER_DATA {
	int sock;
        int pipeID;
};
*/
#include <semaphore.h>



/*--------------Client Functions---------------------*/
pthread_t get_quThread();
void connect_socket_client(int *sock, char serv_addr[], char port[]);
void close_socket_client( int sock );
void* listener_thread();

//void read_pipe_client(int n);

/*--------------Server Functions---------------------*/

void connect_socket_server(int sockets[], int *numsockets, char server[], char service[]);
void close_socket_server( int sockets[], int numsockets );
void accept_loop_server(int fd[], int numfd);

#endif
