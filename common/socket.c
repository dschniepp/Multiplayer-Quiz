/**socket.c*/
#include "common/socket.h"
#include "common/global.h"
#include "client/gui/gui_interface.h"
#include "client/main.h"
#include <unistd.h>

static pthread_t question_thr;
//static int stdPipe[2];
/**--------------Client Functions---------------------*/

/**----External Functions*/

void close_prozess(){
    infoPrint("Before destroying of semaphores!!!");
    sem_destroy(&semaphore_main);
    sem_destroy(&semaphore_socket);
    close(get_socket());
    pthread_detach(question_thr);
    pthread_detach(get_liThread());
    pthread_detach(get_guiThread());
    infoPrint("Before exit!!!");
    exit(0);
}

int init_semaphore(sem_t semaphore){

if(sem_init(&semaphore, 0, 0UL) == -1)
	{
		perror("Error while initiating semaphore!!!");
		return -1;
	}
return 0;
}

/**Tests the return value of the read/write command -> sockets*/

void test_return(int ret){
    	if (ret == 0) {
		errorPrint("Connection closed while trying to read/write");
                if (get_guiruns()!=0){
                        guiShowErrorDialog("Fehler: Die Verbindung zum Server wurde getrennt!", 1);
                }else{
                        errorPrint("Fehler: Die Verbindung zum Server wurde getrennt!");
                        close_prozess();
                }
	}
	if (ret < 0) {
		errorPrint("Cannot read from/write to socket: %s", strerror(errno));		
	}
}

void* question_thread(){
       
    struct GB_QUESTION_REQUEST qu_rq;
    int ret=0;
    
    /**Write QUESTION_REQUEST to server*/
        
    prepare_message(&qu_rq, TYPE_QU_RQ, 0);
    ret = write(get_socket(),&qu_rq,sizeof(qu_rq.h));
    test_return(ret);
    if (ret > 0) {
        infoPrint("Write to socket successful!");
    }
    sleep(4);
    sem_post(&semaphore_socket);
    pthread_exit(0);
    return NULL;
}

/**Listener-Thread client*/

