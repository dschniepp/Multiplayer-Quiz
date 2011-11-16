
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
#include "common/message.h"
#include "common/global.h"

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
        //struct GB_LOGIN_REQUEST lr;
   
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
        
        //listener_thread_client(sock);
        
        struct GB_LOGIN_REQUEST gb_lr;
        
        if (strlen(argv[3])<=31){
                //GB_LR.h.type = 1;
                strcpy(gb_lr.name, argv[3]);
                //gb_lr.h.size = strlen(argv[3]);
        }
        
        int ret=0;
        
        prepare_message(&gb_lr, TYPE_LR, strlen(argv[3]));
        ret = write(sock,&gb_lr,(strlen(argv[3])+sizeof(gb_lr.h)));
                        if (ret == 0) {
				errorPrint("Connection closed while trying to write");
			}
			if (ret < 0) {
				errorPrint("Cannot write to socket: %s", strerror(errno));
				
			}
                        if (ret > 0) {
                                errorPrint("Write to socket successful!");
                        }
        
        struct GB_NET_HEADER net_head;
        struct GB_LOGIN_RESPONSE_OK lg_rs_ok;
        
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
        switch(net_head.type){
                
                        case 2:
                                errorPrint("Case 2");
                                ret = read(sock, &lg_rs_ok.client_id, sizeof(lg_rs_ok.client_id));
                                if (ret == 0) {
                                        errorPrint("Connection closed while trying to read");
                                }
                                if (ret < 0) {
                                        errorPrint("Cannot read from socket: %s", strerror(errno));
                                }
                                if (ret > 0) {
                                        errorPrint("Read from socket successful: %d!", ntohs(lg_rs_ok.client_id));
                                }
                                break;
                    
                        case 255:
                                errorPrint("Case 255");
                                break;
                            
                        default:
                                errorPrint("default Case");
                                break;
        }
                        
    
        /* GUI */
    /*
        guiInit(&argc, &argv);
        
        //preparation_addPlayer(argv[3]);
        
        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
    
        preparation_addCatalog("Test.cat"); 
    
        preparation_addPlayer("Dani");
    
        preparation_showWindow();
        
        guiMain();       
    
        guiDestroy();    
        
     */   
        
        
        //write_message(sock, GB_LR.h.type);
        
        
        //char teststring[]="Testtext\n";
        
        //char teststring2[]="Noch n Test";
        
        
        
        //write_client(sock, teststring);
        //write_client(sock, teststring2);
        
        while(1){
        }
        
	return 0;
}
