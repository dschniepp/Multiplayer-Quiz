/* 
 * File:   client_socket.h
 * Author: sysprog
 *
 * Created on November 28, 2011, 5:05 PM
 */

#ifndef CLIENT_SOCKET_H
#define	CLIENT_SOCKET_H

#ifdef	__cplusplus
extern "C" {
#endif

void connect_socket_client(int *sock, char serv_addr[], char port[]);
void close_socket_client( int sock );

#ifdef	__cplusplus
}
#endif

#endif	/* CLIENT_SOCKET_H */