void* listener_thread()
{      
	int ret;
        int sock=get_socket();
        struct GB_NET_HEADER net_head;
        struct GB_LOGIN_RESPONSE_OK lg_rs_ok;
        struct GB_Player_List pl_li[6];
        struct GB_CATALOG_RESPONSE ca_rp[11]; //Maximum of 10 catalogs
        struct GB_CATALOG_CHANGE ca_ch;
        struct GB_START_GAME st_ga;
        struct GB_QUESTION qu;
        struct GB_QUESTION_RESULT qu_re;
        struct GB_GAME_OVER ga_ov;
        struct GB_Error_Warning er_wa;
        
        
        
        int phase=0; /**The current phase of the game -> 0=preparation; 1=game*/
        int ca_rp_counter=0; /**Counts the number of loaded catalogs --> Maximum=10*/
        int z;
            
	while (1) {
		ret = read(sock, &net_head, sizeof(net_head));
                        test_return(ret);
                        if (ret > 0) {
                                infoPrint("Read from socket successful!");
                                infoPrint("NetHead_type: %d!", net_head.type);
                                infoPrint("NetHead_size: %d!", ntohs(net_head.size));
                        }
                switch(net_head.type){
                
                        case TYPE_LG_RQ_OK:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 2");
                                
                                ret = read(sock, &lg_rs_ok.client_id, ntohs((net_head.size)));
                                test_return(ret);
                                if (ret > 0) {
                                    infoPrint("Client_ID: %d!", lg_rs_ok.client_id);
                                    
                                        //char id[1];
                                        //sprintf(id,"%d",lg_rs_ok.client_id);
                                        infoPrint("Semaphore UP() -> Wait until GUI starts");
                                        sem_post(&semaphore_main);
                                        infoPrint("Listener Thread Moves on, after Semaphore UP()");
                                        
                                        sem_wait(&semaphore_socket);
                                        
                                        if(lg_rs_ok.client_id != 0){
                                        preparation_setMode(PREPARATION_MODE_NORMAL);
                                                }else{
                                        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
                                        }                                      
                                }
                                break;
                        case TYPE_CA_RP:
                                infoPrint("Case 4");
                                
                                if (ca_rp_counter<10){
                                        if ((ntohs((net_head.size)))!=0){
                                        
                                                ca_rp[ca_rp_counter].catalog_msg = (char *)malloc(((ntohs(net_head.size))+1)*sizeof(char));
                                                z=0;
                                        
                                                while (z<((ntohs(net_head.size)))){
                                                        ret = read(sock, &ca_rp[ca_rp_counter].catalog_msg[z],1);
                                                        z++;
                                                }
                                        
                                                ca_rp[ca_rp_counter].catalog_msg[z]='\0';
                                        
                                                if (ret > 0) {
                                                        infoPrint("Catalog Nr.%d: %s!",ca_rp_counter, ca_rp[ca_rp_counter].catalog_msg);
                                                        preparation_addCatalog(ca_rp[ca_rp_counter].catalog_msg);
                                                        free(ca_rp[ca_rp_counter].catalog_msg);
                                                }
                                                        ca_rp[ca_rp_counter].h.type=TYPE_CA_RP;
                                                        ca_rp[ca_rp_counter].h.size=(ntohs(net_head.size));
                                        }else{
                                                infoPrint("All catalogs read!");
                                                ca_rp[ca_rp_counter].h.type=TYPE_CA_RP;
                                                ca_rp[ca_rp_counter].h.size=(ntohs(net_head.size));
                                                preparation_showWindow();
                                        }
                                }else{
                                        if(ca_rp_counter==10){
                                                ca_rp[ca_rp_counter].h.type=TYPE_CA_RP;
                                                ca_rp[ca_rp_counter].h.size=0;
                                                errorPrint("Error: You can not show more then 10 catalogs!");
                                        }   
                                }
                                
                                ca_rp_counter++;
                                break;
                                
                                
                        case TYPE_CA_CH:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 5");
                                
                                ca_ch.catalog_msg = (char *)malloc(((ntohs(net_head.size))+1)*sizeof(char));
                                
                                z=0;
                                while (z<(ntohs(net_head.size))){
                                    ret = read(sock, &ca_ch.catalog_msg[z],1);
                                    //infoPrint("changed cat [%d]: %s", z, ca_ch.catalog_msg);
                                    z++;
                                }
                                ca_ch.catalog_msg[z]='\0';
                                if (ret > 0) {
                                        errorPrint("Changed Catalog: %s!",ca_ch.catalog_msg);
                                        preparation_selectCatalog(ca_ch.catalog_msg     );
                                }
                                free(ca_ch.catalog_msg);
                                break;
                        case TYPE_PL_LI:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 6");
                                
                                /**Clear the Playerlist*/
                                
                                preparation_clearPlayers();
                                
                                /**Read the Playerlist from the socket and show it in the GUI*/
                                
                                z=0;
                                while (z<((ntohs((net_head.size)))/37)){
                                
                                        ret = read(sock, &pl_li[z].playername, 32);
                                        test_return(ret);
                                        if (ret > 0) {
                                                infoPrint("playername: %s!", pl_li[z].playername);
                                        }
                                        ret = read(sock, &pl_li[z].score, 4);
                                        test_return(ret);
                                        if (ret > 0) {
                                                infoPrint("score: %d!", ntohl(pl_li[z].score));
                                        }
                                        ret = read(sock, &pl_li[z].client_id, 1);
                                        test_return(ret);
                                        if (ret > 0) {
                                                infoPrint("Client_ID: %d!", pl_li[z].client_id);
                                        }
                                        switch(phase){
                                                case 0: preparation_addPlayer(pl_li[z].playername);
                                                        break;
                                                case 1: game_setPlayerName(z+1,pl_li[z].playername);
                                                        game_setPlayerScore(z+1,ntohl(pl_li[z].score));
                                                        if(lg_rs_ok.client_id==pl_li[z].client_id){
                                                            game_highlightPlayer(z+1);
                                                        }
                                                        break;
                                                default:errorPrint("Unknown phase --> neither preparation, nor game");
                                                        break;
                                        }                
                                        z++;
                                }
                                break;
                        case TYPE_ST_GA:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 7");
                                if ((ntohs(net_head.size))!=0){
                                        st_ga.catalog_msg = (char *)malloc(((ntohs(net_head.size)))*sizeof(char));
                                
                                        z=0;
                                        while (z<((ntohs(net_head.size)))){
                                                ret = read(sock, &st_ga.catalog_msg[z],1);
                                                z++;
                                        }
                                        st_ga.catalog_msg[z]='\0';
                                        if (ret > 0) {
                                                infoPrint("Start_Game Message: %s!",st_ga.catalog_msg);
                                        }
                                        free(st_ga.catalog_msg);
                                }       
                                infoPrint("Server is ready to start the game! (Start_Game Message received)");
                                preparation_hideWindow();
                                phase=1;
                                game_showWindow();
                                sem_post(&semaphore_main);
                                break;
                                
                        case TYPE_QU:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 9");
                                if ((ntohs(net_head.size))!=0){
                                        ret = read(sock, &qu.question, 256);
                                        test_return(ret);
                                        if (ret > 0) {
                                        infoPrint("Question: %s!", qu.question);
                                        game_setQuestion(qu.question);
                                        }
                                        z=0;
                                        while(z<4){
                                                ret = read(sock, &qu.answer[z], 128);
                                                test_return(ret);
                                                if (ret > 0) {
                                                        infoPrint("Answer[%d]: %s!",z+1, qu.answer[z]);
                                                        game_setAnswer(z,qu.answer[z]);
                                                }
                                                z++;
                                        }
                                        ret = read(sock, &qu.time, 2);
                                        test_return(ret);
                                        if (ret > 0) {
                                                infoPrint("Question: %d!", ntohs(qu.time));
                                        }
                                        game_setStatusIcon(STATUS_ICON_NONE);
                                        game_setStatusText("");
                                        game_unmarkAnswers();
                                        game_setAnswerButtonsEnabled(1);
                                }else{
                                    /**No more questions left!!!*/
                                    infoPrint("No more questions left!!!");
                                    game_hideWindow();
                                }
                                break;
                                
                        case TYPE_QU_RE:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 11");
                                
                                ret = read(sock, &qu_re.answer, 1);
                                test_return(ret);
                                if (ret > 0) {
                                    infoPrint("Answer was: %d!", qu_re.answer);
                                }
                                ret = read(sock, &qu_re.correct, 1);
                                test_return(ret);
                                if (ret > 0) {
                                    infoPrint("Correct answer is: %d!", qu_re.correct);
                                }
                                if (qu_re.answer!=255){
                                    if(qu_re.answer!=qu_re.correct){
                                        game_markAnswerWrong(qu_re.answer);
                                        game_markAnswerCorrect(qu_re.correct);
                                        game_setStatusText("Die Antwort war falsch!");
                                        game_setStatusIcon(STATUS_ICON_WRONG);
                                    }else{
                                        game_markAnswerCorrect(qu_re.answer);
                                        game_setStatusText("Die Antwort war richtig!");
                                        game_setStatusIcon(STATUS_ICON_CORRECT);
                                    }
                                    
                                }else{
                                    game_markAnswerCorrect(qu_re.correct);
                                    game_setStatusText("Die Zeit ist leider abgelaufen!");
                                    game_setStatusIcon(STATUS_ICON_TIMEOUT);
                                }
                                game_setAnswerButtonsEnabled(0);
                                
                                
                                if((pthread_create(&question_thr, NULL, question_thread, NULL))!=0){
                                        errorPrint("Error while creating question change thread: %s", strerror(errno));
                                }
                                sem_wait(&semaphore_socket);
                                break;
                        
                        case TYPE_GA_OV:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 12");
                                
                                ret = read(sock, &ga_ov.rank, 1);
                                test_return(ret);
                                if (ret > 0) {
                                    infoPrint("Rank: %d!", ga_ov.rank);
                                }
                                /*
                                char textpart1[]="Glückwunsch Sie haben den ";
                                char textpart2[]=" Platz belegt!";
                                
                                //ca_ch.catalog_msg = (char *)malloc(((ntohs(net_head.size))+1)*sizeof(char));
                                
                                char *rank = (char *)malloc(sizeof(uint8_t));
 
                                memcpy(rank, &ga_ov.rank, sizeof(int) );

                                strncat(textpart1,rank,1);
                                strncat(textpart1,textpart2,14);
 
                                // ints senden und am Ende freigeben
 
                                free(rank);
                                */
                                char message[50];
                                sprintf(message,"Glueckwusch Sie haben den %dten Platz belegt!",ga_ov.rank);
                                infoPrint("Message: %s",message);
                                guiShowMessageDialog(message, 1);
                                sem_wait(&semaphore_socket);
                                //exit(0);
                                break;
                                
                        case TYPE_ER_WA:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 255");
                                ret = read(sock, &er_wa.msg_type, 1);
                                test_return(ret);
                                if (ret > 0) {
                                        errorPrint("Errortype: %d!",er_wa.msg_type);
                                }
                                
                                //errorPrint("Net_Head size: %d", ((ntohs(net_head.size))-1));
                                
                                er_wa.error_msg = (char *)malloc(((ntohs(net_head.size))-1)*sizeof(char));
                                
                                //er_wa.error_msg[ntohs((net_head.size))-1];
                                z=0;
                                while (z<((ntohs(net_head.size))-1)){
                                    ret = read(sock, &er_wa.error_msg[z],1);
                                    z++;
                                }
                                er_wa.error_msg[z]='\0';
                                
                                //ret = read(sock, &er_wa.error_msg,((ntohs(net_head.size))-1));
                                
                                if (ret > 0) {
                                        
                                    switch(er_wa.msg_type){
                                        
                                        case 0: guiShowMessageDialog(er_wa.error_msg, 0);
                                                free(er_wa.error_msg);
                                                break;
                                        case 1: if (get_guiruns()!=0){
                                                        guiShowErrorDialog(er_wa.error_msg, 1);
                                                        free(er_wa.error_msg);
                                                        sem_wait(&semaphore_socket);
                                                }else{
                                                        errorPrint("Error: %s!", er_wa.error_msg);
                                                        free(er_wa.error_msg);
                                                        close_prozess();
                                                }
                                                break;
                                        default:
                                                errorPrint("Error: Unknown case, while printing error/warning message!!!");
                                                break;
                                        
                                    }
                                    
                                        //errorPrint("Error: %s!", er_wa.error_msg);
                                }
                                break;
                            
                        default:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("default Case");
                                break;
                }
        //t++;
        //break;
	}
        pthread_exit(0);
	return NULL;
}
 
