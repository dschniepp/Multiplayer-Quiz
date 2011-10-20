/* all_socket.h für Linux/UNIX */
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

/* Functions */

int connect_socket(socket_t *sock, char *serv_addr, unsigned short port);

/*
void error_exit(char *error_message);
int create_socket( int af, int type, int protocol );
void bind_socket(socket_t
  *sock, unsigned long adress, unsigned short port);
void listen_socket( socket_t *sock );
void accept_socket( socket_t *new_socket, socket_t *socket );
void connect_socket(socket_t *sock, char *serv_addr,
 unsigned short port);
void TCP_send( socket_t *sock, char *data, size_t size);
void TCP_recv( socket_t *sock, char *data, size_t size);
void UDP_send ( socket_t *sock, char *data, size_t size,
  char *addr, unsigned short port);
void UDP_recv( socket_t *sock, char *data, size_t size);
void close_socket( socket_t *sock );
void cleanup(void);
*/

#endif

