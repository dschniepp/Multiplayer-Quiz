/* socket.h für Linux/UNIX */
#ifndef ALL_SOCKET_H_
#define ALL_SOCKET_H_

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
#include <pthread.h>
#include <sys/un.h>
#include "common/message.h"
#include "common/global.h" 


/*--------------Client Functions---------------------*/

void connect_socket_client(int *sock, char serv_addr[], char port[]);
void close_socket_client( int sock );
void* listener_thread(void *param);
void test_return(int ret);

/*--------------Server Functions---------------------*/

void connect_socket_server(int sockets[], int *numsockets, char server[], char service[]);
void close_socket_server( int sockets[], int numsockets );
void accept_loop_server(int fd[], int numfd);

#endif
