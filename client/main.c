
/*****************************************
 * Demonstration IPv4/IPv6-Client
 * Mit unterstützung von multihomed Servern
 *
 * Original von Sascha Hlusiak, Anpassungen an
 * das Projektgerüst von Stefan Gast
 *****************************************/

#define _POSIX_SOURCE 1

#include "common/socket.h"
#include "client/gui/gui_interface.h"

/**
 * sock -> STDOUT
 * STDIN -> sock
 **/

void preparation_onCatalogChanged(const char *newSelection){
    infoPrint("preparation_onCatalogChanged");
}

void preparation_onStartClicked(const char *currentSelection){
    infoPrint("preparation_onStartClicked");
}

void preparation_onWindowClosed(void){
    infoPrint("preparation_onWindowClosed");    
}

void game_onAnswerClicked(int index){
    infoPrint("game_onAnswerClicked");    
}

void game_onWindowClosed(void){
    infoPrint("game_onWindowClosed");    
}


int main(int argc, char ** argv)
{
        int sock;
   
	setProgName(argv[0]);	/* For infoPrint/errorPrint */

	if (argc <= 3)
	{
		errorPrint("Simple Echo-Client");
		errorPrint("Usage: %s [SERVER] [PORT] [USERNAME]", argv[0]);
		errorPrint("Example: %s www.sixxs.net 80 Dieter", argv[0]);
		exit(1);
	}
        
        /**Establish new Connection with server and send Username*/
        
        connect_socket_client(&sock, argv[1], argv[2], argv[3]);
        if (sock==-1){
                errorPrint("Error while connecting to server");
        }
        
        listener_thread_client(sock);
    
        /* GUI */
    
        guiInit(&argc, &argv);
        
        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
    
        preparation_showWindow();
        
        guiMain();       
    
        guiDestroy();    
        
        //char teststring[]="Testtext\n";
        
        //char teststring2[]="Noch n Test";
        
        
        
        //write_client(sock, teststring);
        //write_client(sock, teststring2);
        
        while(1){
        }
        
	return 0;
}