/* 
 * File:   main.h
 * Author: sysprog
 *
 * Created on November 22, 2011, 11:18 AM
 */



#ifndef TEST_H
#define	TEST_H

#ifdef TEST
int stdPipe[2]={0,0};
#else
extern int stdPipe[2]={0,0};
#endif


#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

