//#include "client/common/client_util.h"
//#include "client/main.h"
//#include "common/socket.h"
#include "client/common/client_global.h"



/**Function to Initiate semaphores*/

int init_semaphore(sem_t semaphore){

if(sem_init(&semaphore, 0, 0UL) == -1)
	{
		perror("Error while initiating semaphore!!!");
		return -1;
	}
return 0;
}

/**Function to prepare the message header*/

void prepare_message(void * hdr, int type, int size)
{
	struct GB_NET_HEADER* phdr = (struct GB_NET_HEADER*)hdr;

	phdr->size = htons((uint16_t)size);
	phdr->type = (uint8_t)type;
}

/**Function to tests the return value of the read/write command -> sockets*/

void test_socketOnErrors(int ret){
        if (ret == 0) {
            
                /**Server closed connection*/
		errorPrint("Connection closed while trying to read/write");
                if (get_guiruns()!=0){
                        guiShowErrorDialog("Fehler: Die Verbindung zum Server wurde getrennt!", 1);
                }else{
                        errorPrint("Fehler: Die Verbindung zum Server wurde getrennt!");
                        close_process();
                }
	}
	if (ret < 0) {
            
                /**Read/write error*/
		errorPrint("Cannot read from/write to socket: %s", strerror(errno));
                if (get_guiruns()!=0){
                        guiShowErrorDialog("Fehler: Fehler beim schreib/lesezugriff auf den Socket!", 1);
                }else{
                        errorPrint("Fehler: Fehler beim schreib/lesezugriff auf den Socket!");
                        close_process();
                }
	}
}

/**Function to close the hole client-process*/

void close_process(){
    
    /**Destroy semaphores*/
    sem_destroy(&semaphore_main);
    sem_destroy(&semaphore_socket);
    
    /**Close socket*/
    close(get_socket());
    
    /**Detach threads*/
    pthread_detach(get_quThread());
    pthread_detach(get_liThread());
    pthread_detach(get_guiThread());
    
    /**Exit the process*/
    exit(0);
}