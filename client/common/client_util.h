/* 
 * File:   util.h
 * Author: sysprog
 *
 * Created on November 28, 2011, 4:23 PM
 */

#ifndef CLIENT_UTIL_H
#define	CLIENT_UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <semaphore.h>  

int init_semaphore(sem_t semaphore);
void prepare_message(void * hdr, int type, int size);
void test_socketOnErrors(int ret);
void close_process();

#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