/**Function to connect client sockets
 * Call-By-Refferenz Übergabe --> sock
 * Je nach Fall, wird folgendes in sock geschrieben:
 * Connected	=	Socketnr
 * Error	=	-1
 * --> Prüfe sock nach dem Aufruf auf -1 !!!
 **/
 
void connect_socket_client(int *sock, char serv_addr[], char port[]){
 
	struct addrinfo *addr_info, *p, hints;
	int ret;
	/**Set the socket at the beginning to -1 and change value if connection works!*/
		
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0 /** | AI_ADDRCONFIG */;
	
	
		/**RTFM: getaddrinfo */
        ret = getaddrinfo(serv_addr, port, &hints, &addr_info);
        if (ret){
			errorPrint("getaddrinfo: %s", gai_strerror(ret));
			exit(1);
		}

        p = addr_info;
        while (p){
			
			char dst[INET6_ADDRSTRLEN];

			/**Create socket for found family */		
                        *sock = socket(p->ai_family, p->ai_socktype, 0);

			/**RTFM: getnameinfo */
			getnameinfo(p->ai_addr,
			p->ai_addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

			/**Try to connect */
                        if (connect(*sock, p->ai_addr, p->ai_addrlen) == 0){
                                infoPrint("Connected");
                                break;
				
			}else{
				errorPrint("Error, while trying %s",dst);
			}
			p = p->ai_next;		
		}
        freeaddrinfo(addr_info);
}
/**Function to close client socket*/

void close_socket_client( int sock ){
    close(sock);
}

/**--------------Server Functions---------------------*/

/**----Internal Functions*/

/**
 * struct to pass custom data to echo_thread
 **/
struct client_data {
	int sock;
	struct sockaddr_storage addr;
	socklen_t addrlen;
};


/**
 * Copy all data from fd -> fd
 * using a buffer of 512 Byte
 **/
static void echo_loop(int fd)
{
	int ret;
	static char buf[512];

	while (1) {
		ret = read(fd, buf, sizeof(buf));
                infoPrint("official readtest :-)/n");
		if (ret == 0) {
			break;
		}
		if (ret < 0) {
			errorPrint("Cannot read: %s", strerror(errno));
			break;
		}
		if (write(fd, buf, ret) < ret) {
			errorPrint("Cannot write: %s", strerror(errno));
			break;
		}
	}
}

/**
 * Thread to handle connection in background and run echo_loop
 * param: struct client_data*
 **/
void* echo_thread(void* param)
{
	char dst[INET6_ADDRSTRLEN];
	struct client_data * data;
	data = (struct client_data*)param;

	/* RTFM: getnameinfo */
	getnameinfo((struct sockaddr*)&data->addr,
			data->addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

	infoPrint("Connection opened from %s",dst);
	echo_loop(data->sock);
	close(data->sock);
	infoPrint("Connection closed from %s", dst);

	free(data);

	pthread_exit(0);
	return NULL;
}

/**----External Functions*/

/**Function to connect server sockets
 * Call-By-Refferenz Übergabe --> sockets, numsockets
 * Je nach Fall, wird folgendes in sockets geschrieben:
 * Connected	=	Socketummern
 * Error		=	sockets(0) = -1
 * --> Prüfe sockets(0) nach dem Aufruf auf -1 !!!
 **/



void connect_socket_server(int sockets[], int *numsockets, char server[], char service[]){

	struct addrinfo *addr_info, *p, hints;
	int ret;
	*numsockets = 0;
	/**Set the socket at the beginning to -1 and change value if connection works!*/
	sockets[*numsockets] = -1;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	/* RTFM: getaddrinfo */
	ret = getaddrinfo(server, service, &hints, &addr_info);
	if (ret)
	{
		errorPrint("Error in getaddrinfo: %s", gai_strerror(ret));
		exit(1);
	}

	p = addr_info;
	while (p)
	{
		int s;
		char dst[INET6_ADDRSTRLEN];
		char service[INET6_ADDRSTRLEN];
		int on = 1;

		/* Create socket for found family */		
		s = socket(p->ai_family, p->ai_socktype, 0);

		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
			errorPrint("Error in setsockopt: %s", strerror(errno)); /* maybe not so fatal, continue */
		}
		if (p->ai_family == AF_INET6) {
			if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
				errorPrint("Error in setsockopt: %s", strerror(errno));
			}
		}

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
				p->ai_addrlen,
				dst,
				sizeof(dst),
				service,
				sizeof(service),
				NI_NUMERICHOST | NI_NUMERICSERV);

		infoPrint("Trying %s:%s ... ",dst, service);

		/* Try to bind socket */
		if (bind(s, p->ai_addr, p->ai_addrlen) == 0) {
			if (listen(s, 1) < 0) {
				errorPrint("listen failed: %s", strerror(errno));
				close(s);
			} else 	{
				infoPrint("bind successful");
				sockets[*numsockets] = s;
                                *numsockets = *numsockets + 1;
			}
		} else {
			errorPrint("bind failed: %s", strerror(errno));
			close(s);
		}

		p = p->ai_next;
	}
	freeaddrinfo(addr_info);
}

