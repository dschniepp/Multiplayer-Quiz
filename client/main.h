/* 
 * File:   main.h
 * Author: sysprog
 *
 * Created on November 26, 2011, 3:07 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif
    
pthread_t get_liThread();
pthread_t get_guiThread();
int get_socket(void);
int get_guiruns(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

