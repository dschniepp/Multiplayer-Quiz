
/*****************************************
 * Demonstration IPv4/IPv6-Client
 * Mit unterstützung von multihomed Servern
 *
 * Original von Sascha Hlusiak, Anpassungen an
 * das Projektgerüst von Stefan Gast
 *****************************************/

#define _POSIX_SOURCE 1

//#include "common/socket.h"

//#include "common/message.h"

#include "common/global.h"
#include "common/socket.h"
#include "client/main.h"

/**Module global variables*/

static int sock=0;
static int gui_runs=0;
static pthread_t listener_thr;
static pthread_t gui_thr;

struct GUI_DATA {
	int argc;
        char ** argv;
};

/**Get and set functions*/

pthread_t get_liThread(){
    return listener_thr;
}

pthread_t get_guiThread(){
    return gui_thr;
}

int get_socket(void){
    return sock;
}
int get_guiruns(void){
    return gui_runs;
}

/**GUI-Thread*/

void* gui_thread(void *param){
    
        struct GUI_DATA * gui_data;
        gui_data = (struct GUI_DATA*)param;
    
        guiInit(&gui_data->argc, &gui_data->argv);
    
        infoPrint("GUI-Thread is started --> Socket can use functions!");
        
        /**GUI is initiated -> Listener-Thread can move on!!!!*/  
        sem_post(&semaphore_socket);
        
        /**Set module global variable --> Only for error message, needed (For the case, that the GUI isn't started, yet.*/
        gui_runs=1;
        
        /**Start GUI Main*/
        guiMain();              
        
        infoPrint("Destroy the GUI!!!");
        
        guiDestroy();
        
        free(gui_data);
    
        close_prozess();
        
        pthread_exit(0);    
    //exit(0);
    return NULL;
}

/**Main Function - Command-Thread*/

int main(int argc, char ** argv)
{
        int ret=0;
        
        struct GUI_DATA *gui_data;
        struct GB_LOGIN_REQUEST lg_rq;
        struct GB_CATALOG_REQUEST ca_rq;
        struct GB_QUESTION_REQUEST qu_rq;
        

   
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
        
        /**Initiate new Semaphores*/
        
        if(init_semaphore(semaphore_main) == -1)
	{
                errorPrint("Error while initiating semaphore %s", strerror(errno));
		return 1;
	}
        
        if(init_semaphore(semaphore_socket) == -1)
	{
                errorPrint("Error while initiating semaphore %s", strerror(errno));
		return 1;
	}
        
        /**Start the Listener-Thread*/
                     
	if((pthread_create(&listener_thr, NULL, listener_thread, NULL))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
        }
        
        /**Write LOGIN_REQUEST to server*/
        
        if (strlen(argv[3])<=31){
                strcpy(lg_rq.name, argv[3]);
        }else{  /**cut string after 31 characters*/ //Nachfragen, ob nur die eine Zeile reicht --> strncpy füllt mit Nullbytes auf
                strncpy(lg_rq.name, argv[3], 31);
        }
        
        prepare_message(&lg_rq, TYPE_LG_RQ, strlen(argv[3]));
        ret = write(sock,&lg_rq,(strlen(argv[3])+sizeof(lg_rq.h)));
        test_socketOnErrors(ret);
        //if (ret > 0) {
                infoPrint("Write to socket successful!");
        //}      
        
        /**Wait for Server to send Login_Response_OK, before starting the GUI*/
        sem_wait(&semaphore_main);
        
        infoPrint("MAIN: Login Response is OK!!!");
        
        /*Start the GUI-Thread*/
                        
        gui_data = (struct GUI_DATA*)malloc(sizeof(struct GUI_DATA));
                        
        gui_data->argc=argc;
        gui_data->argv=argv;
        
	if((pthread_create(&gui_thr, NULL, gui_thread, gui_data))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
        }      
        
        /**Write CATALOG_REQUEST to server*/
        
        prepare_message(&ca_rq, TYPE_CA_RQ, 0);
        ret = write(sock,&ca_rq,sizeof(ca_rq.h));
        test_socketOnErrors(ret);
        //if (ret > 0) {
                infoPrint("Write to socket successful!");
        //}
        
        /**Wait, until game phase starts!!!*/
        sem_wait(&semaphore_main);
        
        infoPrint("MAIN: Game phase started!!!");
        
        /**Write QUESTION_REQUEST to server*/
        
        prepare_message(&qu_rq, TYPE_QU_RQ, 0);
        ret = write(sock,&qu_rq,sizeof(qu_rq.h));
        test_socketOnErrors(ret);
        //if (ret > 0) {
                infoPrint("Write to socket successful!");
        //}
        
        /**Block Command-Thread*/
                
        sem_wait(&semaphore_main);

	return 0;
}
