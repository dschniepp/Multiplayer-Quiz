
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

struct GUI_DATA {
	int argc;
        char ** argv;
};

void* gui_thread(void *param){
    
    struct GUI_DATA * gui_data;
    gui_data = (struct GUI_DATA*)param;
    
    guiInit(&gui_data->argc, &gui_data->argv);
        
        //preparation_addPlayer(argv[3]);
        
        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
    
        preparation_addCatalog("Test.cat"); 
    
        preparation_addPlayer(gui_data->argv[3]);
    
        preparation_showWindow();
        
        errorPrint("before GUIMain");
        
        guiMain();       
    
        errorPrint("GUI runs???");
        
        guiDestroy();
    
    pthread_exit(0);
    return NULL;
}

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
        struct GUI_DATA *gui_data;
        struct GB_LOGIN_REQUEST lg_rq;
   
	setProgName(argv[0]);	/* For infoPrint/errorPrint */

	if (argc <= 3)
	{
		errorPrint("Simple Echo-Client");
		errorPrint("Usage: %s [SERVER] [PORT] [USERNAME]", argv[0]);
		errorPrint("Example: %s www.sixxs.net 80 Dieter", argv[0]);
		exit(1);
	}
        
        /**Establish new Connection with server*/
        
        connect_socket_client(&sock, argv[1], argv[2]);
        if (sock==-1){
                errorPrint("Error while connecting to server");
        }
        
        /**Start the Listener-Thread*/

	pthread_t listener_thr;
	if((pthread_create(&listener_thr, NULL, listener_thread, (void*)sock))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
        }
        
        /**Write LOGIN_REQUEST to server*/
        
        if (strlen(argv[3])<=31){
                strcpy(lg_rq.name, argv[3]);
        }else{  /**cut string after 31 characters*/ //Nachfragen, ob nur die eine Zeile reicht --> strncpy füllt mit Nullbytes auf
                strncpy(lg_rq.name, argv[3], 31);
        }
        
        prepare_message(&lg_rq, TYPE_LR, strlen(argv[3]));
        ret = write(sock,&lg_rq,(strlen(argv[3])+sizeof(lg_rq.h)));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }      
    
        /*Start the GUI-Thread*/
                        
        gui_data = (struct GUI_DATA*)malloc(sizeof(struct GUI_DATA));
                        
        gui_data->argc=argc;
        gui_data->argv=argv;
        
	pthread_t gui_thr;
	if((pthread_create(&gui_thr, NULL, gui_thread, gui_data))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
        }
            
        //Endless loop to avoid closing at the moment
        
        while(1){
        }
        
	return 0;
}
