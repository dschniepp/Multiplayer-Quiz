/* 
 * File:   global.h
 * Author: sysprog
 *
 * Created on November 8, 2011, 9:00 AM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <errno.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include <sys/un.h>
    #include <signal.h>
    #include <semaphore.h>
    #include "common/message.h"
    #include "common/util.h"
    #include "client/gui/gui_interface.h"
   
    #define TYPE_LG_RQ 1
    #define TYPE_LG_RQ_OK 2
    #define TYPE_CA_RQ 3
    #define TYPE_CA_RP 4
    #define TYPE_CA_CH 5
    #define TYPE_PL_LI 6
    #define TYPE_ST_GA 7
    #define TYPE_QU_RQ 8
    #define TYPE_QU 9
    #define TYPE_QU_AN 10
    #define TYPE_QU_RE 11
    #define TYPE_GA_OV 12
    #define TYPE_ER_WA 255

    #pragma pack(1)
    struct GB_NET_HEADER {
        uint8_t type;
        uint16_t size;
    };
    
    struct GB_LOGIN_REQUEST {
        struct GB_NET_HEADER h;
        char name[31];
    };
    
    struct GB_LOGIN_RESPONSE_OK {
        struct GB_NET_HEADER h;
        uint8_t client_id;
    };
    
    struct GB_CATALOG_REQUEST {
        struct GB_NET_HEADER h;
    };
    
    struct GB_CATALOG_RESPONSE {
        struct GB_NET_HEADER h;
        char *catalog_msg;
    };
    
    struct GB_CATALOG_CHANGE {
        struct GB_NET_HEADER h;
        char *catalog_msg;
    };
    
    struct GB_START_GAME {
        struct GB_NET_HEADER h;
        char *catalog_msg;
    };
    
    struct GB_QUESTION_REQUEST {
        struct GB_NET_HEADER h;
    };
    
    struct GB_QUESTION {
        struct GB_NET_HEADER h;
        char question[256];
        char answer[128][4];
        int time;
    };
    
    
    
    struct GB_Player_List{
        struct GB_NET_HEADER h;
        char playername[32];
        unsigned int score;
        uint8_t client_id;
    };
    
    struct GB_Error_Warning{
        struct GB_NET_HEADER h;
        uint8_t msg_type;
        char *error_msg;
    };
    //struct GB_LOGIN_REQUEST GB_LR;
    
    #pragma pack(0)

    
#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

