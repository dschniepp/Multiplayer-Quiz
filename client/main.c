
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

struct GUI_DATA {
	int argc;
        char ** argv;
};

void* gui_thread(void *param){
    
    struct GUI_DATA * gui_data;
    gui_data = (struct GUI_DATA*)param;
    
    guiInit(&gui_data->argc, &gui_data->argv);
    
        infoPrint("Semaphore UP() -> Wait until GUI Preparations are set");
        sem_post(&semaphore_socket);
    
        //preparation_setMode(PREPARATION_MODE_PRIVILEGED);
    
        //preparation_addCatalog("Test.cat"); 
    
        //preparation_addPlayer(gui_data->argv[3]);
    
        //preparation_showWindow();
        
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
        struct GB_CATALOG_REQUEST ca_rq;
        struct GB_CATALOG_CHANGE ca_ch;
        struct GB_START_GAME st_ga;
        struct GB_QUESTION_REQUEST qu_rq;
        
        struct LISTENER_DATA *li_da;
        char *client_id;
              
        char cat_name[]="simple.cat"; /**Catalog name for testing*/
   
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
        
        if(init_semaphore(semaphore_gui) == -1)
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
        
        li_da = (struct LISTENER_DATA*)malloc(sizeof(struct LISTENER_DATA));
        
        li_da->sock=sock;
        li_da->pipeID=0;                
	pthread_t listener_thr;
	if((pthread_create(&listener_thr, NULL, listener_thread, li_da))!=0){
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
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }      
        
        /**Wait for Server to send Login_Response_OK, before starting the GUI*/
        sem_wait(&semaphore_main);
        infoPrint("Semaphore DOWN()");
        
        infoPrint("PIPE-ID from Main= %d",li_da->pipeID);
        
        /*if(pipe(li_da->pipeID)==-1){
                errorPrint("Error, while creating stdinPipe");
        }*/
        /*
        int n=1;
        int z=0;
        char *output;
        output = (char *)malloc((n+1)*(sizeof(char)));
        while(z<n){
                read(*li_da->pipeID, &output[z], 1);
        z++;
        }
        output[z]=0;
        infoPrint("Anzahl: %d", n);
        infoPrint("Read Pipe: %s!", output);
        */
        //infoPrint("Read from Pipe, client-id: %s",client_id);
        
        /*Start the GUI-Thread*/
                        
        gui_data = (struct GUI_DATA*)malloc(sizeof(struct GUI_DATA));
                        
        gui_data->argc=argc;
        gui_data->argv=argv;
        
	pthread_t gui_thr;
	if((pthread_create(&gui_thr, NULL, gui_thread, gui_data))!=0){
		errorPrint("Error while creating thread: %s", strerror(errno));
        }
        
        
        
        /**Write CATALOG_REQUEST to server*/
        
        prepare_message(&ca_rq, TYPE_CA_RQ, 0);
        ret = write(sock,&ca_rq,sizeof(ca_rq.h));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }
        
        sem_wait(&semaphore_main);
        
        /**Write CATALOG_CHANGE to server*/
        /**allocate memory to write*/
        /*
        ca_ch.catalog_msg = (char *)malloc(sizeof(cat_name)*sizeof(char));
        strcpy(ca_ch.catalog_msg,cat_name);
        infoPrint("send catalog_changed: %s", ca_ch.catalog_msg);
        prepare_message(&ca_ch, TYPE_CA_CH, strlen(cat_name));
        ret = write(sock,&ca_ch,(strlen(cat_name)+sizeof(ca_ch.h)));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }
        */
        
        /**Write START_GAME to server*/
        /**allocate memory to write*/
        /*st_ga.catalog_msg = (char *)malloc(sizeof(cat_name)*sizeof(char));
        strcpy(st_ga.catalog_msg,cat_name);
        prepare_message(&st_ga, TYPE_ST_GA, 0);
        ret = write(sock,&st_ga,strlen(cat_name)+sizeof(st_ga.h));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }
        */
        
        /**Write QUESTION_REQUEST to server*/
        /*
        prepare_message(&qu_rq, TYPE_QU_RQ, 0);
        ret = write(sock,&qu_rq,sizeof(qu_rq.h));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }
        */
        //Endless loop to avoid closing at the moment
        
        while(1){
        }
        
	return 0;
}
