/* 
 * File:   li_qu_thread.c
 * Author: sysprog
 *
 * Created on November 28, 2011, 5:07 PM
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include "client/common/li_qu_thread.h"
#include "client/common/client_global.h"

static pthread_t question_thr;

pthread_t get_quThread(){
    return question_thr;
}

/**Question change Thread client*/

void* question_thread(){
       
    struct GB_QUESTION_REQUEST qu_rq;
    int ret=0;
    
    /**Write QUESTION_REQUEST to server*/
        
    prepare_message(&qu_rq, TYPE_QU_RQ, 0);
    ret = write(get_socket(),&qu_rq,sizeof(qu_rq.h));
    test_socketOnErrors(ret);
    infoPrint("Write to socket successful!");
    
    /**Wait for 4 seconds*/
    sleep(4);
    
    /**Allow listener-thread to move on*/
    sem_post(&semaphore_socket);
    pthread_exit(0);
    return NULL;
}

/**Listener-Thread client*/

void* listener_thread()
{      
	int ret; /**Used for to save the return value while reading/writing to the socket!*/
        int sock=get_socket();
        int phase=0; /**The current phase of the game -> 0=preparation; 1=game*/
        int ca_rp_counter=0; /**Counts the number of loaded catalogs --> Maximum=10*/
        int z;
        
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
            
	while (1) {
                
                /**Read type and size of the next message*/
            
		ret = read(sock, &net_head.type, sizeof(net_head.type));
                test_socketOnErrors(ret);
                ret = read(sock, &net_head.size, sizeof(net_head.size));
                test_socketOnErrors(ret);
                
                infoPrint("Read from socket successful!");
                infoPrint("NetHead_type: %d!", net_head.type);
                infoPrint("NetHead_size: %d!", ntohs(net_head.size));
                
                switch(net_head.type){
                    
                        /**Case 2: LOGIN REQUEST_OK*/
                        case TYPE_LG_RQ_OK:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 2");
                                
                                /**Read client-ID from server*/
                                
                                ret = read(sock, &lg_rs_ok.client_id, ntohs((net_head.size)));
                                test_socketOnErrors(ret);
                                infoPrint("Client_ID: %d!", lg_rs_ok.client_id);
                                    
                                /**Let the Main/Command-Thread move on*/
                                sem_post(&semaphore_main);
                                
                                /**Wait until GUI-Thread is started*/        
                                sem_wait(&semaphore_socket);
                                
                                /**Set the mode on the preparation window, depending on the client-ID -> game leader?*/
                                if(lg_rs_ok.client_id != 0){
                                        preparation_setMode(PREPARATION_MODE_NORMAL);
                                }else{
                                        preparation_setMode(PREPARATION_MODE_PRIVILEGED);
                                }                                      
                                break;
                                
                        /**Case 4: CATALOG_RESPONSE*/
                        case TYPE_CA_RP:
                                infoPrint("Case 4");
                                
                                if (ca_rp_counter<10){
                                        if ((ntohs((net_head.size)))!=0){
                                        
                                                ca_rp[ca_rp_counter].catalog_msg = (char *)malloc(((ntohs(net_head.size))+1)*sizeof(char));
                                                z=0;
                                        
                                                /**Read catalog name from server*/                                                
                                                while (z<((ntohs(net_head.size)))){
                                                        ret = read(sock, &ca_rp[ca_rp_counter].catalog_msg[z],1);
                                                        test_socketOnErrors(ret);
                                                        z++;
                                                }
                                                /**Set Null-Byte on the strings end*/
                                                ca_rp[ca_rp_counter].catalog_msg[z]='\0';
                                        
                                                infoPrint("Catalog Nr.%d: %s!",ca_rp_counter, ca_rp[ca_rp_counter].catalog_msg);
                                                preparation_addCatalog(ca_rp[ca_rp_counter].catalog_msg);
                                                free(ca_rp[ca_rp_counter].catalog_msg); 
                                        }else{
                                                /**Show the Window, because all catalogs are loaded*/
                                                infoPrint("All catalogs read!");
                                                preparation_showWindow();
                                        }
                                }else{
                                        if(ca_rp_counter==10){
                                                errorPrint("Error: You can not show more then 10 catalogs!");
                                                /**Show the User a message, that there were more then 10 catalogs available, but only 10 loaded*/
                                                if (get_guiruns()!=0){
                                                        guiShowMessageDialog("Es wurde versucht, mehr als 10 Kataloge zu laden!", 0);
                                                }else{
                                                        infoPrint("Es wurde versucht, mehr als 10 Kataloge zu laden!");
                                                }
                                                /**Show the Window, because the max. of catalogs is loaded*/
                                                preparation_showWindow();
                                        }   
                                }
                                ca_rp_counter++;
                                break;
                               
                        /**Case 5: CATALOG_CHANGE*/        
                        case TYPE_CA_CH:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 5");
                                
                                ca_ch.catalog_msg = (char *)malloc(((ntohs(net_head.size))+1)*sizeof(char));
                                z=0;
                                
                                /**Read catalog name from server*/
                                while (z<(ntohs(net_head.size))){
                                    ret = read(sock, &ca_ch.catalog_msg[z],1);
                                    test_socketOnErrors(ret);
                                    z++;
                                }
                                /**Set Null-Byte on the strings end*/
                                ca_ch.catalog_msg[z]='\0';
                                
                                infoPrint("Changed Catalog: %s!",ca_ch.catalog_msg);
                                preparation_selectCatalog(ca_ch.catalog_msg);
                                free(ca_ch.catalog_msg);
                                break;
                        
                        /**Case 6: PLAYER_LIST*/        
                        case TYPE_PL_LI:         
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 6");
                                
                                /**Clear the Playerlist*/
                                if (phase==0){
                                        preparation_clearPlayers();
                                }else{
                                        z=0;                                               
                                        while(z<6){
                                            game_setPlayerName(z+1,NULL);
                                            z++;
                                        }
                                }
                                /**Read the Playerlist from the socket and show it in the GUI*/
                                
                                z=0;
                                while (z<((ntohs((net_head.size)))/37)){
                                
                                        /**Read player name from socket*/
                                        ret = read(sock, &pl_li[z].playername, 32);
                                        test_socketOnErrors(ret);
                                        infoPrint("player name: %s!", pl_li[z].playername);
                                        
                                        /**Read score from socket*/
                                        ret = read(sock, &pl_li[z].score, 4);
                                        test_socketOnErrors(ret);
                                        infoPrint("score: %d!", ntohl(pl_li[z].score));
                                        
                                        /**Read client-ID from socket*/
                                        ret = read(sock, &pl_li[z].client_id, 1);
                                        test_socketOnErrors(ret);
                                        infoPrint("Client_ID: %d!", pl_li[z].client_id);
                                        
                                        /**Switch, depending on the current phase of the game*/
                                        switch(phase){
                                                case 0: /**preparation phase -> add player*/
                                                        preparation_addPlayer(pl_li[z].playername);
                                                        break;
                                                case 1: /**game phase -> add player, score and highlight own player*/
                                                        game_setPlayerName(z+1,pl_li[z].playername);
                                                        game_setPlayerScore(z+1,ntohl(pl_li[z].score));
                                                        if(lg_rs_ok.client_id==pl_li[z].client_id){
                                                            game_highlightPlayer(z+1);
                                                        }
                                                        break;
                                                default:/**error -> unknown phase*/
                                                        errorPrint("Unknown phase --> neither preparation, nor game");
                                                        close_process();
                                                        break;
                                        }                
                                        z++;
                                }
                                break;
                                
                        /**Case 7: START_GAME*/        
                        case TYPE_ST_GA:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 7");
                                
                                if ((ntohs(net_head.size))!=0){
                                        st_ga.catalog_msg = (char *)malloc(((ntohs(net_head.size)))*sizeof(char));
                                
                                        /**Read catalog name from server, if it was send*/
                                        z=0;
                                        while (z<((ntohs(net_head.size)))){
                                                ret = read(sock, &st_ga.catalog_msg[z],1);
                                                test_socketOnErrors(ret);
                                                z++;
                                        }
                                        st_ga.catalog_msg[z]='\0';
                                        infoPrint("Start_Game Message: %s!",st_ga.catalog_msg);
                                        free(st_ga.catalog_msg);
                                }       
                                /**Start the game phase*/
                                infoPrint("Server is ready to start the game! (Start_Game Message received)");
                                preparation_hideWindow();
                                phase=1;
                                game_showWindow();
                                
                                /**Let the Main/Command-Thread move on*/
                                sem_post(&semaphore_main);
                                break;
                        
                        /**Case 9: QUESTION*/        
                        case TYPE_QU:
                                //ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 9");
                                
                                if ((ntohs(net_head.size))!=0){
                                        
                                        /**Reset status icon, status text + unmark answers*/
                                        game_setStatusIcon(STATUS_ICON_NONE);
                                        game_setStatusText("");
                                        game_unmarkAnswers();
                                        
                                        /**Read a question from the socket*/
                                        ret = read(sock, &qu.question, 256);
                                        test_socketOnErrors(ret);
                                        infoPrint("Question: %s!", qu.question);
                                        game_setQuestion(qu.question);
                                        
                                        /**Read the 4 answers from the socket*/
                                        z=0;
                                        while(z<4){
                                                ret = read(sock, &qu.answer[z], 128);
                                                test_socketOnErrors(ret);
                                                infoPrint("Answer[%d]: %s!",z+1, qu.answer[z]);
                                                game_setAnswer(z,qu.answer[z]);
                                                z++;
                                        }
                                        
                                        /**Read the time, the player has, to answer the question*/
                                        ret = read(sock, &qu.time, 2);
                                        test_socketOnErrors(ret);
                                        infoPrint("Question: %d!", ntohs(qu.time));
                                        
                                        /**Enable Buttons*/
                                        game_setAnswerButtonsEnabled(1);
                                }else{
                                    /**No more questions left*/
                                    infoPrint("No more questions left!!!");
                                    game_setStatusIcon(STATUS_ICON_NONE);
                                    game_setStatusText("Alle Fragen gespielt! Warte auf andere Spieler...");
                                    game_unmarkAnswers();
                                    game_setQuestion("");
                                    z=0;
                                    while(z<4){
                                        game_setAnswer(z,"");
                                        z++;
                                    }
                                }
                                break;
                        
                        /**Case 11: QUESTION_RESULT*/        
                        case TYPE_QU_RE:
                                //ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 11");
                                
                                /**Read the given answer from the socket*/
                                ret = read(sock, &qu_re.answer, 1);
                                test_socketOnErrors(ret);
                                infoPrint("Answer was: %d!", qu_re.answer);
                                
                                /**Read the correct answer from the socket*/
                                ret = read(sock, &qu_re.correct, 1);
                                test_socketOnErrors(ret);
                                infoPrint("Correct answer is: %d!", qu_re.correct);
                                
                                /**Show text and icon, depending on the fact if the answer, was right, wrong, or there was a timeout*/
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
                                /**Disable Buttons*/
                                game_setAnswerButtonsEnabled(0);
                                
                                /**Start question change thread*/
                                if((pthread_create(&question_thr, NULL, question_thread, NULL))!=0){
                                        errorPrint("Error while creating question change thread: %s", strerror(errno));
                                }
                                
                                /**Wait until question change thread finished*/
                                sem_wait(&semaphore_socket);
                                break;
                        
                        /**Case 12: GAME_OVER*/         
                        case TYPE_GA_OV:
                                //ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 12");
                                
                                /**Close the window of the game phase*/
                                game_hideWindow();
                                
                                /**Read rank from socket*/
                                ret = read(sock, &ga_ov.rank, 1);
                                test_socketOnErrors(ret);
                                infoPrint("Rank: %d!", ga_ov.rank);
                                
                                /**Prepare and show game end message -> After showing the message quit game*/
                                char message[50];
                                sprintf(message,"Glueckwusch Sie haben den %dten Platz belegt!",ga_ov.rank);
                                infoPrint("Message: %s",message);
                                guiShowMessageDialog(message, 1);
                                
                                /**Block thread, until process gets quit from close_process*/
                                sem_wait(&semaphore_socket);
                                break;
                        
                        /**Case 255: ERROR_WARNING*/        
                        case TYPE_ER_WA:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("Case 255");
                                
                                /**Read type of error message from socket*/
                                ret = read(sock, &er_wa.msg_type, 1);
                                test_socketOnErrors(ret);
                                infoPrint("Errortype: %d!",er_wa.msg_type);
                                
                                er_wa.error_msg = (char *)malloc(((ntohs(net_head.size))-1)*sizeof(char));
                                
                                /**Read error message from socket*/
                                z=0;
                                while (z<((ntohs(net_head.size))-1)){
                                    ret = read(sock, &er_wa.error_msg[z],1);
                                    test_socketOnErrors(ret);
                                    z++;
                                }
                                er_wa.error_msg[z]='\0';
                                
                                /**Switch, depending on error message type*/
                                switch(er_wa.msg_type){
                                        
                                        case 0: /**Usual error message*/
                                                guiShowMessageDialog(er_wa.error_msg, 0);
                                                free(er_wa.error_msg);
                                                break;
                                    case 1: /**Fatal error message*/
                                                if (get_guiruns()!=0){
                                                        guiShowErrorDialog(er_wa.error_msg, 1);
                                                        free(er_wa.error_msg);
                                                        
                                                        /**Block listener thread, until process gets closed*/
                                                        sem_wait(&semaphore_socket);
                                                }else{
                                                        errorPrint("Error: %s!", er_wa.error_msg);
                                                        free(er_wa.error_msg);
                                                        close_process();
                                                }
                                                break;
                                        default:
                                                errorPrint("Error: Unknown case, while printing error/warning message!!!");
                                                close_process();
                                                break;
                                        
                                    }
                                break;
                            
                        default:
                                ca_rp_counter=0; /**set ca_rp_counter to zero*/
                                infoPrint("default Case");
                                close_process();
                                break;
                }
	}
        pthread_exit(0);
	return NULL;
}
