
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
        size_t str_length2;
   
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
        
        char teststring[]="Testtext\n";
        str_length=strlen(teststring);
        
        char teststring2[]="Noch n Test";
        str_length2=strlen(teststring2);
        
        command_thread_client(sock);
        
        write_client(sock, teststring, str_length);
        write_client(sock, teststring2, str_length2);
        
        while(1){
        }
        
	return 0;
}