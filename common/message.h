/* 
 * File:   message.h
 * Author: sysprog
 *
 * Created on November 6, 2011, 11:07 AM
 */

#ifndef MESSAGE_H
#define	MESSAGE_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdint.h>
//#include <arpa/inet.h>
 
    
    //void write_message(int sock, int type);
    void prepare_message(void * hdr, int type, int size);
    void print_message(void * hdr);


#ifdef	__cplusplus
}
#endif

#endif	/* MESSAGE_H */

