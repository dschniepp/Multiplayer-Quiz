
/*****************************************
 * Demonstration IPv4/IPv6-Client
 * Mit unterstützung von multihomed Servern
 *
 * Original von Sascha Hlusiak, Anpassungen an
 * das Projektgerüst von Stefan Gast
 *****************************************/

#define _POSIX_SOURCE 1

#include "common/socket.h"

/**
 * sock -> STDOUT
 * STDIN -> sock
 **/




int main(int argc, char ** argv)
{
        int sock;
        //char teststring[20];
        size_t str_length;
   
	setProgName(argv[0]);	/* For infoPrint/errorPrint */

	if (argc <= 2)
	{
		errorPrint("Simple Echo-Client");
		errorPrint("Usage: %s [SERVER] [PORT]", argv[0]);
		errorPrint("Example: %s www.sixxs.net 80", argv[0]);
		exit(1);
	}
        
        connect_socket_client(&sock, argv[1], argv[2]);
        if (sock==-1){
                errorPrint("Error while connecting to server");
        }
        
        char teststring[]="Testtext";
        str_length=strlen(teststring);
        
        write_client(sock, teststring, str_length);
        
        command_thread_client(sock);
        
        while(1){
        }
        
	return 0;
}