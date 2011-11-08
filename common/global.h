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

    extern struct GB_NET_HEADER {
        int type;
        int length;
    };
    
    extern struct GB_LOGIN_REQUEST {
        struct GB_NET_HEADER h;
        char name[31];
    };


#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

