#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "client/common/client_global.h"
#include "client/common/client_socket.h"
#include "client/common/li_qu_thread.h"
#include "client/gui/gui_interface.h"
#include "common/util.h"
#include "client/main.h"

#include "client/common/client_util.h"

static int closeSocketOnEndGame=0;

void set_closeSocketOnEndGame(int clSoOnEnGa){
    closeSocketOnEndGame=clSoOnEnGa;
}

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
                        guiShowErrorDialog("Error: Die Verbindung zum Server wurde getrennt!", 1);
                        sem_wait(&semaphore_socket);
                }else{
                        errorPrint("Error: Die Verbindung zum Server wurde getrennt!");
                        close_process();
                        sem_wait(&semaphore_socket);
                }
	}
	if (ret < 0) {
                /**Avoids the printing of the error message, when the user clicked on the close button, of the window
                  *Problem: When there is no close(get_socket()) in game_onAnswerClicked(), then the game of the other player 
                  *(appears only, if there is only one more player left)
                  *moves on, until the user, who pushed the "x"-Button of his game window, pushes also the OK-Button, of the
                  *appearing message dialog*/
                if(closeSocketOnEndGame!=0){
                
                }else{
                        /**Read/write error*/
                        errorPrint("Cannot read from/write to socket: %s", strerror(errno));
                        if (get_guiruns()!=0){
                                guiShowErrorDialog("Error: Fehler beim schreib/lesezugriff auf den Socket!", 1);
                                sem_wait(&semaphore_socket);
                        }else{
                                errorPrint("Error: Fehler beim schreib/lesezugriff auf den Socket!");
                                close_process();
                                sem_wait(&semaphore_socket);
                        }
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