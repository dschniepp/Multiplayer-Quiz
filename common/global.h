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

    #define TYPE_LR 1
    #define TYPE_LROK 2

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
    
    struct GB_ErrorWarning{
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