void close_socket_server( int sockets[], int numsockets ){
    int i;
	for (i=0; i<numsockets; i++) {
		close(sockets[i]);
	}
}

/**
 * Wait for connection on all available sockets
 * fd: array of sockets in listen state
 * numfd: number of valid sockets in fd
 **/

void accept_loop_server(int fd[], int numfd)
{
	fd_set set;
	int max, i, ret;

	if (numfd < 1) {
		errorPrint("No sockets available!");
		return;
	}
	while (1) {
		max = 0;
		FD_ZERO(&set);
		for (i=0; i<numfd; i++) {
			if (fd[i] > max)
				max = fd[i];
			FD_SET(fd[i], &set);
		}

		/* wait for first fd that has data */
		ret = select(max+1, &set, NULL, NULL, NULL);
		if (ret <= 0) {
			errorPrint("Error in select: %s", strerror(errno));
			return;
		}
		for (i=0; i<numfd; i++)
			if (FD_ISSET(fd[i], &set)) {
				struct client_data *data;

				data = (struct client_data*)malloc(sizeof(struct client_data));

				data->addrlen = sizeof(data->addr);
				data->sock = accept(fd[i], (struct sockaddr*) &data->addr, &data->addrlen);

				if (data->sock < 0) {
					errorPrint("Error in accept: %s", strerror(errno));
					free(data);
				} else {
					pthread_t thread_id;
					/* Background new connection */
					if((pthread_create(&thread_id, NULL, echo_thread, data))!=0){
						errorPrint("Error while creating thread: %s", strerror(errno));
					};
				}
			}
	}
}

