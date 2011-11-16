
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
        int ret;
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
        
        connect_socket_client(&sock, argv[1], argv[2]);
        if (sock==-1){
                errorPrint("Error while connecting to server");
        }
        
        listener_thread_client(sock);
        
        struct GB_LOGIN_REQUEST lg_rq;
        
        if (strlen(argv[3])<=31){
                //GB_LR.h.type = 1;
                strcpy(lg_rq.name, argv[3]);
                //gb_lr.h.size = strlen(argv[3]);
        }
        

        
        prepare_message(&lg_rq, TYPE_LR, strlen(argv[3]));
        ret = write(sock,&lg_rq,(strlen(argv[3])+sizeof(lg_rq.h)));
                        test_return(ret);
                        if (ret > 0) {
                                infoPrint("Write to socket successful!");
                        }      
    
        /* GUI */
    
        guiInit(&argc, &argv);
        
        //preparation_addPlayer(argv[3]);
        
        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
    
        preparation_addCatalog("Test.cat"); 
    
        preparation_addPlayer(argv[3]);
    
        preparation_showWindow();
        
        errorPrint("before GUIMain");
        
        guiMain();       
    
        errorPrint("GUI runs???");
        
        //guiDestroy();    
        
        
        
        
        //write_message(sock, GB_LR.h.type);
        
        
        //char teststring[]="Testtext\n";
        
        //char teststring2[]="Noch n Test";
        
        
        
        //write_client(sock, teststring);
        //write_client(sock, teststring2);
        
        while(1){
        }
        
	return 0;
}
